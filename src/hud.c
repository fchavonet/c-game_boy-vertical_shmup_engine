#include <gb/gb.h>

#include "hud.h"

#define HUD_MAX_LIVES 3
#define HUD_SCORE_DIGITS 5

#define HUD_BLANK_TILE_ID 128u
#define HUD_SEPARATOR_TILE_ID 129u

#define HUD_HEART_TOP_TILE_ID 130u
#define HUD_FIRST_DIGIT_TOP_TILE_ID 132u

#define HUD_GLYPH_COUNT 11
#define HUD_GLYPH_HEIGHT 7
#define HUD_CONTENT_Y 3

#define HUD_HEART_COLUMN 1
#define HUD_SCORE_COLUMN 14

static uint8_t displayed_lives;
static uint32_t displayed_score;

static uint8_t handlers_installed;
static volatile uint8_t hud_visible;

/*
 * Two tiles:
 * - An empty tile for the background and empty HUD cells.
 * - A horizontal separator for the first HUD row.
 */
static const uint8_t hud_base_tiles[] = {
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,

    0xFF, 0xFF,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00};

/*
 * One byte per image row.
 * The heart is six pixels wide.
 * Digits are shifted right during tile generation.
 */
static const uint8_t hud_glyphs[HUD_GLYPH_COUNT][HUD_GLYPH_HEIGHT] = {
    /* Heart */
    {0x48, 0xFC, 0xFC, 0xFC, 0x78, 0x78, 0x30},

    /* 0 */
    {0x38, 0x44, 0x4C, 0x54, 0x64, 0x44, 0x38},

    /* 1 */
    {0x10, 0x30, 0x10, 0x10, 0x10, 0x10, 0x38},

    /* 2 */
    {0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x7C},

    /* 3 */
    {0x78, 0x04, 0x04, 0x38, 0x04, 0x04, 0x78},

    /* 4 */
    {0x08, 0x18, 0x28, 0x48, 0x7C, 0x08, 0x08},

    /* 5 */
    {0x7C, 0x40, 0x40, 0x78, 0x04, 0x04, 0x78},

    /* 6 */
    {0x38, 0x40, 0x40, 0x78, 0x44, 0x44, 0x38},

    /* 7 */
    {0x7C, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20},

    /* 8 */
    {0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38},

    /* 9 */
    {0x38, 0x44, 0x44, 0x3C, 0x04, 0x04, 0x38}};

static void hud_vblank(void)
{
    if (hud_visible)
    {
        SHOW_SPRITES;
    }
}

static void hud_lcd(void)
{
    if (hud_visible)
    {
        HIDE_SPRITES;
    }
}

void hud_init(void)
{
    uint8_t glyph;
    uint8_t row;
    uint8_t i;
    uint8_t pixels;

    uint8_t tile_data[32];
    uint8_t map_row[20];

    hud_visible = 0;

    displayed_lives = 255;
    displayed_score = 100000UL;

    LCDC_REG &= (uint8_t)~(LCDCF_BG8000 | LCDCF_BG9C00);
    LCDC_REG |= LCDCF_WIN9C00;

    BGP_REG = DMG_PALETTE(
        DMG_WHITE,
        DMG_LITE_GRAY,
        DMG_DARK_GRAY,
        DMG_BLACK);

    set_bkg_data(
        HUD_BLANK_TILE_ID,
        2,
        hud_base_tiles);

    /*
     * Each glyph occupies two vertically stacked tiles.
     * Row 0 contains the separator.
     * Rows 3 through 9 contain the seven-pixel-high glyph.
     */
    for (glyph = 0; glyph < HUD_GLYPH_COUNT; glyph++)
    {
        for (row = 0; row < 16; row++)
        {
            pixels = 0;

            if (row == 0)
            {
                pixels = 0xFF;
            }
            else if (
                row >= HUD_CONTENT_Y &&
                row < HUD_CONTENT_Y + HUD_GLYPH_HEIGHT)
            {
                pixels = hud_glyphs[glyph][row - HUD_CONTENT_Y];

                if (glyph > 0)
                {
                    pixels >>= 2;
                }
            }

            tile_data[row * 2] = pixels;
            tile_data[row * 2 + 1] = pixels;
        }

        set_bkg_data(
            HUD_HEART_TOP_TILE_ID + glyph * 2,
            2,
            tile_data);
    }

    fill_bkg_rect(0, 0, 32, 32, HUD_BLANK_TILE_ID);

    for (i = 0; i < 20; i++)
    {
        map_row[i] = HUD_SEPARATOR_TILE_ID;
    }

    set_win_tiles(0, 0, 20, 1, map_row);

    for (i = 0; i < 20; i++)
    {
        map_row[i] = HUD_BLANK_TILE_ID;
    }

    set_win_tiles(0, 1, 20, 1, map_row);

    /* These sprite slots are no longer used by the HUD. */
    for (i = 15; i <= 22; i++)
    {
        move_sprite(i, 0, 0);
    }

    move_win(7, HUD_TOP);

    if (!handlers_installed)
    {
        add_VBL(hud_vblank);
        add_LCD(hud_lcd);

        handlers_installed = 1;
    }

    LYC_REG = HUD_TOP;
    STAT_REG = STATF_LYC;

    hud_visible = 1;

    set_interrupts(VBL_IFLAG | LCD_IFLAG);

    SHOW_BKG;
    SHOW_WIN;
}

void hud_render(uint8_t lives, uint32_t score)
{
    uint8_t i;
    uint8_t digit;
    uint8_t top_tile;

    uint8_t heart_tiles[2];
    uint8_t score_tiles[HUD_SCORE_DIGITS * 2];

    uint32_t remaining;

    if (lives != displayed_lives)
    {
        for (i = 0; i < HUD_MAX_LIVES; i++)
        {
            heart_tiles[0] = HUD_SEPARATOR_TILE_ID;
            heart_tiles[1] = HUD_BLANK_TILE_ID;

            if (i < lives)
            {
                heart_tiles[0] = HUD_HEART_TOP_TILE_ID;
                heart_tiles[1] = HUD_HEART_TOP_TILE_ID + 1u;
            }

            set_win_tiles(
                HUD_HEART_COLUMN + i,
                0,
                1,
                2,
                heart_tiles);
        }

        displayed_lives = lives;
    }

    if (score != displayed_score)
    {
        remaining = score;

        for (i = 0; i < HUD_SCORE_DIGITS; i++)
        {
            digit = (uint8_t)(remaining % 10);
            remaining /= 10;

            top_tile = HUD_FIRST_DIGIT_TOP_TILE_ID + digit * 2;

            score_tiles[HUD_SCORE_DIGITS - 1 - i] = top_tile;

            score_tiles[HUD_SCORE_DIGITS * 2 - 1 - i] =
                top_tile + 1;
        }

        set_win_tiles(
            HUD_SCORE_COLUMN,
            0,
            HUD_SCORE_DIGITS,
            2,
            score_tiles);

        displayed_score = score;
    }
}

void hud_hide(void)
{
    hud_visible = 0;
    HIDE_WIN;
}
