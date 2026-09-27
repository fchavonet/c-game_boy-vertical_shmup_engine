#include <gb/gb.h>
#include <stdint.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "enemy.h"
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

    uint8_t active;
    uint8_t hp;
    uint8_t hit_flash_timer;

    EnemyMovement movement;
    int8_t direction_x;
    uint8_t movement_timer;
    uint8_t shot_timer;
} Enemy;

static Enemy enemies[ENEMY_COUNT];

/*
 * Shared enemy definitions.
 */

const EnemyDefinition enemy_standard = {
    1,   /* Initial health */
    1,   /* Movement speed */
    45,  /* First shot delay */
    60,  /* Shot interval */
    100, /* Score value */
    GFX_ENEMY_TILE_ID};

const EnemyDefinition enemy_resistant = {
    3,   /* Initial health */
    1,   /* Movement speed */
    30,  /* First shot delay */
    45,  /* Shot interval */
    250, /* Score value */
    GFX_ENEMY_RESISTANT_TILE_ID};

/*
 * Placeholder graphics.
 */

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

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        enemies[i].x = 0;
        enemies[i].y = 0;
        enemies[i].definition = 0;
        enemies[i].active = 0;
        enemies[i].hp = 0;
        enemies[i].hit_flash_timer = 0;
        enemies[i].movement = ENEMY_MOVE_DOWN;
        enemies[i].direction_x = 0;
        enemies[i].movement_timer = 0;
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
    const EnemyDefinition *definition)
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
            enemies[i].hp = definition->start_hp;
            enemies[i].hit_flash_timer = 0;

            enemies[i].movement = movement;
            enemies[i].movement_timer = 0;
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

            enemies[i].active = 1;
            return 1;
        }
    }

    return 0;
}

void enemy_update(void)
{
    uint8_t i;
    const EnemyDefinition *definition;

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (!enemies[i].active)
        {
            continue;
        }

        definition = enemies[i].definition;

        if (enemies[i].hit_flash_timer > 0)
        {
            enemies[i].hit_flash_timer--;
        }

        enemies[i].y += definition->speed;

        enemies[i].x +=
            enemies[i].direction_x * definition->speed;

        if (enemies[i].movement == ENEMY_MOVE_ZIGZAG)
        {
            enemies[i].movement_timer++;

            if (
                enemies[i].movement_timer >=
                ENEMY_ZIGZAG_INTERVAL)
            {
                enemies[i].movement_timer = 0;

                enemies[i].direction_x =
                    -enemies[i].direction_x;
            }
        }

        if (
            enemies[i].y >= GAME_PLAYFIELD_HEIGHT ||
            enemies[i].x <= -ENEMY_WIDTH ||
            enemies[i].x >= GAME_PLAYFIELD_WIDTH)
        {
            enemies[i].active = 0;
            continue;
        }

        /*
         * Damage remains enabled during the hit flash.
         */
        if (shots_hit(
                enemies[i].x,
                enemies[i].y,
                ENEMY_WIDTH,
                ENEMY_HEIGHT))
        {
            enemies[i].hp--;

            if (enemies[i].hp == 0)
            {
                enemies[i].active = 0;

                effects_spawn_explosion(
                    enemies[i].x,
                    enemies[i].y);

                score_add(definition->score_value);

                powerup_on_enemy_destroyed(
                    enemies[i].x,
                    enemies[i].y);

                continue;
            }

            enemies[i].hit_flash_timer =
                ENEMY_HIT_FLASH_DURATION;
        }

        /*
         * A zero interval disables shooting.
         */
        if (definition->shot_interval == 0)
        {
            continue;
        }

        if (enemies[i].shot_timer > 0)
        {
            enemies[i].shot_timer--;
        }

        if (enemies[i].shot_timer == 0)
        {
            enemy_shots_spawn(
                enemies[i].x + 2,
                enemies[i].y + ENEMY_HEIGHT);

            enemies[i].shot_timer = definition->shot_interval;
        }
    }
}

void enemy_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        sprite_id = GFX_ENEMY_FIRST_SPRITE_ID + i;

        if (!enemies[i].active)
        {
            move_sprite(sprite_id, 0, 0);
            continue;
        }

        /*
         * Hide the sprite on alternating frames.
         * The enemy remains active in the simulation.
         */
        if (
            enemies[i].hit_flash_timer > 0 &&
            (enemies[i].hit_flash_timer & 1u) == 0u)
        {
            move_sprite(sprite_id, 0, 0);
            continue;
        }

        set_sprite_tile(
            sprite_id,
            enemies[i].definition->tile_id);

        move_sprite(
            sprite_id,
            (uint8_t)(enemies[i].x + GFX_SPRITE_OFFSET_X),
            (uint8_t)(enemies[i].y + GFX_SPRITE_OFFSET_Y));
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
    uint8_t i;

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (enemies[i].active)
        {
            if (
                enemies[i].x < x + width &&
                enemies[i].x + ENEMY_WIDTH > x &&
                enemies[i].y < y + height &&
                enemies[i].y + ENEMY_HEIGHT > y)
            {
                enemies[i].active = 0;
                return 1;
            }
        }
    }

    return 0;
}
