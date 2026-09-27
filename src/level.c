#include <stdint.h>

#include "level.h"
#include "enemy.h"
#include "boss.h"

#define BOSS_START_FRAME 900

typedef struct
{
    uint16_t frame;
    uint8_t x;
    uint8_t y;
    EnemyMovement movement;
} SpawnEvent;

static const SpawnEvent spawn_events[] = {
    {60, 32, 0, ENEMY_MOVE_DOWN},
    {60, 76, 0, ENEMY_MOVE_DOWN},
    {60, 120, 0, ENEMY_MOVE_DOWN},

    {240, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},
    {285, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},

    {480, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},
    {525, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},

    {720, 32, 0, ENEMY_MOVE_ZIGZAG},
    {720, 96, 0, ENEMY_MOVE_ZIGZAG}};

#define SPAWN_EVENT_COUNT \
    (sizeof(spawn_events) / sizeof(spawn_events[0]))

static uint16_t level_frame;
static uint16_t next_event;
static uint8_t boss_started;

void level_init(void)
{
    level_frame = 0;
    next_event = 0;
    boss_started = 0;
}

uint8_t level_is_complete(void)
{
    return boss_is_defeated();
}

void level_update(void)
{
    if (boss_started)
    {
        return;
    }

    while (next_event < SPAWN_EVENT_COUNT)
    {
        if (spawn_events[next_event].frame > level_frame)
        {
            break;
        }

        if (!enemy_spawn(
                spawn_events[next_event].x,
                spawn_events[next_event].y,
                spawn_events[next_event].movement))
        {
            return;
        }

        next_event++;
    }

    if (
        next_event >= SPAWN_EVENT_COUNT &&
        level_frame >= BOSS_START_FRAME)
    {
        boss_start();
        boss_started = 1;

        return;
    }

    level_frame++;
}
