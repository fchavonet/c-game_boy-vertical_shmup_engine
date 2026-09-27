#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "shots.h"
#include "enemy.h"

void main(void)
{
    uint8_t buttons;

    uint8_t spawn_timer = 0;
    uint8_t spawn_x = 24;

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
    enemy_init();

    player_render();
    shots_render();
    enemy_render();

    SHOW_SPRITES;
    DISPLAY_ON;

    while (1)
    {
        buttons = joypad();

        shots_update();
        player_update(buttons);
        enemy_update();

        if (spawn_timer > 0)
        {
            spawn_timer--;
        }

        if (spawn_timer == 0)
        {
            if (enemy_spawn(spawn_x, 0))
            {
                spawn_x += 24;

                if (spawn_x > 120)
                {
                    spawn_x = 24;
                }
            }

            spawn_timer = 45;
        }

        player_render();
        shots_render();
        enemy_render();

        vsync();
    }
}
