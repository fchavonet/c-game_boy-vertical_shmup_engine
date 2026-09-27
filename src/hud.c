#include <gb/gb.h>

#include "hud.h"

#define HUD_MAX_LIVES 3

#define HUD_FIRST_HEART_SPRITE_ID 15
#define HUD_HEART_TILE_ID 3

#define HUD_HEART_X 4
#define HUD_HEART_Y 4
#define HUD_HEART_SPACING 10

#define HUD_SCORE_DIGITS 5
#define HUD_FIRST_SCORE_SPRITE_ID 18
#define HUD_FIRST_DIGIT_TILE_ID 4

#define HUD_SCORE_X 116
#define HUD_SCORE_Y 4
#define HUD_DIGIT_SPACING 8

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

static uint32_t displayed_score;

static const uint8_t heart_tile[] = {
    0x66, 0x66,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0x7E, 0x7E,
    0x3C, 0x3C,
    0x18, 0x18,
    0x00, 0x00};

static const uint8_t digit_tiles[] = {
    /* 0 */
    0x38, 0x38,
    0x44, 0x44,
    0x4C, 0x4C,
    0x54, 0x54,
    0x64, 0x64,
    0x44, 0x44,
    0x38, 0x38,
    0x00, 0x00,

    /* 1 */
    0x10, 0x10,
    0x30, 0x30,
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x10,
    0x10, 0x10,
    0x38, 0x38,
    0x00, 0x00,

    /* 2 */
    0x38, 0x38,
    0x44, 0x44,
    0x04, 0x04,
    0x08, 0x08,
    0x10, 0x10,
    0x20, 0x20,
    0x7C, 0x7C,
    0x00, 0x00,

    /* 3 */
    0x78, 0x78,
    0x04, 0x04,
    0x04, 0x04,
    0x38, 0x38,
    0x04, 0x04,
    0x04, 0x04,
    0x78, 0x78,
    0x00, 0x00,

    /* 4 */
    0x08, 0x08,
    0x18, 0x18,
    0x28, 0x28,
    0x48, 0x48,
    0x7C, 0x7C,
    0x08, 0x08,
    0x08, 0x08,
    0x00, 0x00,

    /* 5 */
    0x7C, 0x7C,
    0x40, 0x40,
    0x40, 0x40,
    0x78, 0x78,
    0x04, 0x04,
    0x04, 0x04,
    0x78, 0x78,
    0x00, 0x00,

    /* 6 */
    0x38, 0x38,
    0x40, 0x40,
    0x40, 0x40,
    0x78, 0x78,
    0x44, 0x44,
    0x44, 0x44,
    0x38, 0x38,
    0x00, 0x00,

    /* 7 */
    0x7C, 0x7C,
    0x04, 0x04,
    0x08, 0x08,
    0x10, 0x10,
    0x20, 0x20,
    0x20, 0x20,
    0x20, 0x20,
    0x00, 0x00,

    /* 8 */
    0x38, 0x38,
    0x44, 0x44,
    0x44, 0x44,
    0x38, 0x38,
    0x44, 0x44,
    0x44, 0x44,
    0x38, 0x38,
    0x00, 0x00,

    /* 9 */
    0x38, 0x38,
    0x44, 0x44,
    0x44, 0x44,
    0x3C, 0x3C,
    0x04, 0x04,
    0x04, 0x04,
    0x38, 0x38,
    0x00, 0x00};

void hud_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    displayed_score = 100000UL;

    set_sprite_data(HUD_HEART_TILE_ID, 1, heart_tile);

    set_sprite_data(
        HUD_FIRST_DIGIT_TILE_ID,
        10,
        digit_tiles);

    for (i = 0; i < HUD_MAX_LIVES; i++)
    {
        sprite_id = HUD_FIRST_HEART_SPRITE_ID + i;

        set_sprite_tile(sprite_id, HUD_HEART_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }

    for (i = 0; i < HUD_SCORE_DIGITS; i++)
    {
        sprite_id = HUD_FIRST_SCORE_SPRITE_ID + i;

        set_sprite_tile(sprite_id, HUD_FIRST_DIGIT_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void hud_render(uint8_t lives, uint32_t score)
{
    uint8_t i;
    uint8_t sprite_id;
    uint8_t digit;
    uint32_t remaining;

    for (i = 0; i < HUD_MAX_LIVES; i++)
    {
        sprite_id = HUD_FIRST_HEART_SPRITE_ID + i;

        if (i < lives)
        {
            move_sprite(
                sprite_id,
                HUD_HEART_X + i * HUD_HEART_SPACING + SPRITE_OFFSET_X,
                HUD_HEART_Y + SPRITE_OFFSET_Y);
        }
        else
        {
            move_sprite(sprite_id, 0, 0);
        }
    }

    if (score != displayed_score)
    {
        remaining = score;

        for (i = 0; i < HUD_SCORE_DIGITS; i++)
        {
            digit = (uint8_t)(remaining % 10);
            remaining /= 10;

            sprite_id =
                HUD_FIRST_SCORE_SPRITE_ID + HUD_SCORE_DIGITS - 1 - i;

            set_sprite_tile(
                sprite_id,
                HUD_FIRST_DIGIT_TILE_ID + digit);
        }

        displayed_score = score;
    }

    for (i = 0; i < HUD_SCORE_DIGITS; i++)
    {
        sprite_id = HUD_FIRST_SCORE_SPRITE_ID + i;

        move_sprite(
            sprite_id,
            HUD_SCORE_X + i * HUD_DIGIT_SPACING + SPRITE_OFFSET_X,
            HUD_SCORE_Y + SPRITE_OFFSET_Y);
    }
}
