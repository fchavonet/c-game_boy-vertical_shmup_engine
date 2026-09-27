#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "background.h"

#define BACKGROUND_MAP_WIDTH 32
#define BACKGROUND_MAP_HEIGHT 32

#define BACKGROUND_VISIBLE_COLUMNS \
    (GAME_PLAYFIELD_WIDTH / 8)

#define BACKGROUND_SCROLL_INTERVAL 2

static uint8_t scroll_y;
static uint8_t scroll_timer;

static const uint8_t background_tiles[] = {
    /*
     * Empty tile.
     */
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,

    /*
     * Black square: 2 x 2 pixels.
     */
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x18, 0x18,
    0x18, 0x18,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,

    /*
     * Black cross: 5 x 5 pixels.
     */
    0x00, 0x00,
    0x10, 0x10,
    0x10, 0x10,
    0x7C, 0x7C,
    0x10, 0x10,
    0x10, 0x10,
    0x00, 0x00,
    0x00, 0x00};

void background_init(void)
{
    uint8_t x;
    uint8_t y;
    uint8_t tile_id;

    scroll_y = 0;
    scroll_timer = 0;

    set_bkg_data(
        GFX_BACKGROUND_FIRST_TILE_ID,
        GFX_BACKGROUND_TILE_COUNT,
        background_tiles);

    /*
     * Clear the entire background map first.
     */
    fill_bkg_rect(
        0,
        0,
        BACKGROUND_MAP_WIDTH,
        BACKGROUND_MAP_HEIGHT,
        GFX_BACKGROUND_BLANK_TILE_ID);

    /*
     * Place stars directly in the background map.
     * One star every two tile rows.
     */
    x = 3;

    for (y = 0; y < BACKGROUND_MAP_HEIGHT; y += 2)
    {
        tile_id = GFX_BACKGROUND_SMALL_STAR_TILE_ID;

        if ((y & 6) == 0)
        {
            tile_id = GFX_BACKGROUND_LARGE_STAR_TILE_ID;
        }

        fill_bkg_rect(
            x,
            y,
            1,
            1,
            tile_id);

        x += 13;

        if (x >= BACKGROUND_VISIBLE_COLUMNS)
        {
            x -= BACKGROUND_VISIBLE_COLUMNS;
        }
    }
}

void background_update(void)
{
    scroll_timer++;

    if (scroll_timer < BACKGROUND_SCROLL_INTERVAL)
    {
        return;
    }

    scroll_timer = 0;

    if (scroll_y == 0)
    {
        scroll_y = 255;
    }
    else
    {
        scroll_y--;
    }
}

void background_render(void)
{
    move_bkg(0, scroll_y);
}
