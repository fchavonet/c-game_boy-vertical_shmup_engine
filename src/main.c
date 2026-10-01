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
#include "pause_indicator.h"
#include "effects.h"
#include "transition.h"

void main(void)
{
    uint8_t buttons;
    uint8_t previous_buttons;
    uint8_t pressed_buttons;
    uint8_t paused;
    uint8_t level_index;

    transition_init();

    gotoxy(3, 8);
    printf("VERTICAL SHMUP");

    gotoxy(7, 9);
    printf("ENGINE");

    transition_fade_in();

    waitpad(J_START);
    waitpadup();
    transition_fade_out();

    while (1)
    {
        level_index = 0;

        while (level_index < LEVEL_COUNT)
        {
            hud_hide();

            HIDE_SPRITES;

            move_bkg(0, 0);

            SPRITES_8x8;

            previous_buttons = 0;
            paused = 0;

            if (level_index == 0)
            {
                player_init();
                score_init();
            }

            shots_init();
            enemy_init();
            enemy_shots_init();
            powerup_init();
            boss_init();
            effects_init();

            level_init(levels[level_index]);

            hud_init();
            background_init();
            pause_indicator_init();

            player_render();
            shots_render();
            enemy_render();
            enemy_shots_render();
            powerup_render();
            boss_render();
            effects_render();
            hud_render(player_get_lives(), score_get());
            background_render();

            SHOW_SPRITES;
            transition_fade_in();
            /* A Start press during a fade must not pause the new game. */
            previous_buttons = joypad();

            while (
                (player_is_alive() || player_is_destroying()) &&
                (!level_is_complete() || player_is_destroying()))
            {
                buttons = joypad();

                pressed_buttons =
                    buttons & (uint8_t)~previous_buttons;

                previous_buttons = buttons;

                if (pressed_buttons & J_START)
                {
                    if (paused)
                    {
                        paused = 0;
                    }
                    else
                    {
                        paused = 1;
                    }
                }

                pause_indicator_render(paused);

                if (paused)
                {
                    vsync();
                    continue;
                }

                background_update();
                effects_update();

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
                effects_render();
                hud_render(player_get_lives(), score_get());

                vsync();

                background_render();
            }

            transition_fade_out();

            hud_hide();
            HIDE_SPRITES;

            move_bkg(0, 0);

            cls();

            if (!player_is_alive())
            {
                gotoxy(5, 7);
                printf("GAME OVER!");
            }
            else if (level_index == LEVEL_COUNT - 1)
            {
                gotoxy(5, 7);
                printf("ALL CLEAR!");
            }
            else
            {
                gotoxy(4, 7);
                printf("STAGE CLEAR!");
            }

            gotoxy(4, 9);
            printf("PRESS START...");

            SHOW_BKG;
            transition_fade_in();

            waitpadup();
            waitpad(J_START);
            waitpadup();
            transition_fade_out();

            if (!player_is_alive())
            {
                break;
            }

            level_index++;
        }
    }
}
