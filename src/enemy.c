#include <gb/gb.h>
#include <stdint.h>

#include "enemy.h"
#include "shots.h"

#define ENEMY_WIDTH 8
#define ENEMY_HEIGHT 8

#define ENEMY_SPRITE_ID 9
#define ENEMY_TILE_ID 2

#define ENEMY_RESPAWN_DELAY 60

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

static const uint8_t enemy_tile[] = {
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF,
    0xFF, 0xFF};

static uint8_t enemy_x;
static uint8_t enemy_y;
static uint8_t enemy_active;
static uint8_t respawn_timer;

void enemy_init(void)
{
    enemy_x = (SCREENWIDTH - ENEMY_WIDTH) / 2;
    enemy_y = 24;
    enemy_active = 1;
    respawn_timer = 0;

    set_sprite_data(ENEMY_TILE_ID, 1, enemy_tile);
    set_sprite_tile(ENEMY_SPRITE_ID, ENEMY_TILE_ID);
    set_sprite_prop(ENEMY_SPRITE_ID, 0);
}

void enemy_update(void)
{
    if (!enemy_active)
    {
        if (respawn_timer > 0)
        {
            respawn_timer--;
        }

        if (respawn_timer == 0)
        {
            enemy_active = 1;
        }

        return;
    }

    if (shots_hit(
            enemy_x,
            enemy_y,
            ENEMY_WIDTH,
            ENEMY_HEIGHT))
    {
        enemy_active = 0;
        respawn_timer = ENEMY_RESPAWN_DELAY;
    }
}

void enemy_render(void)
{
    if (enemy_active)
    {
        move_sprite(
            ENEMY_SPRITE_ID,
            enemy_x + SPRITE_OFFSET_X,
            enemy_y + SPRITE_OFFSET_Y);
    }
    else
    {
        move_sprite(ENEMY_SPRITE_ID, 0, 0);
    }
}
