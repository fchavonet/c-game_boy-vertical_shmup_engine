#include <gb/gb.h>
#include "transition.h"

#if TRANSITION_FRAMES_PER_STEP < 1 || TRANSITION_FRAMES_PER_STEP > 255
#error TRANSITION_FRAMES_PER_STEP must be between 1 and 255
#endif

static uint8_t normal_background;
static uint8_t normal_objects0;
static uint8_t normal_objects1;

void transition_set_palettes(uint8_t background, uint8_t objects0, uint8_t objects1)
{
    normal_background = background;
    normal_objects0 = objects0;
    normal_objects1 = objects1;
}

void transition_init(void)
{
    uint8_t normal = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK);

    transition_set_palettes(normal, normal, normal);
    BGP_REG = 0xFFu;
    OBP0_REG = 0xFFu;
    OBP1_REG = 0xFFu;
}

/* Darken the shades themselves; preserve any custom palette ordering. */
static uint8_t darken_palette(uint8_t palette, uint8_t amount)
{
    uint8_t i;
    uint8_t shade;
    uint8_t result = 0;

    for (i = 0; i < 4u; i++)
    {
        shade = (palette & 3u) + amount;
        if (shade > 3u)
        {
            shade = 3u;
        }
        result |= (uint8_t)(shade << (i * 2u));
        palette >>= 2;
    }
    return result;
}

static void transition_step(uint8_t amount)
{
    uint8_t background;
    uint8_t objects0;
    uint8_t objects1;
    uint8_t frame;

    /* Compute before VBlank, then only write the three palette registers. */
    background = darken_palette(normal_background, amount);
    objects0 = darken_palette(normal_objects0, amount);
    objects1 = darken_palette(normal_objects1, amount);

    vsync();
    BGP_REG = background;
    OBP0_REG = objects0;
    OBP1_REG = objects1;

    for (frame = 1; frame < TRANSITION_FRAMES_PER_STEP; frame++)
    {
        vsync();
    }
}

void transition_fade_out(void)
{
    uint8_t amount;

    for (amount = 1; amount <= 3u; amount++)
    {
        transition_step(amount);
    }
}

void transition_fade_in(void)
{
    uint8_t amount = 3;

    while (amount > 0)
    {
        amount--;
        transition_step(amount);
    }
}
