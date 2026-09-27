#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "player.h"
#include "player_shots.h"
#include "enemy.h"
#include "enemy_shots.h"
#include "powerup.h"
#include "boss.h"

#define PLAYER_WIDTH 8
#define PLAYER_HEIGHT 8

#define POSITION_SCALE 16

#define PLAYER_SPEED 24
#define PLAYER_DIAGONAL_SPEED \
    ((PLAYER_SPEED * 181L + 128) / 256)

#define PLAYER_MAX_X \
    ((GAME_PLAYFIELD_WIDTH - PLAYER_WIDTH) * POSITION_SCALE)

#define PLAYER_MAX_Y \
    ((GAME_PLAYFIELD_HEIGHT - PLAYER_HEIGHT) * POSITION_SCALE)

#define PLAYER_START_X \
    (((GAME_PLAYFIELD_WIDTH - PLAYER_WIDTH) / 2) * POSITION_SCALE)

#define PLAYER_START_Y \
    ((GAME_PLAYFIELD_HEIGHT - PLAYER_HEIGHT - 16) * POSITION_SCALE)

#define PLAYER_MAX_LIVES 3
#define PLAYER_START_LIVES PLAYER_MAX_LIVES

#define PLAYER_INVULNERABILITY_DURATION 120
#define PLAYER_MAX_WEAPON_LEVEL 3

#define PLAYER_EXPLOSION_FRAME_DURATION 4u

#define PLAYER_EXPLOSION_DURATION \
    (GFX_EXPLOSION_TILE_COUNT * PLAYER_EXPLOSION_FRAME_DURATION)

#define PLAYER_RESPAWN_DELAY 18

#define PLAYER_DESTRUCTION_DURATION \
    (PLAYER_EXPLOSION_DURATION + PLAYER_RESPAWN_DELAY)

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
static uint8_t destruction_timer;

void player_init(void)
{
    player_x = PLAYER_START_X;
    player_y = PLAYER_START_Y;

    player_lives = PLAYER_START_LIVES;
    invulnerability_timer = 0;
    weapon_level = 1;
    destruction_timer = 0;

    set_sprite_data(GFX_PLAYER_TILE_ID, 1, player_tile);
    set_sprite_tile(GFX_PLAYER_SPRITE_ID, GFX_PLAYER_TILE_ID);
    set_sprite_prop(GFX_PLAYER_SPRITE_ID, 0);
}

uint8_t player_is_alive(void)
{
    return player_lives > 0;
}

uint8_t player_is_destroying(void)
{
    return destruction_timer > 0;
}

void player_update(uint8_t buttons)
{
    int8_t direction_x = 0;
    int8_t direction_y = 0;
    int16_t speed = PLAYER_SPEED;

    if (player_is_destroying())
    {
        destruction_timer--;

        if (destruction_timer == 0 && player_is_alive())
        {
            player_x = PLAYER_START_X;
            player_y = PLAYER_START_Y;

            invulnerability_timer =
                PLAYER_INVULNERABILITY_DURATION;
        }

        return;
    }

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

    if (
        !player_is_alive() ||
        player_is_destroying() ||
        invulnerability_timer > 0)
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

    if (!hit)
    {
        hit = boss_touch(
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

        invulnerability_timer = 0;

        destruction_timer =
            (uint8_t)PLAYER_DESTRUCTION_DURATION;
    }
}

void player_render(void)
{
    uint8_t elapsed;
    uint8_t frame;
    uint8_t tile_id;

    if (player_is_destroying())
    {
        elapsed = (uint8_t)(PLAYER_DESTRUCTION_DURATION - destruction_timer);

        if (elapsed < PLAYER_EXPLOSION_DURATION)
        {
            frame = elapsed / PLAYER_EXPLOSION_FRAME_DURATION;

            tile_id = (uint8_t)(GFX_EXPLOSION_FIRST_TILE_ID + frame);

            set_sprite_tile(GFX_PLAYER_SPRITE_ID, tile_id);

            move_sprite(
                GFX_PLAYER_SPRITE_ID,
                (uint8_t)(player_x / POSITION_SCALE) + GFX_SPRITE_OFFSET_X,
                (uint8_t)(player_y / POSITION_SCALE) + GFX_SPRITE_OFFSET_Y);
        }
        else
        {
            move_sprite(GFX_PLAYER_SPRITE_ID, 0, 0);
        }

        return;
    }

    if (!player_is_alive())
    {
        move_sprite(GFX_PLAYER_SPRITE_ID, 0, 0);
        return;
    }

    set_sprite_tile(GFX_PLAYER_SPRITE_ID, GFX_PLAYER_TILE_ID);

    if (
        invulnerability_timer > 0 &&
        (invulnerability_timer & 4))
    {
        move_sprite(GFX_PLAYER_SPRITE_ID, 0, 0);
        return;
    }

    move_sprite(
        GFX_PLAYER_SPRITE_ID,
        (uint8_t)(player_x / POSITION_SCALE) + GFX_SPRITE_OFFSET_X,
        (uint8_t)(player_y / POSITION_SCALE) + GFX_SPRITE_OFFSET_Y);
}

uint8_t player_get_lives(void)
{
    return player_lives;
}

uint8_t player_add_life(void)
{
    /*
     * Extra lives cannot revive a player with no lives left.
     */
    if (!player_is_alive())
    {
        return 0;
    }

    if (player_lives >= PLAYER_MAX_LIVES)
    {
        return 0;
    }

    player_lives++;
    return 1;
}

uint8_t player_get_center_x(void)
{
    return (uint8_t)(player_x / POSITION_SCALE + PLAYER_WIDTH / 2);
}

uint8_t player_get_center_y(void)
{
    return (uint8_t)(player_y / POSITION_SCALE + PLAYER_HEIGHT / 2);
}
