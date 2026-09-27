#include <gb/gb.h>

#include "shots.h"

#define SHOT_COUNT 8
#define SHOT_SPEED 4
#define SHOT_INTERVAL 10

#define SHOT_WIDTH 2
#define SHOT_HEIGHT 4

#define SHOT_FIRST_SPRITE_ID 1
#define SHOT_TILE_ID 1

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t active;
} Shot;

static Shot shots[SHOT_COUNT];
static uint8_t cooldown;

static const uint8_t shot_tile[] = {
    0xC0, 0xC0,
    0xC0, 0xC0,
    0xC0, 0xC0,
    0xC0, 0xC0,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00};

void shots_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    cooldown = 0;

    set_sprite_data(SHOT_TILE_ID, 1, shot_tile);

    for (i = 0; i < SHOT_COUNT; i++)
    {
        shots[i].x = 0;
        shots[i].y = 0;
        shots[i].active = 0;

        sprite_id = SHOT_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, SHOT_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void shots_update(void)
{
    uint8_t i;

    if (cooldown > 0)
    {
        cooldown--;
    }

    for (i = 0; i < SHOT_COUNT; i++)
    {
        if (shots[i].active)
        {
            if (shots[i].y < SHOT_SPEED)
            {
                shots[i].active = 0;
            }
            else
            {
                shots[i].y -= SHOT_SPEED;
            }
        }
    }
}

void shots_spawn(uint8_t x, uint8_t y)
{
    uint8_t i;

    if (cooldown > 0)
    {
        return;
    }

    for (i = 0; i < SHOT_COUNT; i++)
    {
        if (!shots[i].active)
        {
            shots[i].x = x;
            shots[i].y = y;
            shots[i].active = 1;

            cooldown = SHOT_INTERVAL;

            return;
        }
    }
}

void shots_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    for (i = 0; i < SHOT_COUNT; i++)
    {
        sprite_id = SHOT_FIRST_SPRITE_ID + i;

        if (shots[i].active)
        {
            move_sprite(
                sprite_id,
                shots[i].x + SPRITE_OFFSET_X,
                shots[i].y + SPRITE_OFFSET_Y);
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }
}

uint8_t shots_hit(
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height)
{
    uint8_t i;

    for (i = 0; i < SHOT_COUNT; i++)
    {
        if (shots[i].active)
        {
            if (
                shots[i].x < x + width &&
                shots[i].x + SHOT_WIDTH > x &&
                shots[i].y < y + height &&
                shots[i].y + SHOT_HEIGHT > y)
            {
                shots[i].active = 0;

                return 1;
            }
        }
    }

    return 0;
}
