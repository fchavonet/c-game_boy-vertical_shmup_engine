#include <gb/gb.h>

#include "game_config.h"
#include "enemy_shots.h"

#define ENEMY_SHOT_COUNT 6

#define ENEMY_SHOT_WIDTH 4
#define ENEMY_SHOT_HEIGHT 4
#define ENEMY_SHOT_SPEED 2

#define ENEMY_SHOT_FIRST_SPRITE_ID 23
#define ENEMY_SHOT_TILE_ID 14

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

typedef struct
{
    int16_t x;
    int16_t y;
    uint8_t active;
} EnemyShot;

static EnemyShot enemy_shots[ENEMY_SHOT_COUNT];

static const uint8_t enemy_shot_tile[] = {
    0x60, 0x60,
    0xF0, 0xF0,
    0xF0, 0xF0,
    0x60, 0x60,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00};

void enemy_shots_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    set_sprite_data(
        ENEMY_SHOT_TILE_ID,
        1,
        enemy_shot_tile);

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        enemy_shots[i].x = 0;
        enemy_shots[i].y = 0;
        enemy_shots[i].active = 0;

        sprite_id = ENEMY_SHOT_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, ENEMY_SHOT_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void enemy_shots_spawn(int16_t x, int16_t y)
{
    uint8_t i;

    if (
        x < 0 ||
        x > GAME_PLAYFIELD_WIDTH - ENEMY_SHOT_WIDTH ||
        y < 0 ||
        y > GAME_PLAYFIELD_HEIGHT - ENEMY_SHOT_HEIGHT)
    {
        return;
    }

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (!enemy_shots[i].active)
        {
            enemy_shots[i].x = x;
            enemy_shots[i].y = y;
            enemy_shots[i].active = 1;
            return;
        }
    }
}

void enemy_shots_update(void)
{
    uint8_t i;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (enemy_shots[i].active)
        {
            enemy_shots[i].y += ENEMY_SHOT_SPEED;

            if (enemy_shots[i].y >= GAME_PLAYFIELD_HEIGHT)
            {
                enemy_shots[i].active = 0;
            }
        }
    }
}

uint8_t enemy_shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    uint8_t i;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (enemy_shots[i].active)
        {
            if (
                enemy_shots[i].x < x + width &&
                enemy_shots[i].x + ENEMY_SHOT_WIDTH > x &&
                enemy_shots[i].y < y + height &&
                enemy_shots[i].y + ENEMY_SHOT_HEIGHT > y)
            {
                enemy_shots[i].active = 0;
                return 1;
            }
        }
    }

    return 0;
}

void enemy_shots_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        sprite_id = ENEMY_SHOT_FIRST_SPRITE_ID + i;

        if (enemy_shots[i].active)
        {
            move_sprite(
                sprite_id,
                (uint8_t)(enemy_shots[i].x + SPRITE_OFFSET_X),
                (uint8_t)(enemy_shots[i].y + SPRITE_OFFSET_Y));
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }
}
