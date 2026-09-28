#include <gb/gb.h>

#include "game_config.h"
#include "graphics_layout.h"
#include "enemy_shots.h"

#define ENEMY_SHOT_COUNT GFX_ENEMY_SHOT_SPRITE_COUNT

#define ENEMY_SHOT_WIDTH 4
#define ENEMY_SHOT_HEIGHT 4

#define SHOT_POSITION_SCALE 16
#define ENEMY_SHOT_SPEED 2

#define SHOT_FIXED_SPEED \
    (ENEMY_SHOT_SPEED * SHOT_POSITION_SCALE)

#define SPREAD_SHOT_COUNT 3u

#define SPREAD_VELOCITY_X \
    (SHOT_FIXED_SPEED / 2)

#define SPREAD_VELOCITY_Y \
    ((SHOT_FIXED_SPEED * 7) / 8)

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t velocity_x;
    int16_t velocity_y;
    uint8_t active;
} EnemyShot;

static EnemyShot enemy_shots[ENEMY_SHOT_COUNT];

static const uint8_t enemy_shot_tile[] = {
    0x60, 0x60,
    0xF0, 0xF0,
    0xF0, 0xF0,
    0x60, 0x60,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00};

static uint16_t integer_sqrt(uint16_t value)
{
    uint16_t root = 0;
    uint16_t bit = 0x4000u;

    while (bit != 0u)
    {
        if (value >= root + bit)
        {
            value -= root + bit;
            root = (root >> 1u) + bit;
        }
        else
        {
            root >>= 1u;
        }

        bit >>= 2u;
    }

    return root;
}

static uint8_t shot_position_is_valid(int16_t x, int16_t y)
{
    if (
        x < 0 ||
        x > GAME_PLAYFIELD_WIDTH - ENEMY_SHOT_WIDTH ||
        y < 0 ||
        y > GAME_PLAYFIELD_HEIGHT - ENEMY_SHOT_HEIGHT)
    {
        return 0;
    }

    return 1;
}

static void spawn_with_velocity(
    int16_t x,
    int16_t y,
    int16_t velocity_x,
    int16_t velocity_y)
{
    static uint8_t i;

    if (!shot_position_is_valid(x, y))
    {
        return;
    }

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (!enemy_shots[i].active)
        {
            enemy_shots[i].x = x * SHOT_POSITION_SCALE;
            enemy_shots[i].y = y * SHOT_POSITION_SCALE;

            enemy_shots[i].velocity_x = velocity_x;
            enemy_shots[i].velocity_y = velocity_y;

            enemy_shots[i].active = 1;
            return;
        }
    }
}

void enemy_shots_init(void)
{
    uint8_t i;
    uint8_t sprite_id;

    set_sprite_data(
        GFX_ENEMY_SHOT_TILE_ID,
        1,
        enemy_shot_tile);

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        enemy_shots[i].x = 0;
        enemy_shots[i].y = 0;
        enemy_shots[i].velocity_x = 0;
        enemy_shots[i].velocity_y = 0;
        enemy_shots[i].active = 0;

        sprite_id = GFX_ENEMY_SHOT_FIRST_SPRITE_ID + i;

        set_sprite_tile(sprite_id, GFX_ENEMY_SHOT_TILE_ID);
        set_sprite_prop(sprite_id, 0);
        move_sprite(sprite_id, 0, 0);
    }
}

void enemy_shots_spawn(int16_t x, int16_t y)
{
    spawn_with_velocity(
        x,
        y,
        0,
        SHOT_FIXED_SPEED);
}

void enemy_shots_spawn_aimed(
    int16_t x,
    int16_t y,
    int16_t target_x,
    int16_t target_y)
{
    static int16_t delta_x;
    static int16_t delta_y;

    static int16_t velocity_x;
    static int16_t velocity_y;

    static uint16_t absolute_x;
    static uint16_t absolute_y;
    static uint16_t distance_squared;
    static uint16_t distance;

    if (!shot_position_is_valid(x, y))
    {
        return;
    }

    if (
        target_x < 0 ||
        target_x >= GAME_PLAYFIELD_WIDTH ||
        target_y < 0 ||
        target_y >= GAME_PLAYFIELD_HEIGHT)
    {
        return;
    }

    delta_x = target_x - (x + ENEMY_SHOT_WIDTH / 2);
    delta_y = target_y - (y + ENEMY_SHOT_HEIGHT / 2);

    if (delta_x < 0)
    {
        absolute_x = (uint16_t)(-delta_x);
    }
    else
    {
        absolute_x = (uint16_t)delta_x;
    }

    if (delta_y < 0)
    {
        absolute_y = (uint16_t)(-delta_y);
    }
    else
    {
        absolute_y = (uint16_t)delta_y;
    }

    distance_squared =
        absolute_x * absolute_x +
        absolute_y * absolute_y;

    distance = integer_sqrt(distance_squared);

    if (distance == 0u)
    {
        enemy_shots_spawn(x, y);
        return;
    }

    velocity_x =
        (delta_x * SHOT_FIXED_SPEED) / (int16_t)distance;

    velocity_y =
        (delta_y * SHOT_FIXED_SPEED) / (int16_t)distance;

    spawn_with_velocity(
        x,
        y,
        velocity_x,
        velocity_y);
}

void enemy_shots_spawn_spread(int16_t x, int16_t y)
{
    static uint8_t i;
    static uint8_t available;

    available = 0;

    if (!shot_position_is_valid(x, y))
    {
        return;
    }

    /*
     * Reserve enough capacity for the entire volley
     * before creating any projectile.
     */
    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (!enemy_shots[i].active)
        {
            available++;
        }
    }

    if (available < SPREAD_SHOT_COUNT)
    {
        return;
    }

    spawn_with_velocity(
        x,
        y,
        -SPREAD_VELOCITY_X,
        SPREAD_VELOCITY_Y);

    spawn_with_velocity(
        x,
        y,
        0,
        SHOT_FIXED_SPEED);

    spawn_with_velocity(
        x,
        y,
        SPREAD_VELOCITY_X,
        SPREAD_VELOCITY_Y);
}

void enemy_shots_update(void)
{
    static uint8_t i;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (!enemy_shots[i].active)
        {
            continue;
        }

        enemy_shots[i].x += enemy_shots[i].velocity_x;
        enemy_shots[i].y += enemy_shots[i].velocity_y;

        if (
            enemy_shots[i].x <=
                -ENEMY_SHOT_WIDTH * SHOT_POSITION_SCALE ||
            enemy_shots[i].x >=
                GAME_PLAYFIELD_WIDTH * SHOT_POSITION_SCALE ||
            enemy_shots[i].y <=
                -ENEMY_SHOT_HEIGHT * SHOT_POSITION_SCALE ||
            enemy_shots[i].y >=
                GAME_PLAYFIELD_HEIGHT * SHOT_POSITION_SCALE)
        {
            enemy_shots[i].active = 0;
        }
    }
}

uint8_t enemy_shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height)
{
    static uint8_t i;
    static int16_t shot_x;
    static int16_t shot_y;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        if (!enemy_shots[i].active)
        {
            continue;
        }

        shot_x = enemy_shots[i].x / SHOT_POSITION_SCALE;
        shot_y = enemy_shots[i].y / SHOT_POSITION_SCALE;

        if (
            shot_x < x + width &&
            shot_x + ENEMY_SHOT_WIDTH > x &&
            shot_y < y + height &&
            shot_y + ENEMY_SHOT_HEIGHT > y)
        {
            enemy_shots[i].active = 0;
            return 1;
        }
    }

    return 0;
}

void enemy_shots_render(void)
{
    static uint8_t i;
    static uint8_t sprite_id;
    static int16_t shot_x;
    static int16_t shot_y;

    for (i = 0; i < ENEMY_SHOT_COUNT; i++)
    {
        sprite_id = GFX_ENEMY_SHOT_FIRST_SPRITE_ID + i;

        if (!enemy_shots[i].active)
        {
            move_sprite(sprite_id, 0, 0);
            continue;
        }

        shot_x = enemy_shots[i].x / SHOT_POSITION_SCALE;
        shot_y = enemy_shots[i].y / SHOT_POSITION_SCALE;

        move_sprite(
            sprite_id,
            (uint8_t)(shot_x + GFX_SPRITE_OFFSET_X),
            (uint8_t)(shot_y + GFX_SPRITE_OFFSET_Y));
    }
}
