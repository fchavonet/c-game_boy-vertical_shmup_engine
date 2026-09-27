#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "shots.h"
#include "enemy.h"
#include "level.h"

void main(void)
{
    uint8_t buttons;

    gotoxy(3, 8);
    printf("VERTICAL SHMUP");

    gotoxy(7, 9);
    printf("ENGINE");

    waitpad(J_START);
    waitpadup();

    while (1)
    {
        DISPLAY_OFF;

        HIDE_BKG;
        move_bkg(0, 0);
        HIDE_WIN;
        HIDE_SPRITES;

        SPRITES_8x8;

        OBP0_REG = DMG_PALETTE(
            DMG_WHITE,
            DMG_LITE_GRAY,
            DMG_DARK_GRAY,
            DMG_BLACK);

        player_init();
        shots_init();
        enemy_init();
        level_init();

        player_render();
        shots_render();
        enemy_render();

        SHOW_SPRITES;
        DISPLAY_ON;

        while (player_is_alive())
        {
            buttons = joypad();

            shots_update();
            player_update(buttons);
            enemy_update();
            player_check_collision();
            level_update();

            player_render();
            shots_render();
            enemy_render();

            vsync();
        }

        HIDE_SPRITES;

        cls();

        gotoxy(5, 7);
        printf("GAME OVER!");

        gotoxy(4, 9);
        printf("PRESS START...");

        SHOW_BKG;

        waitpadup();
        waitpad(J_START);
        waitpadup();
    }
}
