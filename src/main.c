#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "player_shots.h"
#include "enemy.h"
#include "enemy_shots.h"
#include "powerup.h"
#include "boss.h"
#include "level.h"
#include "level_data.h"
#include "hud.h"
#include "score.h"
#include "background.h"

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

        hud_hide();

        HIDE_BKG;
        HIDE_SPRITES;

        move_bkg(0, 0);

        SPRITES_8x8;

        OBP0_REG = DMG_PALETTE(
            DMG_WHITE,
            DMG_LITE_GRAY,
            DMG_DARK_GRAY,
            DMG_BLACK);

        player_init();
        shots_init();
        enemy_init();
        enemy_shots_init();
        powerup_init();
        boss_init();

        level_init(&level_one);

        score_init();

        hud_init();
        background_init();

        player_render();
        shots_render();
        enemy_render();
        enemy_shots_render();
        powerup_render();
        boss_render();
        hud_render(player_get_lives(), score_get());
        background_render();

        SHOW_SPRITES;
        DISPLAY_ON;

        while (player_is_alive() && !level_is_complete())
        {
            buttons = joypad();

            background_update();

            shots_update();
            enemy_shots_update();
            powerup_update();

            player_update(buttons);
            enemy_update();
            boss_update();

            if (!level_is_complete())
            {
                player_check_collision();
                level_update();
            }

            player_render();
            shots_render();
            enemy_render();
            enemy_shots_render();
            powerup_render();
            boss_render();
            hud_render(player_get_lives(), score_get());

            vsync();

            background_render();
        }

        DISPLAY_OFF;

        hud_hide();
        HIDE_SPRITES;

        move_bkg(0, 0);

        cls();

        if (level_is_complete())
        {
            gotoxy(4, 7);
            printf("STAGE CLEAR!");
        }
        else
        {
            gotoxy(5, 7);
            printf("GAME OVER!");
        }

        gotoxy(4, 9);
        printf("PRESS START...");

        SHOW_BKG;
        DISPLAY_ON;

        waitpadup();
        waitpad(J_START);
        waitpadup();
    }
}
