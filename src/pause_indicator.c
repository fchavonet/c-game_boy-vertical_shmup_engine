#include <gb/gb.h>

#include "graphics_layout.h"
#include "pause_indicator.h"

#define PAUSE_LETTER_COUNT 6
#define PAUSE_GLYPH_HEIGHT 7
#define PAUSE_CONTENT_Y 3

#define PAUSE_COLUMN 7

static uint8_t displayed_paused;

static const uint8_t pause_glyphs
    [PAUSE_LETTER_COUNT][PAUSE_GLYPH_HEIGHT] = {
        /* P */
        {0x78, 0x44, 0x44, 0x78, 0x40, 0x40, 0x40},

        /* A */
        {0x38, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44},

        /* U */
        {0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38},

        /* S */
        {0x3C, 0x40, 0x40, 0x38, 0x04, 0x04, 0x78},

        /* E */
        {0x7C, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7C},

        /* D */
        {0x78, 0x44, 0x44, 0x44, 0x44, 0x44, 0x78}};

void pause_indicator_init(void)
{
    uint8_t letter;
    uint8_t row;
    uint8_t pixels;
    uint8_t first_tile;

    uint8_t tile_data[32];

    for (letter = 0; letter < PAUSE_LETTER_COUNT; letter++)
    {
        for (row = 0; row < 16; row++)
        {
            pixels = 0;

            if (row == 0)
            {
                pixels = 0xFF;
            }
            else if (
                row >= PAUSE_CONTENT_Y &&
                row < PAUSE_CONTENT_Y + PAUSE_GLYPH_HEIGHT)
            {
                pixels =
                    pause_glyphs[letter][row - PAUSE_CONTENT_Y];
            }

            tile_data[row * 2] = pixels;
            tile_data[row * 2 + 1] = pixels;
        }

        first_tile = (uint8_t)(GFX_PAUSE_FIRST_TILE_ID + letter * 2u);

        set_bkg_data(
            first_tile,
            2,
            tile_data);
    }

    displayed_paused = 255;

    pause_indicator_render(0);
}

void pause_indicator_render(uint8_t paused)
{
    uint8_t letter;
    uint8_t first_tile;
    uint8_t tiles[2];

    if (paused == displayed_paused)
    {
        return;
    }

    for (letter = 0; letter < PAUSE_LETTER_COUNT; letter++)
    {
        if (paused)
        {
            first_tile = (uint8_t)(GFX_PAUSE_FIRST_TILE_ID + letter * 2u);

            tiles[0] = first_tile;
            tiles[1] = (uint8_t)(first_tile + 1u);
        }
        else
        {
            tiles[0] = GFX_HUD_SEPARATOR_TILE_ID;
            tiles[1] = GFX_HUD_BLANK_TILE_ID;
        }

        set_win_tiles(
            PAUSE_COLUMN + letter,
            0,
            1,
            2,
            tiles);
    }

    displayed_paused = paused;
}
