#include <gb/gb.h>

#include "graphics_layout.h"
#include "hud.h"
#include "player_sprite.h"

#define HUD_MAX_LIVES 3
#define HUD_SCORE_DIGITS 5

#define HUD_GLYPH_COUNT 10
#define HUD_GLYPH_HEIGHT 7
#define HUD_CONTENT_Y 3

#define HUD_LIFE_COLUMN 1
/* Empty pixels between the 8-pixel-wide player icons, from 0 to 8. */
#define HUD_LIFE_GAP_PX 2
#define HUD_LIFE_WIDTH_PX (HUD_MAX_LIVES * 8 + (HUD_MAX_LIVES - 1) * HUD_LIFE_GAP_PX)
#define HUD_LIFE_COLUMNS ((HUD_LIFE_WIDTH_PX + 7) / 8)
#define HUD_LIFE_STATE_TILES (HUD_LIFE_COLUMNS * 2)

#if HUD_LIFE_GAP_PX < 0 || HUD_LIFE_GAP_PX > 8
#error HUD_LIFE_GAP_PX must be between 0 and 8
#endif

#if HUD_LIFE_STATE_TILES * HUD_MAX_LIVES > GFX_HUD_LIVES_TILE_CAPACITY
#error Not enough reserved tiles for HUD lives
#endif
#define HUD_SCORE_COLUMN 14

static uint8_t displayed_lives;
static uint32_t displayed_score;

static uint8_t handlers_installed;
static volatile uint8_t hud_visible;

/*
 * An empty tile followed by a separator tile.
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
 * Digits are shifted right during tile generation.
 */
static const uint8_t hud_glyphs[HUD_GLYPH_COUNT][HUD_GLYPH_HEIGHT] = {
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

/* Prepare 1-, 2- and 3-life strips while the new screen is hidden.
 * During gameplay, changing lives only selects another tile map.
 */
static void hud_prepare_lives(void)
{
    uint8_t lives;
    uint8_t column;
    uint8_t icon;
    uint8_t row;
    uint8_t plane;
    uint8_t i;
    uint8_t x;
    uint8_t icon_column;
#if HUD_LIFE_GAP_PX % 8 != 0
    uint8_t shift;
#endif
    uint8_t bits;
    uint8_t first_tile;
    uint8_t tile_data[32];

    for (lives = 1; lives <= HUD_MAX_LIVES; lives++)
    {
        for (column = 0; column < HUD_LIFE_COLUMNS; column++)
        {
            for (i = 0; i < sizeof(tile_data); i++)
            {
                tile_data[i] = 0;
            }
            tile_data[0] = 0xFF;
            tile_data[1] = 0xFF;

            for (icon = 0; icon < lives; icon++)
            {
                x = icon * (8u + HUD_LIFE_GAP_PX);
                icon_column = x >> 3;
#if HUD_LIFE_GAP_PX % 8 == 0
                if (column != icon_column)
#else
                shift = x & 7u;
                if (column != icon_column &&
                    (shift == 0 || column != icon_column + 1u))
#endif
                {
                    continue;
                }

                for (row = 0; row < 8; row++)
                {
                    for (plane = 0; plane < 2; plane++)
                    {
                        bits = player_sprite_tiles[row * 2u + plane];
#if HUD_LIFE_GAP_PX % 8 != 0
                        if (column == icon_column)
                        {
                            bits >>= shift;
                        }
                        else
                        {
                            bits <<= 8u - shift;
                        }
#endif
                        tile_data[(HUD_CONTENT_Y + row) * 2u + plane] |= bits;
                    }
                }
            }

            first_tile = (uint8_t)(GFX_HUD_LIVES_FIRST_TILE_ID +
                (lives - 1u) * HUD_LIFE_STATE_TILES + column * 2u);
            set_bkg_data(first_tile, 2, tile_data);
        }
    }
}

void hud_init(void)
{
    uint8_t glyph;
    uint8_t row;
    uint8_t i;
    uint8_t pixels;
    uint8_t glyph_tile;

    uint8_t tile_data[32];
    uint8_t map_row[20];

    hud_visible = 0;

    displayed_lives = 255;
    displayed_score = 100000UL;

    LCDC_REG &= (uint8_t)~(LCDCF_BG8000 | LCDCF_BG9C00);
    LCDC_REG |= LCDCF_WIN9C00;

    /* Palette ownership belongs to transition.c: keep the image black
     * while loading the HUD and background for a new level.
     */

    set_bkg_data(
        GFX_HUD_BLANK_TILE_ID,
        1,
        hud_base_tiles);

    set_bkg_data(
        GFX_HUD_SEPARATOR_TILE_ID,
        1,
        hud_base_tiles + 16);

    /*
     * Each glyph occupies two vertically stacked tiles.
     * Row 0 contains the separator.
     * Rows 3 through 9 contain the glyph.
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

                pixels >>= 2;
            }

            tile_data[row * 2] = pixels;
            tile_data[row * 2 + 1] = pixels;
        }

        glyph_tile = (uint8_t)(GFX_HUD_FIRST_DIGIT_TILE_ID + glyph * 2u);
        set_bkg_data(glyph_tile, 2, tile_data);
    }

    hud_prepare_lives();

    fill_bkg_rect(0, 0, 32, 32, GFX_HUD_BLANK_TILE_ID);

    for (i = 0; i < 20; i++)
    {
        map_row[i] = GFX_HUD_SEPARATOR_TILE_ID;
    }

    set_win_tiles(0, 0, 20, 1, map_row);

    for (i = 0; i < 20; i++)
    {
        map_row[i] = GFX_HUD_BLANK_TILE_ID;
    }

    set_win_tiles(0, 1, 20, 1, map_row);

    /*
     * Hide the reserved slots from the old sprite HUD.
     */
    for (i = 0; i < GFX_LEGACY_HUD_SPRITE_COUNT; i++)
    {
        move_sprite(
            GFX_LEGACY_HUD_FIRST_SPRITE_ID + i,
            0,
            0);
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

    uint8_t life_tiles[HUD_LIFE_STATE_TILES];
    uint8_t score_tiles[HUD_SCORE_DIGITS * 2];

    uint32_t remaining;
    uint16_t remainder;
    static const uint16_t divisors[4] = {1000u, 100u, 10u, 1u};

    if (lives > HUD_MAX_LIVES)
    {
        lives = HUD_MAX_LIVES;
    }

    if (lives != displayed_lives)
    {
        for (i = 0; i < HUD_LIFE_COLUMNS; i++)
        {
            life_tiles[i] = GFX_HUD_SEPARATOR_TILE_ID;
            life_tiles[i + HUD_LIFE_COLUMNS] = GFX_HUD_BLANK_TILE_ID;

            if (lives != 0)
            {
                top_tile = (uint8_t)(GFX_HUD_LIVES_FIRST_TILE_ID +
                    (lives - 1u) * HUD_LIFE_STATE_TILES + i * 2u);
                life_tiles[i] = top_tile;
                life_tiles[i + HUD_LIFE_COLUMNS] = top_tile + 1u;
            }
        }

        set_win_tiles(HUD_LIFE_COLUMN, 0, HUD_LIFE_COLUMNS, 2, life_tiles);
        displayed_lives = lives;
    }

    if (score != displayed_score)
    {
        remaining = score;

        digit = 0;

        while (remaining >= 10000UL)
        {
            remaining -= 10000UL;
            digit++;
        }

        top_tile = (uint8_t)(GFX_HUD_FIRST_DIGIT_TILE_ID + digit * 2u);
        score_tiles[0] = top_tile;
        score_tiles[HUD_SCORE_DIGITS] = (uint8_t)(top_tile + 1u);

        remainder = (uint16_t)remaining;

        for (i = 0; i < 4u; i++)
        {
            digit = 0;

            while (remainder >= divisors[i])
            {
                remainder -= divisors[i];
                digit++;
            }

            top_tile = (uint8_t)(GFX_HUD_FIRST_DIGIT_TILE_ID + digit * 2u);
            score_tiles[i + 1u] = top_tile;
            score_tiles[i + 1u + HUD_SCORE_DIGITS] = (uint8_t)(top_tile + 1u);
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
