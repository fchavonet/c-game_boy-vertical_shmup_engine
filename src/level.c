#include <stdint.h>

#include "level.h"
#include "enemy.h"

typedef struct
{
    uint16_t frame;
    uint8_t x;
    uint8_t y;
} SpawnEvent;

static const SpawnEvent spawn_events[] = {
    {60, 24, 0},
    {90, 48, 0},
    {120, 72, 0},
    {150, 96, 0},
    {180, 120, 0},

    {300, 120, 0},
    {330, 96, 0},
    {360, 72, 0},
    {390, 48, 0},
    {420, 24, 0},

    {540, 32, 0},
    {540, 76, 0},
    {540, 120, 0}};

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
                spawn_events[next_event].y))
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
