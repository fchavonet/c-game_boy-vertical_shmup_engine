#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <stdint.h>

#include "enemy.h"
#include "boss.h"
#include "waves.h"

#define LEVEL_COUNT 2

typedef struct
{
    uint16_t frame;
    uint8_t x;
    uint8_t y;

    EnemyMovement movement;
    const EnemyDefinition *enemy;
    const EnemyPath *path;
} SpawnEvent;

typedef struct
{
    uint16_t frame;
    uint8_t x;
    uint8_t y;
    const WaveDefinition *wave;
} WaveEvent;

typedef struct
{
    const SpawnEvent *events;
    uint16_t event_count;
    uint16_t boss_start_frame;

    const BossDefinition *boss;

    const WaveEvent *waves;
    uint16_t wave_count;
} LevelDefinition;

extern const LevelDefinition level_one;
extern const LevelDefinition level_two;

extern const LevelDefinition *const levels[LEVEL_COUNT];

#endif
