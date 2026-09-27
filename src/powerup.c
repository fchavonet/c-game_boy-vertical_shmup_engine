#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "powerup.h"

#define POWERUP_WIDTH 8
#define POWERUP_HEIGHT 8

#define POWERUP_KILLS_REQUIRED 3
#define POWERUP_MOVE_INTERVAL 2

static int16_t powerup_x;
static int16_t powerup_y;

static uint8_t powerup_active;
static uint8_t movement_timer;
static uint8_t kill_count;

static const uint8_t powerup_tile[] = {
    0xFF, 0xFF,
    0x81, 0x81,
    0xB9, 0xB9,
    0xA5, 0xA5,
    0xB9, 0xB9,
    0xA1, 0xA1,
    0x81, 0x81,
    0xFF, 0xFF};

void powerup_init(void)
{
    powerup_x = 0;
    powerup_y = 0;
    powerup_active = 0;
    movement_timer = 0;
    kill_count = 0;

    set_sprite_data(GFX_POWERUP_TILE_ID, 1, powerup_tile);
    set_sprite_tile(GFX_POWERUP_SPRITE_ID, GFX_POWERUP_TILE_ID);
    set_sprite_prop(GFX_POWERUP_SPRITE_ID, 0);
    move_sprite(GFX_POWERUP_SPRITE_ID, 0, 0);
}

void powerup_on_enemy_destroyed(int16_t x, int16_t y)
{
    kill_count++;

    if (kill_count < POWERUP_KILLS_REQUIRED)
    {
        return;
    }

    kill_count = 0;

    if (powerup_active)
    {
        return;
    }

    if (x < 0)
    {
        x = 0;
    }
    else if (x > GAME_PLAYFIELD_WIDTH - POWERUP_WIDTH)
    {
        x = GAME_PLAYFIELD_WIDTH - POWERUP_WIDTH;
    }

    if (y < 0)
    {
        y = 0;
    }
    else if (y > GAME_PLAYFIELD_HEIGHT - POWERUP_HEIGHT)
    {
        y = GAME_PLAYFIELD_HEIGHT - POWERUP_HEIGHT;
    }

    powerup_x = x;
    powerup_y = y;
    powerup_active = 1;
    movement_timer = 0;
}

void powerup_update(void)
{
    if (!powerup_active)
    {
        return;
    }

    movement_timer++;

    if (movement_timer >= POWERUP_MOVE_INTERVAL)
    {
        movement_timer = 0;
        powerup_y++;

        if (powerup_y >= GAME_PLAYFIELD_HEIGHT)
        {
            powerup_active = 0;
        }
    }
}

uint8_t powerup_collect(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    if (!powerup_active)
    {
        return 0;
    }

    if (
        powerup_x < x + width &&
        powerup_x + POWERUP_WIDTH > x &&
        powerup_y < y + height &&
        powerup_y + POWERUP_HEIGHT > y)
    {
        powerup_active = 0;
        return 1;
    }

    return 0;
}

void powerup_render(void)
{
    if (powerup_active)
    {
        move_sprite(
            GFX_POWERUP_SPRITE_ID,
            (uint8_t)(powerup_x + GFX_SPRITE_OFFSET_X),
            (uint8_t)(powerup_y + GFX_SPRITE_OFFSET_Y));
    }
    else
    {
        move_sprite(GFX_POWERUP_SPRITE_ID, 0, 0);
    }
}
