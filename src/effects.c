#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "effects.h"

#define EFFECT_COUNT GFX_EFFECT_SPRITE_COUNT

#define EFFECT_WIDTH 8
#define EFFECT_HEIGHT 8

#define EXPLOSION_FRAME_DURATION 4u
#define EXPLOSION_DURATION \
    (GFX_EXPLOSION_TILE_COUNT * EXPLOSION_FRAME_DURATION)

typedef struct
{
    int16_t x;
    int16_t y;
    uint8_t age;
    uint8_t active;
} Effect;

static Effect effects[EFFECT_COUNT];

static const uint8_t explosion_tiles[] = {
    /*
     * Frame 0: small central flash.
     */
    0x00, 0x00,
    0x00, 0x00,
    0x18, 0x18,
    0x3C, 0x3C,
    0x3C, 0x3C,
    0x18, 0x18,
    0x00, 0x00,
    0x00, 0x00,

    /*
     * Frame 1: expanding burst.
     */
    0x24, 0x24,
    0x18, 0x18,
    0x5A, 0x5A,
    0x3C, 0x3C,
    0x3C, 0x3C,
    0x5A, 0x5A,
    0x18, 0x18,
    0x24, 0x24,

    /*
     * Frame 2: scattered fragments.
     */
    0x81, 0x81,
    0x24, 0x24,
    0x00, 0x00,
    0x42, 0x42,
    0x42, 0x42,
    0x00, 0x00,
    0x24, 0x24,
    0x81, 0x81};

void effects_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    set_sprite_data(
        GFX_EXPLOSION_FIRST_TILE_ID,
        GFX_EXPLOSION_TILE_COUNT,
        explosion_tiles);

    for (i = 0; i < EFFECT_COUNT; i++)
    {
        effects[i].x = 0;
        effects[i].y = 0;
        effects[i].age = 0;
        effects[i].active = 0;

        sprite_id = GFX_EFFECT_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, GFX_EXPLOSION_FIRST_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void effects_spawn_explosion(int16_t x, int16_t y)
{
    uint8_t i;

    if (
        x <= -EFFECT_WIDTH ||
        x >= GAME_PLAYFIELD_WIDTH ||
        y <= -EFFECT_HEIGHT ||
        y >= GAME_PLAYFIELD_HEIGHT)
    {
        return;
    }

    for (i = 0; i < EFFECT_COUNT; i++)
    {
        if (!effects[i].active)
        {
            effects[i].x = x;
            effects[i].y = y;
            effects[i].age = 0;
            effects[i].active = 1;
            return;
        }
    }
}

void effects_update(void)
{
    uint8_t i;

    for (i = 0; i < EFFECT_COUNT; i++)
    {
        if (effects[i].active)
        {
            effects[i].age++;

            if (effects[i].age >= EXPLOSION_DURATION)
            {
                effects[i].active = 0;
            }
        }
    }
}

void effects_render(void)
{
    uint8_t i;
    uint8_t sprite_id;
    uint8_t frame;
    uint8_t tile_id;

    for (i = 0; i < EFFECT_COUNT; i++)
    {
        sprite_id = GFX_EFFECT_FIRST_SPRITE_ID + i;

        if (effects[i].active)
        {
            frame = effects[i].age / EXPLOSION_FRAME_DURATION;

            tile_id = (uint8_t)(GFX_EXPLOSION_FIRST_TILE_ID + frame);

            set_sprite_tile(sprite_id, tile_id);

            move_sprite(
                sprite_id,
                (uint8_t)(effects[i].x + GFX_SPRITE_OFFSET_X),
                (uint8_t)(effects[i].y + GFX_SPRITE_OFFSET_Y));
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }
}
