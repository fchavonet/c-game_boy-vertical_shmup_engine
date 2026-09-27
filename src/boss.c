#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "boss.h"
#include "player_shots.h"
#include "enemy_shots.h"
#include "score.h"

#define BOSS_WIDTH 16
#define BOSS_HEIGHT 16

#define BOSS_TARGET_Y 24

#define BOSS_MIN_X 16
#define BOSS_MAX_X \
    (GAME_PLAYFIELD_WIDTH - BOSS_WIDTH - BOSS_MIN_X)

#define BOSS_HIT_FLASH_DURATION 6
#define BOSS_SCORE_VALUE 1000

typedef enum
{
    BOSS_WAITING,
    BOSS_ENTERING,
    BOSS_FIGHTING,
    BOSS_DEFEATED
} BossState;

static const BossDefinition *boss_definition;

static BossState boss_state;

static int16_t boss_x;
static int16_t boss_y;

static int8_t direction_x;

static uint8_t boss_hp;
static uint8_t shot_timer;
static uint8_t hit_flash_timer;

static const uint8_t boss_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF};

void boss_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    boss_definition = 0;
    boss_state = BOSS_WAITING;

    boss_x = 0;
    boss_y = 0;
    direction_x = 1;

    boss_hp = 0;
    shot_timer = 0;
    hit_flash_timer = 0;

    set_sprite_data(
        GFX_BOSS_FIRST_TILE_ID,
        GFX_BOSS_TILE_COUNT,
        boss_tile);

    for (i = 0; i < GFX_BOSS_SPRITE_COUNT; i++)
    {
        sprite_id = GFX_BOSS_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, GFX_BOSS_FIRST_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void boss_start(const BossDefinition *definition)
{
    if (boss_state != BOSS_WAITING)
    {
        return;
    }

    boss_definition = definition;

    boss_x = (GAME_PLAYFIELD_WIDTH - BOSS_WIDTH) / 2;
    boss_y = -BOSS_HEIGHT;

    direction_x = 1;

    boss_hp = boss_definition->start_hp;
    shot_timer = boss_definition->phase_one_shot_interval;
    hit_flash_timer = 0;

    boss_state = BOSS_ENTERING;
}

uint8_t boss_is_defeated(void)
{
    return boss_state == BOSS_DEFEATED;
}

void boss_update(void)
{
    uint8_t speed;
    uint8_t shot_interval;

    if (
        boss_state == BOSS_WAITING ||
        boss_state == BOSS_DEFEATED)
    {
        return;
    }

    if (boss_state == BOSS_ENTERING)
    {
        boss_y++;

        if (boss_y >= BOSS_TARGET_Y)
        {
            boss_y = BOSS_TARGET_Y;
            boss_state = BOSS_FIGHTING;
        }

        return;
    }

    if (hit_flash_timer > 0)
    {
        hit_flash_timer--;
    }

    if (shots_hit(
            boss_x,
            boss_y,
            BOSS_WIDTH,
            BOSS_HEIGHT))
    {
        boss_hp--;
        hit_flash_timer = BOSS_HIT_FLASH_DURATION;

        if (boss_hp == 0)
        {
            boss_state = BOSS_DEFEATED;
            score_add(BOSS_SCORE_VALUE);
            return;
        }
    }

    speed = boss_definition->phase_one_speed;
    shot_interval = boss_definition->phase_one_shot_interval;

    if (boss_hp <= boss_definition->phase_two_hp)
    {
        speed = boss_definition->phase_two_speed;
        shot_interval = boss_definition->phase_two_shot_interval;

        if (shot_timer > shot_interval)
        {
            shot_timer = shot_interval;
        }
    }

    boss_x += direction_x * speed;

    if (boss_x <= BOSS_MIN_X)
    {
        boss_x = BOSS_MIN_X;
        direction_x = 1;
    }
    else if (boss_x >= BOSS_MAX_X)
    {
        boss_x = BOSS_MAX_X;
        direction_x = -1;
    }

    if (shot_timer > 0)
    {
        shot_timer--;
    }

    if (shot_timer == 0)
    {
        if (boss_hp > boss_definition->phase_two_hp)
        {
            enemy_shots_spawn(
                boss_x + 6,
                boss_y + BOSS_HEIGHT);
        }
        else
        {
            enemy_shots_spawn(
                boss_x + 1,
                boss_y + BOSS_HEIGHT);

            enemy_shots_spawn(
                boss_x + 11,
                boss_y + BOSS_HEIGHT);
        }

        shot_timer = shot_interval;
    }
}

uint8_t boss_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    if (
        boss_state != BOSS_ENTERING &&
        boss_state != BOSS_FIGHTING)
    {
        return 0;
    }

    if (
        boss_x < x + width &&
        boss_x + BOSS_WIDTH > x &&
        boss_y < y + height &&
        boss_y + BOSS_HEIGHT > y)
    {
        return 1;
    }

    return 0;
}

void boss_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    int16_t part_x;
    int16_t part_y;

    if (
        boss_state == BOSS_WAITING ||
        boss_state == BOSS_DEFEATED ||
        (hit_flash_timer & 1))
    {
        for (i = 0; i < GFX_BOSS_SPRITE_COUNT; i++)
        {
            move_sprite(GFX_BOSS_FIRST_SPRITE_ID + i, 0, 0);
        }

        return;
    }

    for (i = 0; i < GFX_BOSS_SPRITE_COUNT; i++)
    {
        sprite_id = GFX_BOSS_FIRST_SPRITE_ID + i;

        part_x = boss_x;
        part_y = boss_y;

        if (i & 1)
        {
            part_x += 8;
        }

        if (i >= 2)
        {
            part_y += 8;
        }

        if (part_y <= -8 || part_y >= GAME_PLAYFIELD_HEIGHT)
        {
            move_sprite(sprite_id, 0, 0);
        }
        else
        {
            move_sprite(
                sprite_id,
                (uint8_t)(part_x + GFX_SPRITE_OFFSET_X),
                (uint8_t)(part_y + GFX_SPRITE_OFFSET_Y));
        }
    }
}
