#include <stdint.h>

#include "level.h"
#include "enemy.h"
#include "boss.h"

static const LevelDefinition *current_level;

static uint16_t level_frame;
static uint16_t next_event;
static uint8_t boss_started;

void level_init(const LevelDefinition *definition)
{
    current_level = definition;

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
    const SpawnEvent *event;

    if (boss_started)
    {
        return;
    }

    while (next_event < current_level->event_count)
    {
        event = &current_level->events[next_event];

        if (event->frame > level_frame)
        {
            break;
        }

        if (!enemy_spawn(
                event->x,
                event->y,
                event->movement))
        {
            return;
        }

        next_event++;
    }

    if (
        next_event >= current_level->event_count &&
        level_frame >= current_level->boss_start_frame)
    {
        if (!enemy_is_clear())
        {
            return;
        }

        boss_start(current_level->boss);
        boss_started = 1;
        return;
    }

    level_frame++;
}
