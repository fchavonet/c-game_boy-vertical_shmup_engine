#ifndef WAVES_H
#define WAVES_H

#include <stdint.h>
#include "enemy.h"

#define WAVE_MAX_ACTIVE 3u
#define WAVE_MAX_POINTS 6u

typedef struct
{
    int8_t x;
    int8_t y;
} WavePoint;

typedef struct
{
    const WavePoint *points;
    uint8_t point_count;
} WaveFormation;

typedef struct
{
    const EnemyDefinition *enemy;
    EnemyMovement movement;
    const EnemyPath *path;
    const WaveFormation *formation;
    uint8_t count;
    uint16_t interval; /* Updates between successful spawns; 0 = atomic group. */
} WaveDefinition;

typedef enum
{
    WAVE_INVALID,
    WAVE_BUSY,
    WAVE_STARTED
} WaveStartResult;

extern const WaveFormation wave_single;
extern const WaveFormation wave_line_3;
extern const WaveFormation wave_line_5;
extern const WaveFormation wave_v_5;
extern const WaveFormation wave_inverted_v_5;

void waves_init(void);
WaveStartResult waves_start(const WaveDefinition *definition, uint8_t x, uint8_t y);
void waves_update(void);
uint8_t waves_is_clear(void);

#endif
