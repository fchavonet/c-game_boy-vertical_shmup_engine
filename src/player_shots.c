#include "player_shot_sprite.h"
#include <gb/gb.h>

#include "graphics_layout.h"
#include "player_shots.h"

#define SHOT_COUNT GFX_PLAYER_SHOT_SPRITE_COUNT

#define SHOT_SPEED 4
#define SHOT_INTERVAL 10

#define SHOT_WIDTH 2
#define SHOT_HEIGHT 4

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t active;
} Shot;

static Shot shots[SHOT_COUNT];
static uint8_t cooldown;

static const uint8_t shot_offsets[3][3] = {
    {3, 0, 0},
    {0, 6, 0},
    {0, 3, 6}};



void shots_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    cooldown = 0;

    set_sprite_data(
        GFX_PLAYER_SHOT_TILE_ID,
        1,
        player_shot_sprite_tiles);

    for (i = 0; i < SHOT_COUNT; i++)
    {
        shots[i].x = 0;
        shots[i].y = 0;
        shots[i].active = 0;

        sprite_id = GFX_PLAYER_SHOT_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, GFX_PLAYER_SHOT_TILE_ID);
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

void shots_spawn(
    uint8_t x,
    uint8_t y,
    uint8_t weapon_level)
{
    uint8_t i;
    uint8_t available = 0;
    uint8_t created = 0;

    if (cooldown > 0)
    {
        return;
    }

    if (weapon_level < 1 || weapon_level > 3)
    {
        return;
    }

    for (i = 0; i < SHOT_COUNT; i++)
    {
        if (!shots[i].active)
        {
            available++;
        }
    }

    if (available < weapon_level)
    {
        return;
    }

    for (i = 0; i < SHOT_COUNT; i++)
    {
        if (!shots[i].active)
        {
            shots[i].x =
                x + shot_offsets[weapon_level - 1][created];

            shots[i].y = y;
            shots[i].active = 1;

            created++;

            if (created == weapon_level)
            {
                break;
            }
        }
    }

    cooldown = SHOT_INTERVAL;
}

void shots_render(void)
{
    uint8_t i;
    uint8_t sprite_id;

    for (i = 0; i < SHOT_COUNT; i++)
    {
        sprite_id = GFX_PLAYER_SHOT_FIRST_SPRITE_ID + i;

        if (shots[i].active)
        {
            move_sprite(
                sprite_id,
                shots[i].x + GFX_SPRITE_OFFSET_X,
                shots[i].y + GFX_SPRITE_OFFSET_Y);
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }
}

uint8_t shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    static uint8_t i;
    static Shot *shot;
    static uint8_t origin_x;
    static uint8_t origin_y;
    static uint8_t extent_x;
    static uint8_t extent_y;

    if (width == 0 || height == 0)
    {
        return 0;
    }

    /*
     * Coordinates stay within our 160 x 132 playfield and its
     * small exit margins. Unsigned differences let each axis
     * use one 8-bit comparison, including near negative edges.
     */
    origin_x = (uint8_t)(x - SHOT_WIDTH + 1);
    origin_y = (uint8_t)(y - SHOT_HEIGHT + 1);
    extent_x = (uint8_t)(width + SHOT_WIDTH - 1u);
    extent_y = (uint8_t)(height + SHOT_HEIGHT - 1u);

    for (i = 0, shot = shots; i < SHOT_COUNT; i++, shot++)
    {
        if (!shot->active)
        {
            continue;
        }

        if (
            (uint8_t)(shot->x - origin_x) < extent_x &&
            (uint8_t)(shot->y - origin_y) < extent_y)
        {
            shot->active = 0;
            return 1;
        }
    }

    return 0;
}
