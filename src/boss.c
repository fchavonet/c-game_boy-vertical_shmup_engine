#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "boss.h"
#include "player.h"
#include "player_shots.h"
#include "enemy_shots.h"
#include "score.h"

#define BOSS_WIDTH 16
#define BOSS_HEIGHT 16

#define BOSS_TARGET_Y 24

#define BOSS_MIN_X 16
#define BOSS_MAX_X \
    (GAME_PLAYFIELD_WIDTH - BOSS_WIDTH - BOSS_MIN_X)

#define BOSS_MIN_Y 16
#define BOSS_MAX_Y 64

#define BOSS_HIT_FLASH_DURATION 6
#define BOSS_SCORE_VALUE 1000

#define BOSS_DESTRUCTION_DURATION 48
#define BOSS_EXPLOSION_FRAME_DURATION 4u
#define BOSS_EXPLOSION_PART_DELAY 3

typedef enum
{
    BOSS_WAITING,
    BOSS_ENTERING,
    BOSS_FIGHTING,
    BOSS_DESTROYING,
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
static uint8_t destruction_timer;

static const BossSequence *phase_sequences[2];
static const BossSequence *active_sequence;
static const BossAction *active_action;
static uint16_t action_remaining;
static uint8_t action_index;
static uint8_t action_salvos;
static uint8_t active_phase;
static uint8_t invalid_sequence_count;

static const uint8_t boss_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF};

/*
 * Execute the attack selected by the current phase.
 */
static void boss_fire(BossShotMode mode, uint8_t speed)
{
    switch (mode)
    {
    case BOSS_SHOT_STRAIGHT:
        enemy_shots_spawn(
            boss_x + 6,
            boss_y + BOSS_HEIGHT,
            speed);
        break;

    case BOSS_SHOT_DOUBLE:
        enemy_shots_spawn(
            boss_x + 1,
            boss_y + BOSS_HEIGHT,
            speed);

        enemy_shots_spawn(
            boss_x + 11,
            boss_y + BOSS_HEIGHT,
            speed);
        break;

    case BOSS_SHOT_AIMED:
        if (
            player_is_alive() &&
            !player_is_destroying())
        {
            enemy_shots_spawn_aimed(
                boss_x + 6,
                boss_y + BOSS_HEIGHT,
                player_get_center_x(),
                player_get_center_y(),
                speed);
        }
        break;

    case BOSS_SHOT_SPREAD:
        enemy_shots_spawn_spread(
            boss_x + 6,
            boss_y + BOSS_HEIGHT,
            speed);
        break;

    case BOSS_SHOT_NONE:
    default:
        break;
    }
}

/* Checked once when the boss starts, not inside the frame loop. */
static uint8_t boss_sequence_valid(const BossSequence *sequence)
{
    uint8_t i;
    const BossAction *action;

    if (sequence->actions == 0 || sequence->action_count == 0 ||
        sequence->action_count > BOSS_SEQUENCE_MAX_ACTIONS)
    {
        return 0;
    }
    for (i = 0; i < sequence->action_count; i++)
    {
        action = &sequence->actions[i];
        if (action->duration == 0 || action->movement > BOSS_MOVE_DOWN ||
            action->speed > 4u || action->shot_mode > BOSS_SHOT_SPREAD)
        {
            return 0;
        }
        if (action->shot_mode != BOSS_SHOT_NONE &&
            (action->shot_interval == 0 || action->shot_speed < SHOT_SPEED_MIN ||
             action->shot_speed > SHOT_SPEED_MAX))
        {
            return 0;
        }
    }
    return 1;
}

static void boss_begin_action(void)
{
    active_action = &active_sequence->actions[action_index];
    action_remaining = active_action->duration;
    action_salvos = 0;
    shot_timer = active_action->first_shot_delay;
}

static void boss_sequence_update(void)
{
    static uint8_t speed;

    if (action_remaining == 0)
    {
        action_index++;
        if (action_index >= active_sequence->action_count)
        {
            action_index = 0;
        }
        boss_begin_action();
    }

    speed = active_action->speed;
    switch (active_action->movement)
    {
    case BOSS_MOVE_SWEEP:
        if (direction_x < 0)
        {
            boss_x -= speed;
        }
        else
        {
            boss_x += speed;
        }
        break;
    case BOSS_MOVE_LEFT:
        boss_x -= speed;
        direction_x = -1;
        break;
    case BOSS_MOVE_RIGHT:
        boss_x += speed;
        direction_x = 1;
        break;
    case BOSS_MOVE_UP:
        boss_y -= speed;
        break;
    case BOSS_MOVE_DOWN:
        boss_y += speed;
        break;
    default:
        break;
    }

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
    if (boss_y < BOSS_MIN_Y)
    {
        boss_y = BOSS_MIN_Y;
    }
    else if (boss_y > BOSS_MAX_Y)
    {
        boss_y = BOSS_MAX_Y;
    }

    action_remaining--;
    if (active_action->shot_mode == BOSS_SHOT_NONE)
    {
        return;
    }
    if (active_action->salvos != 0 && action_salvos >= active_action->salvos)
    {
        return;
    }
    if (shot_timer > 0)
    {
        shot_timer--;
    }
    if (shot_timer == 0)
    {
        boss_fire(active_action->shot_mode, active_action->shot_speed);
        shot_timer = active_action->shot_interval;
        if (active_action->salvos != 0)
        {
            action_salvos++;
        }
    }
}

uint8_t boss_get_invalid_sequence_count(void)
{
    return invalid_sequence_count;
}

void boss_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    active_sequence = 0;
    active_action = 0;
    phase_sequences[0] = 0;
    phase_sequences[1] = 0;
    action_remaining = 0;
    action_index = 0;
    action_salvos = 0;
    active_phase = 255u;
    invalid_sequence_count = 0;

    boss_definition = 0;
    boss_state = BOSS_WAITING;

    boss_x = 0;
    boss_y = 0;
    direction_x = 1;

    boss_hp = 0;
    shot_timer = 0;
    hit_flash_timer = 0;
    destruction_timer = 0;

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
    uint8_t i;
    if (boss_state != BOSS_WAITING)
    {
        return;
    }

    if (definition == 0)
    {
        return;
    }

    if (definition->start_hp == 0)
    {
        return;
    }

    boss_definition = definition;
    phase_sequences[0] = definition->phase_one_sequence;
    phase_sequences[1] = definition->phase_two_sequence;
    invalid_sequence_count = 0;
    for (i = 0; i < 2u; i++)
    {
        if (phase_sequences[i] != 0 && !boss_sequence_valid(phase_sequences[i]))
        {
            phase_sequences[i] = 0;
            invalid_sequence_count++;
        }
    }
    active_phase = 255u;
    active_sequence = 0;
    active_action = 0;

    boss_x = (GAME_PLAYFIELD_WIDTH - BOSS_WIDTH) / 2;
    boss_y = -BOSS_HEIGHT;

    direction_x = 1;

    boss_hp = boss_definition->start_hp;
    shot_timer = boss_definition->phase_one_shot_interval;
    hit_flash_timer = 0;
    destruction_timer = 0;

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
    BossShotMode shot_mode;
    uint8_t shot_speed;
    uint8_t phase;

    if (
        boss_state == BOSS_WAITING ||
        boss_state == BOSS_DEFEATED)
    {
        return;
    }

    if (boss_state == BOSS_DESTROYING)
    {
        destruction_timer++;

        if (destruction_timer >= BOSS_DESTRUCTION_DURATION)
        {
            boss_state = BOSS_DEFEATED;
        }

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
            boss_state = BOSS_DESTROYING;

            destruction_timer = 0;
            hit_flash_timer = 0;

            score_add(BOSS_SCORE_VALUE);
            return;
        }
    }

    phase = 0;
    if (boss_hp <= boss_definition->phase_two_hp)
    {
        phase = 1;
    }
    if (phase != active_phase)
    {
        active_phase = phase;
        active_sequence = phase_sequences[phase];
        action_index = 0;
        if (active_sequence != 0)
        {
            boss_begin_action();
        }
        else
        {
            shot_timer = boss_definition->phase_one_shot_interval;
            if (phase == 1)
            {
                shot_timer = boss_definition->phase_two_shot_interval;
            }
        }
    }
    if (active_sequence != 0)
    {
        boss_sequence_update();
        return;
    }

    /*
     * Select all parameters from the current phase.
     */
    speed = boss_definition->phase_one_speed;
    shot_interval = boss_definition->phase_one_shot_interval;
    shot_mode = boss_definition->phase_one_shot_mode;
    shot_speed = boss_definition->phase_one_shot_speed;

    if (boss_hp <= boss_definition->phase_two_hp)
    {
        speed = boss_definition->phase_two_speed;
        shot_interval = boss_definition->phase_two_shot_interval;
        shot_mode = boss_definition->phase_two_shot_mode;
        shot_speed = boss_definition->phase_two_shot_speed;

        if (shot_timer > shot_interval)
        {
            shot_timer = shot_interval;
        }
    }

    if (direction_x < 0)
    {
        boss_x -= speed;
    }
    else
    {
        boss_x += speed;
    }

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

    /*
     * Allow phases without shooting.
     */
    if (shot_mode == BOSS_SHOT_NONE || shot_interval == 0)
    {
        shot_timer = shot_interval;
        return;
    }

    if (shot_timer > 0)
    {
        shot_timer--;
    }

    if (shot_timer == 0)
    {
        boss_fire(shot_mode, shot_speed);
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
    uint8_t delay;
    uint8_t age;
    uint8_t frame;
    uint8_t tile_id;

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
            continue;
        }

        if (boss_state == BOSS_DESTROYING)
        {
            delay = i * BOSS_EXPLOSION_PART_DELAY;

            if (destruction_timer < delay)
            {
                move_sprite(sprite_id, 0, 0);
                continue;
            }

            age = destruction_timer - delay;

            frame = (uint8_t)((age / BOSS_EXPLOSION_FRAME_DURATION) %
                              GFX_EXPLOSION_TILE_COUNT);

            tile_id = (uint8_t)(GFX_EXPLOSION_FIRST_TILE_ID + frame);

            set_sprite_tile(sprite_id, tile_id);
        }
        else
        {
            set_sprite_tile(sprite_id, GFX_BOSS_FIRST_TILE_ID);
        }

        move_sprite(
            sprite_id,
            (uint8_t)(part_x + GFX_SPRITE_OFFSET_X),
            (uint8_t)(part_y + GFX_SPRITE_OFFSET_Y));
    }
}
