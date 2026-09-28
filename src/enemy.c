#include <gb/gb.h>
#include <stdint.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "enemy.h"
#include "player.h"
#include "player_shots.h"
#include "score.h"
#include "enemy_shots.h"
#include "powerup.h"
#include "effects.h"

#define ENEMY_COUNT GFX_ENEMY_SPRITE_COUNT

#define ENEMY_WIDTH 8
#define ENEMY_HEIGHT 8

#define ENEMY_ZIGZAG_INTERVAL 32
#define ENEMY_HIT_FLASH_DURATION 6

typedef struct
{
    int16_t x;
    int16_t y;

    const EnemyDefinition *definition;
    const EnemyPath *path;

    uint8_t active;
    uint8_t hp;
    uint8_t hit_flash_timer;

    EnemyMovement movement;
    int8_t direction_x;

    uint16_t movement_timer;
    uint8_t movement_step;

    uint8_t shot_timer;
} Enemy;

static Enemy enemies[ENEMY_COUNT];

const EnemyDefinition enemy_standard = {
    1, /* Initial health */
    1, /* Movement speed */

    ENEMY_SHOT_NONE,
    0, /* First shot delay: unused */
    0, /* Shot interval: unused */
    0, /* Shot speed: unused */

    100, /* Score value */
    GFX_ENEMY_TILE_ID};

const EnemyDefinition enemy_resistant = {
    3, /* Initial health */
    1, /* Movement speed */

    ENEMY_SHOT_AIMED,
    30, /* First shot delay */
    45, /* Shot interval */
    SHOT_SPEED_1_5,

    250, /* Score value */
    GFX_ENEMY_RESISTANT_TILE_ID};

const EnemyDefinition enemy_spread = {
    2, /* Initial health */
    1, /* Movement speed */

    ENEMY_SHOT_SPREAD,
    45, /* First shot delay */
    90, /* Shot interval */
    SHOT_SPEED_1_75,

    200, /* Score value */
    GFX_ENEMY_SPREAD_TILE_ID};

static const uint8_t enemy_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF};

static const uint8_t resistant_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xC3, 0xC3,
    0xC3, 0xC3,
    0xC3, 0xC3,
    0xC3, 0xC3,
    0xFF, 0xFF,
    0xFF, 0xFF};

static const uint8_t spread_tile[] = {
    0x18, 0x18,
    0x3C, 0x3C,
    0x7E, 0x7E,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0x7E, 0x7E,
    0x3C, 0x3C,
    0x18, 0x18};

/*
 * Execute the path selected by the spawn event.
 */
static uint8_t enemy_update_sequence(Enemy *enemy)
{
    static const EnemyPath *path;
    static const EnemyMovementStep *step;
    static uint8_t can_shoot;

    path = enemy->path;

    /*
     * Advance past completed or zero-duration steps.
     */
    while (enemy->movement_step < path->step_count)
    {
        step = &path->steps[enemy->movement_step];

        if (enemy->movement_timer < step->duration)
        {
            break;
        }

        enemy->movement_timer = 0;
        enemy->movement_step++;
    }

    if (enemy->movement_step >= path->step_count)
    {
        enemy->active = 0;
        return 0;
    }

    step = &path->steps[enemy->movement_step];

    if (step->direction_x < 0)
    {
        enemy->x -= enemy->definition->speed;
    }
    else if (step->direction_x > 0)
    {
        enemy->x += enemy->definition->speed;
    }

    if (step->direction_y < 0)
    {
        enemy->y -= enemy->definition->speed;
    }
    else if (step->direction_y > 0)
    {
        enemy->y += enemy->definition->speed;
    }

    can_shoot = step->can_shoot;

    enemy->movement_timer++;

    return can_shoot;
}

static uint8_t enemy_update_movement(Enemy *enemy)
{
    if (enemy->movement == ENEMY_MOVE_SEQUENCE)
    {
        return enemy_update_sequence(enemy);
    }

    /*
     * Existing continuous movements.
     */
    enemy->y += enemy->definition->speed;

    if (enemy->direction_x < 0)
    {
        enemy->x -= enemy->definition->speed;
    }
    else if (enemy->direction_x > 0)
    {
        enemy->x += enemy->definition->speed;
    }

    if (enemy->movement == ENEMY_MOVE_ZIGZAG)
    {
        enemy->movement_timer++;

        if (enemy->movement_timer >= ENEMY_ZIGZAG_INTERVAL)
        {
            enemy->movement_timer = 0;
            enemy->direction_x = -enemy->direction_x;
        }
    }

    return 1;
}

static void enemy_update_shooting(Enemy *enemy)
{
    static const EnemyDefinition *definition;

    definition = enemy->definition;

    if (definition->shot_mode == ENEMY_SHOT_NONE)
    {
        return;
    }

    if (definition->shot_interval == 0)
    {
        return;
    }

    if (enemy->shot_timer > 0)
    {
        enemy->shot_timer--;
    }

    if (enemy->shot_timer > 0)
    {
        return;
    }

    switch (definition->shot_mode)
    {
    case ENEMY_SHOT_STRAIGHT:
        enemy_shots_spawn(
            enemy->x + 2,
            enemy->y + ENEMY_HEIGHT,
            definition->shot_speed);
        break;

    case ENEMY_SHOT_AIMED:
        if (
            player_is_alive() &&
            !player_is_destroying())
        {
            enemy_shots_spawn_aimed(
                enemy->x + 2,
                enemy->y + ENEMY_HEIGHT,
                player_get_center_x(),
                player_get_center_y(),
                definition->shot_speed);
        }
        break;

    case ENEMY_SHOT_SPREAD:
        enemy_shots_spawn_spread(
            enemy->x + 2,
            enemy->y + ENEMY_HEIGHT,
            definition->shot_speed);
        break;

    default:
        return;
    }

    enemy->shot_timer = definition->shot_interval;
}

void enemy_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    set_sprite_data(
        GFX_ENEMY_TILE_ID,
        1,
        enemy_tile);

    set_sprite_data(
        GFX_ENEMY_RESISTANT_TILE_ID,
        1,
        resistant_tile);

    set_sprite_data(
        GFX_ENEMY_SPREAD_TILE_ID,
        1,
        spread_tile);

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        enemies[i].x = 0;
        enemies[i].y = 0;

        enemies[i].definition = 0;
        enemies[i].path = 0;

        enemies[i].active = 0;
        enemies[i].hp = 0;
        enemies[i].hit_flash_timer = 0;

        enemies[i].movement = ENEMY_MOVE_DOWN;
        enemies[i].direction_x = 0;
        enemies[i].movement_timer = 0;
        enemies[i].movement_step = 0;

        enemies[i].shot_timer = 0;

        sprite_id = GFX_ENEMY_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, GFX_ENEMY_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

uint8_t enemy_spawn(
    uint8_t x,
    uint8_t y,
    EnemyMovement movement,
    const EnemyDefinition *definition,
    const EnemyPath *path)
{
    uint8_t i;

    if (definition == 0)
    {
        return 0;
    }

    if (
        definition->start_hp == 0 ||
        definition->speed == 0)
    {
        return 0;
    }

    if (x > GAME_PLAYFIELD_WIDTH - ENEMY_WIDTH)
    {
        return 0;
    }

    if (y > GAME_PLAYFIELD_HEIGHT - ENEMY_HEIGHT)
    {
        return 0;
    }

    switch (movement)
    {
    case ENEMY_MOVE_DOWN:
    case ENEMY_MOVE_DIAGONAL_LEFT:
    case ENEMY_MOVE_DIAGONAL_RIGHT:
    case ENEMY_MOVE_ZIGZAG:
        break;

    case ENEMY_MOVE_SEQUENCE:
        if (path == 0)
        {
            return 0;
        }

        if (path->steps == 0 || path->step_count == 0)
        {
            return 0;
        }
        break;

    default:
        return 0;
    }

    switch (definition->shot_mode)
    {
    case ENEMY_SHOT_NONE:
    case ENEMY_SHOT_STRAIGHT:
    case ENEMY_SHOT_AIMED:
    case ENEMY_SHOT_SPREAD:
        break;

    default:
        return 0;
    }

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].x = x;
            enemies[i].y = y;

            enemies[i].definition = definition;
            enemies[i].path = path;

            enemies[i].hp = definition->start_hp;
            enemies[i].hit_flash_timer = 0;

            enemies[i].movement = movement;
            enemies[i].movement_timer = 0;
            enemies[i].movement_step = 0;

            enemies[i].shot_timer = definition->first_shot_delay;
            enemies[i].direction_x = 0;

            switch (movement)
            {
            case ENEMY_MOVE_DIAGONAL_LEFT:
                enemies[i].direction_x = -1;
                break;

            case ENEMY_MOVE_DIAGONAL_RIGHT:
            case ENEMY_MOVE_ZIGZAG:
                enemies[i].direction_x = 1;
                break;

            default:
                break;
            }

            set_sprite_tile(
                GFX_ENEMY_FIRST_SPRITE_ID + i,
                definition->tile_id);

            enemies[i].active = 1;
            return 1;
        }
    }

    return 0;
}

void enemy_update(void)
{
    static uint8_t i;
    static Enemy *enemy;
    static uint8_t can_shoot;
    static const EnemyDefinition *definition;

    for (i = 0, enemy = enemies; i < ENEMY_COUNT; i++, enemy++)
    {
        if (!enemy->active)
        {
            continue;
        }

        definition = enemy->definition;

        if (enemy->hit_flash_timer > 0)
        {
            enemy->hit_flash_timer--;
        }

        can_shoot = enemy_update_movement(enemy);

        if (!enemy->active)
        {
            continue;
        }

        if (
            enemy->y <= -ENEMY_HEIGHT ||
            enemy->y >= GAME_PLAYFIELD_HEIGHT ||
            enemy->x <= -ENEMY_WIDTH ||
            enemy->x >= GAME_PLAYFIELD_WIDTH)
        {
            enemy->active = 0;
            continue;
        }

        if (shots_hit(
                enemy->x,
                enemy->y,
                ENEMY_WIDTH,
                ENEMY_HEIGHT))
        {
            enemy->hp--;

            if (enemy->hp == 0)
            {
                enemy->active = 0;

                effects_spawn_explosion(
                    enemy->x,
                    enemy->y);

                score_add(definition->score_value);

                powerup_on_enemy_destroyed(
                    enemy->x,
                    enemy->y);

                continue;
            }

            enemy->hit_flash_timer =
                ENEMY_HIT_FLASH_DURATION;
        }

        if (can_shoot)
        {
            enemy_update_shooting(enemy);
        }
        else
        {
            enemy->shot_timer =
                definition->first_shot_delay;
        }
    }
}

void enemy_render(void)
{
    static uint8_t i;
    static Enemy *enemy;
    static volatile OAM_item_t *sprite;

    enemy = enemies;
    sprite = &shadow_OAM[GFX_ENEMY_FIRST_SPRITE_ID];

    for (i = 0; i < ENEMY_COUNT; i++, enemy++, sprite++)
    {
        if (!enemy->active)
        {
            sprite->y = 0;
            continue;
        }

        if (
            enemy->hit_flash_timer > 0 &&
            (enemy->hit_flash_timer & 1u) == 0u)
        {
            sprite->y = 0;
            continue;
        }

        sprite->y = (uint8_t)(enemy->y + GFX_SPRITE_OFFSET_Y);
        sprite->x = (uint8_t)(enemy->x + GFX_SPRITE_OFFSET_X);
    }
}

uint8_t enemy_is_clear(void)
{
    uint8_t i;

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (enemies[i].active)
        {
            return 0;
        }
    }

    return 1;
}

uint8_t enemy_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    static uint8_t i;
    static Enemy *enemy;
    static uint8_t origin_x;
    static uint8_t origin_y;
    static uint8_t extent_x;
    static uint8_t extent_y;

    if (width == 0 || height == 0)
    {
        return 0;
    }

    origin_x = (uint8_t)(x - ENEMY_WIDTH + 1);
    origin_y = (uint8_t)(y - ENEMY_HEIGHT + 1);
    extent_x = (uint8_t)(width + ENEMY_WIDTH - 1u);
    extent_y = (uint8_t)(height + ENEMY_HEIGHT - 1u);

    for (i = 0, enemy = enemies; i < ENEMY_COUNT; i++, enemy++)
    {
        if (!enemy->active)
        {
            continue;
        }

        if (
            (uint8_t)((uint8_t)enemy->x - origin_x) < extent_x &&
            (uint8_t)((uint8_t)enemy->y - origin_y) < extent_y)
        {
            enemy->active = 0;
            return 1;
        }
    }

    return 0;
}
