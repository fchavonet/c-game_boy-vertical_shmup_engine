#include <gb/gb.h>

#include "player.h"
#include "shots.h"
#include "enemy.h"
#include "enemy_shots.h"
#include "powerup.h"

#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 8

#define POSITION_SCALE 16

#define PLAYER_SPEED 24
#define PLAYER_DIAGONAL_SPEED \
    ((PLAYER_SPEED * 181L + 128) / 256)

#define PLAYER_MAX_X \
    ((SCREENWIDTH - PLAYER_WIDTH) * POSITION_SCALE)

#define PLAYER_MAX_Y \
    ((SCREENHEIGHT - PLAYER_HEIGHT) * POSITION_SCALE)

#define PLAYER_START_X \
    (((SCREENWIDTH - PLAYER_WIDTH) / 2) * POSITION_SCALE)

#define PLAYER_START_Y \
    ((SCREENHEIGHT - PLAYER_HEIGHT - 16) * POSITION_SCALE)

#define PLAYER_START_LIVES 3
#define PLAYER_INVULNERABILITY_DURATION 120
#define PLAYER_MAX_WEAPON_LEVEL 3

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

static int16_t player_x;
static int16_t player_y;

static uint8_t player_lives;
static uint8_t invulnerability_timer;
static uint8_t weapon_level;

void player_init(void)
{
    player_x = PLAYER_START_X;
    player_y = PLAYER_START_Y;

    player_lives = PLAYER_START_LIVES;
    invulnerability_timer = 0;
    weapon_level = 1;

    set_sprite_data(PLAYER_TILE_ID, 1, player_tile);
    set_sprite_tile(PLAYER_SPRITE_ID, PLAYER_TILE_ID);
    set_sprite_prop(PLAYER_SPRITE_ID, 0);
}

uint8_t player_is_alive(void)
{
    return player_lives > 0;
}

void player_update(uint8_t buttons)
{
    int8_t direction_x = 0;
    int8_t direction_y = 0;
    int16_t speed = PLAYER_SPEED;

    if (!player_is_alive())
    {
        return;
    }

    if (invulnerability_timer > 0)
    {
        invulnerability_timer--;
    }

    if (buttons & J_LEFT)
    {
        direction_x--;
    }

    if (buttons & J_RIGHT)
    {
        direction_x++;
    }

    if (buttons & J_UP)
    {
        direction_y--;
    }

    if (buttons & J_DOWN)
    {
        direction_y++;
    }

    if (direction_x != 0 && direction_y != 0)
    {
        speed = PLAYER_DIAGONAL_SPEED;
    }

    if (direction_x < 0)
    {
        player_x -= speed;
    }
    else if (direction_x > 0)
    {
        player_x += speed;
    }

    if (direction_y < 0)
    {
        player_y -= speed;
    }
    else if (direction_y > 0)
    {
        player_y += speed;
    }

    if (player_x < 0)
    {
        player_x = 0;
    }
    else if (player_x > PLAYER_MAX_X)
    {
        player_x = PLAYER_MAX_X;
    }

    if (player_y < 0)
    {
        player_y = 0;
    }
    else if (player_y > PLAYER_MAX_Y)
    {
        player_y = PLAYER_MAX_Y;
    }

    if (powerup_collect(
            player_x / POSITION_SCALE,
            player_y / POSITION_SCALE,
            PLAYER_WIDTH,
            PLAYER_HEIGHT))
    {
        if (weapon_level < PLAYER_MAX_WEAPON_LEVEL)
        {
            weapon_level++;
        }
    }

    if (buttons & J_A)
    {
        shots_spawn(
            (uint8_t)(player_x / POSITION_SCALE),
            (uint8_t)(player_y / POSITION_SCALE),
            weapon_level);
    }
}

void player_check_collision(void)
{
    uint8_t hit;
    int16_t x;
    int16_t y;

    if (!player_is_alive() || invulnerability_timer > 0)
    {
        return;
    }

    x = player_x / POSITION_SCALE;
    y = player_y / POSITION_SCALE;

    hit = enemy_shots_hit(
        x,
        y,
        PLAYER_WIDTH,
        PLAYER_HEIGHT);

    if (!hit)
    {
        hit = enemy_touch(
            x,
            y,
            PLAYER_WIDTH,
            PLAYER_HEIGHT);
    }

    if (hit)
    {
        player_lives--;

        if (weapon_level > 1)
        {
            weapon_level--;
        }

        if (player_is_alive())
        {
            player_x = PLAYER_START_X;
            player_y = PLAYER_START_Y;

            invulnerability_timer =
                PLAYER_INVULNERABILITY_DURATION;
        }
    }
}

void player_render(void)
{
    if (!player_is_alive())
    {
        move_sprite(PLAYER_SPRITE_ID, 0, 0);

        return;
    }

    if (
        invulnerability_timer > 0 &&
        (invulnerability_timer & 4))
    {
        move_sprite(PLAYER_SPRITE_ID, 0, 0);

        return;
    }

    move_sprite(
        PLAYER_SPRITE_ID,
        (uint8_t)(player_x / POSITION_SCALE) + SPRITE_OFFSET_X,
        (uint8_t)(player_y / POSITION_SCALE) + SPRITE_OFFSET_Y);
}

uint8_t player_get_lives(void)
{
    return player_lives;
}
