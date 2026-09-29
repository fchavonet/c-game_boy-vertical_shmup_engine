#include "waves.h"
#include "game_config.h"
#include "graphics_layout.h"

/* Coordinates are relative to the event's anchor, in screen pixels. */
static const WavePoint single_points[] = {{0, 0}};
static const WavePoint line_3_points[] = {{-24, 0}, {0, 0}, {24, 0}};
static const WavePoint line_5_points[] = {
    {-48, 0}, {-24, 0}, {0, 0}, {24, 0}, {48, 0}};
/* The V tip is below the wings; screen Y increases downwards. */
static const WavePoint v_points[] = {
    {0, 24}, {-24, 12}, {24, 12}, {-48, 0}, {48, 0}};
static const WavePoint inverted_v_points[] = {
    {0, 0}, {-24, 12}, {24, 12}, {-48, 24}, {48, 24}};

const WaveFormation wave_single = {single_points, 1};
const WaveFormation wave_line_3 = {line_3_points, 3};
const WaveFormation wave_line_5 = {line_5_points, 5};
const WaveFormation wave_v_5 = {v_points, 5};
const WaveFormation wave_inverted_v_5 = {inverted_v_points, 5};

typedef struct
{
    const WaveDefinition *definition;
    uint16_t delay;
    uint8_t x;
    uint8_t y;
    uint8_t remaining;
    uint8_t point;
} ActiveWave;

static ActiveWave active_waves[WAVE_MAX_ACTIVE];
static uint8_t active_count;

void waves_init(void)
{
    uint8_t i;

    active_count = 0;
    for (i = 0; i < WAVE_MAX_ACTIVE; i++)
    {
        active_waves[i].remaining = 0;
    }
}

WaveStartResult waves_start(const WaveDefinition *definition, uint8_t x, uint8_t y)
{
    static uint8_t i;
    static uint8_t points_to_check;
    static int16_t spawn_x;
    static int16_t spawn_y;
    static const WaveFormation *formation;

    if (definition == 0 || definition->count == 0)
    {
        return WAVE_INVALID;
    }

    formation = definition->formation;
    if (formation == 0 || formation->points == 0 ||
        formation->point_count == 0 || formation->point_count > WAVE_MAX_POINTS)
    {
        return WAVE_INVALID;
    }

    if (definition->interval == 0 && definition->count > GFX_ENEMY_SPRITE_COUNT)
    {
        return WAVE_INVALID;
    }

    if (active_count >= WAVE_MAX_ACTIVE)
    {
        return WAVE_BUSY;
    }

    points_to_check = formation->point_count;
    if (definition->count < points_to_check)
    {
        points_to_check = definition->count;
    }

    /* Validate once at activation, never in the per-frame movement loop. */
    for (i = 0; i < points_to_check; i++)
    {
        spawn_x = (int16_t)x + formation->points[i].x;
        spawn_y = (int16_t)y + formation->points[i].y;
        if (spawn_x < 0 || spawn_x > GAME_PLAYFIELD_WIDTH - 8 ||
            spawn_y < 0 || spawn_y > GAME_PLAYFIELD_HEIGHT - 8)
        {
            return WAVE_INVALID;
        }
    }
    /* All members share one enemy definition and one movement. */
    if (!enemy_can_spawn((uint8_t)spawn_x, (uint8_t)spawn_y,
            definition->movement, definition->enemy, definition->path))
    {
        return WAVE_INVALID;
    }

    for (i = 0; i < WAVE_MAX_ACTIVE; i++)
    {
        if (active_waves[i].remaining == 0)
        {
            active_waves[i].definition = definition;
            active_waves[i].x = x;
            active_waves[i].y = y;
            active_waves[i].remaining = definition->count;
            active_waves[i].point = 0;
            active_waves[i].delay = 0;
            active_count++;
            return WAVE_STARTED;
        }
    }
    return WAVE_BUSY;
}

void waves_update(void)
{
    static uint8_t i;
    static ActiveWave *wave;
    static const WaveDefinition *definition;
    static const WaveFormation *formation;
    static const WavePoint *point;

    if (active_count == 0)
    {
        return;
    }

    for (i = 0, wave = active_waves; i < WAVE_MAX_ACTIVE; i++, wave++)
    {
        if (wave->remaining == 0)
        {
            continue;
        }
        if (wave->delay > 0)
        {
            wave->delay--;
            if (wave->delay > 0)
            {
                continue;
            }
        }

        definition = wave->definition;
        formation = definition->formation;

        /* A simultaneous formation waits until ALL its members fit. */
        if (definition->interval == 0 && enemy_free_count() < wave->remaining)
        {
            continue;
        }

        do
        {
            point = &formation->points[wave->point];
            if (!enemy_spawn_validated(
                    (uint8_t)((int16_t)wave->x + point->x),
                    (uint8_t)((int16_t)wave->y + point->y),
                    definition->movement, definition->enemy, definition->path))
            {
                break; /* Keep this member pending; retry next update. */
            }
            wave->remaining--;
            if (wave->remaining == 0)
            {
                active_count--;
                break;
            }
            wave->point++;
            if (wave->point >= formation->point_count)
            {
                wave->point = 0;
            }
            if (definition->interval > 0)
            {
                wave->delay = definition->interval;
                break;
            }
        } while (wave->remaining > 0);
    }
}

uint8_t waves_is_clear(void)
{
    return active_count == 0;
}
