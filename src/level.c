#include <stdint.h>

#include "level.h"
#include "enemy.h"
#include "boss.h"
#include "waves.h"

static const LevelDefinition *current_level;
static uint16_t level_frame;
static uint16_t next_event;
static uint16_t next_wave;
static uint8_t boss_started;
static uint8_t invalid_event_count;

void level_init(const LevelDefinition *definition)
{
    current_level = definition;
    level_frame = 0;
    next_event = 0;
    next_wave = 0;
    boss_started = 0;
    invalid_event_count = 0;
    waves_init();
}

uint8_t level_is_complete(void)
{
    return boss_is_defeated();
}

uint8_t level_get_invalid_event_count(void)
{
    return invalid_event_count;
}

static void level_invalid_event(void)
{
    if (invalid_event_count < 255u)
    {
        invalid_event_count++;
    }
}

void level_update(void)
{
    static const SpawnEvent *event;
    static const WaveEvent *wave;
    static WaveStartResult result;
    static uint8_t blocked;

    if (boss_started || current_level == 0)
    {
        return;
    }

    blocked = 0;
    /* Both arrays must be sorted by frame. Singles win ties. */
    while (1)
    {
        event = 0;
        wave = 0;
        if (next_event < current_level->event_count)
        {
            event = &current_level->events[next_event];
        }
        if (next_wave < current_level->wave_count)
        {
            wave = &current_level->waves[next_wave];
        }
        if (event == 0 && wave == 0)
        {
            break;
        }

        if (event != 0 && (wave == 0 || event->frame <= wave->frame))
        {
            if (event->frame > level_frame)
            {
                break;
            }
            if (!enemy_spawn(event->x, event->y, event->movement,
                    event->enemy, event->path))
            {
                if (enemy_can_spawn(event->x, event->y, event->movement,
                        event->enemy, event->path))
                {
                    blocked = 1;
                    break;
                }
                level_invalid_event();
            }
            next_event++;
        }
        else
        {
            if (wave->frame > level_frame)
            {
                break;
            }
            result = waves_start(wave->wave, wave->x, wave->y);
            if (result == WAVE_BUSY)
            {
                blocked = 1;
                break;
            }
            if (result == WAVE_INVALID)
            {
                level_invalid_event();
            }
            next_wave++;
        }
    }

    /* Existing waves keep progressing even if the level clock is blocked.
     * Otherwise a full scheduler could deadlock while waiting for a slot.
     */
    waves_update();

    if (blocked)
    {
        return;
    }

    if (next_event >= current_level->event_count &&
        next_wave >= current_level->wave_count &&
        level_frame >= current_level->boss_start_frame)
    {
        if (!waves_is_clear() || !enemy_is_clear())
        {
            return;
        }
        boss_start(current_level->boss);
        boss_started = 1;
        return;
    }

    /* Long waits cannot wrap the timeline back to the start. */
    if (level_frame < 65535u)
    {
        level_frame++;
    }
}
