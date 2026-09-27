#include <gb/gb.h>
#include "shots.h"

#include "player.h"

#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 8

#define PLAYER_MAX_X (SCREENWIDTH - PLAYER_WIDTH)
#define PLAYER_MAX_Y (SCREENHEIGHT - PLAYER_HEIGHT)

#define PLAYER_SPRITE_ID 0
#define PLAYER_TILE_ID 0

#define SPRITE_OFFSET_X 8
#define SPRITE_OFFSET_Y 16

static const uint8_t player_tile[] = {
    0x18, 0x18,
    0x18, 0x18,
    0x3C, 0x3C,
    0x3C, 0x3C,
    0x7E, 0x7E,
    0x7E, 0x7E,
    0xFF, 0xFF,
    0xFF, 0xFF};

static uint8_t player_x;
static uint8_t player_y;

void player_init(void)
{
    player_x = (SCREENWIDTH - PLAYER_WIDTH) / 2;
    player_y = SCREENHEIGHT - PLAYER_HEIGHT - 16;

    set_sprite_data(PLAYER_TILE_ID, 1, player_tile);
    set_sprite_tile(PLAYER_SPRITE_ID, PLAYER_TILE_ID);
    set_sprite_prop(PLAYER_SPRITE_ID, 0);
}

void player_update(uint8_t buttons)
{
    if ((buttons & J_LEFT) && player_x > 0)
    {
        player_x--;
    }

    if ((buttons & J_RIGHT) && player_x < PLAYER_MAX_X)
    {
        player_x++;
    }

    if ((buttons & J_UP) && player_y > 0)
    {
        player_y--;
    }

    if ((buttons & J_DOWN) && player_y < PLAYER_MAX_Y)
    {
        player_y++;
    }

    if (buttons & J_A)
    {
        shots_spawn(player_x + 3, player_y);
    }
}

void player_render(void)
{
    move_sprite(
        PLAYER_SPRITE_ID,
        player_x + SPRITE_OFFSET_X,
        player_y + SPRITE_OFFSET_Y);
}
