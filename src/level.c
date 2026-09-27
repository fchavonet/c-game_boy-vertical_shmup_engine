#include <stdint.h>

#include "level.h"
#include "enemy.h"

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

void level_init(void)
{
    level_frame = 0;
    next_event = 0;
}

void level_update(void)
{
    if (next_event >= SPAWN_EVENT_COUNT)
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

    if (next_event < SPAWN_EVENT_COUNT)
    {
        level_frame++;
    }
}
