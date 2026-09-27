#include <gb/gb.h>
#include <stdint.h>

#include "enemy.h"
#include "shots.h"
#include "score.h"

#define ENEMY_COUNT 6

#define ENEMY_WIDTH 8
#define ENEMY_HEIGHT 8

#define ENEMY_SPEED 1
#define ENEMY_ZIGZAG_INTERVAL 32

#define ENEMY_FIRST_SPRITE_ID 9
#define ENEMY_TILE_ID 2

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

#define ENEMY_SCORE_VALUE 100

typedef struct
{
    int16_t x;
    int16_t y;
    uint8_t active;
    EnemyMovement movement;
    int8_t direction_x;
    uint8_t movement_timer;
} Enemy;

static Enemy enemies[ENEMY_COUNT];

static const uint8_t enemy_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF};

void enemy_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    set_sprite_data(ENEMY_TILE_ID, 1, enemy_tile);

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        enemies[i].x = 0;
        enemies[i].y = 0;
        enemies[i].active = 0;
        enemies[i].movement = ENEMY_MOVE_DOWN;
        enemies[i].direction_x = 0;
        enemies[i].movement_timer = 0;

        sprite_id = ENEMY_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, ENEMY_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

uint8_t enemy_spawn(
    uint8_t x,
    uint8_t y,
    EnemyMovement movement)
{
    uint8_t i;

    if (x > SCREENWIDTH - ENEMY_WIDTH)
    {
        return 0;
    }

    if (y > SCREENHEIGHT - ENEMY_HEIGHT)
    {
        return 0;
    }

    if (movement > ENEMY_MOVE_ZIGZAG)
    {
        return 0;
    }

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].x = x;
            enemies[i].y = y;
            enemies[i].movement = movement;
            enemies[i].movement_timer = 0;
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

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        if (!enemies[i].active)
        {
            continue;
        }

        enemies[i].y += ENEMY_SPEED;

        enemies[i].x +=
            enemies[i].direction_x * ENEMY_SPEED;

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
            enemies[i].y >= SCREENHEIGHT ||
            enemies[i].x <= -ENEMY_WIDTH ||
            enemies[i].x >= SCREENWIDTH)
        {
            enemies[i].active = 0;
        }
        else if (shots_hit(
                     enemies[i].x,
                     enemies[i].y,
                     ENEMY_WIDTH,
                     ENEMY_HEIGHT))
        {
            enemies[i].active = 0;
            score_add(ENEMY_SCORE_VALUE);
        }
    }
}

void enemy_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    for (i = 0; i < ENEMY_COUNT; i++)
    {
        sprite_id = ENEMY_FIRST_SPRITE_ID + i;

        if (enemies[i].active)
        {
            move_sprite(
                sprite_id,
                (uint8_t)(enemies[i].x + SPRITE_OFFSET_X),
                (uint8_t)(enemies[i].y + SPRITE_OFFSET_Y));
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }
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
