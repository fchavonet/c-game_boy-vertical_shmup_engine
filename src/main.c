#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "shots.h"

void main(void)
{
    uint8_t buttons;

    gotoxy(3, 8);
    printf("VERTICAL SHMUP");

    gotoxy(7, 9);
    printf("ENGINE");

    waitpad(J_START);
    waitpadup();

    DISPLAY_OFF;

    HIDE_BKG;
    HIDE_WIN;

    SPRITES_8x8;

    OBP0_REG = DMG_PALETTE(
        DMG_WHITE,
        DMG_LITE_GRAY,
        DMG_DARK_GRAY,
        DMG_BLACK);

    player_init();
    shots_init();

    player_render();
    shots_render();

    SHOW_SPRITES;
    DISPLAY_ON;

    while (1)
    {
        buttons = joypad();

        shots_update();
        player_update(buttons);

        player_render();
        shots_render();

        vsync();
    }
}
