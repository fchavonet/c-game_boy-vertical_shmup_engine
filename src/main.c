#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "shots.h"
#include "enemy.h"
#include "level.h"
#include "hud.h"
#include "score.h"

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

        /* Initialize once per game. */
        player_init();
        shots_init();
        enemy_init();
        level_init();
        score_init();
        hud_init();

        player_render();
        shots_render();
        enemy_render();
        hud_render(player_get_lives(), score_get());

        SHOW_SPRITES;
        DISPLAY_ON;

        while (player_is_alive())
        {
            buttons = joypad();

            /* Update the current game. */
            shots_update();
            player_update(buttons);
            enemy_update();
            player_check_collision();
            level_update();

            /* Prepare the display. */
            player_render();
            shots_render();
            enemy_render();
            hud_render(player_get_lives(), score_get());

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
