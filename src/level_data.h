#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <stdint.h>

#include "enemy.h"
#include "boss.h"

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
    const SpawnEvent *events;
    uint16_t event_count;
    uint16_t boss_start_frame;

    const BossDefinition *boss;
} LevelDefinition;

extern const LevelDefinition level_one;
extern const LevelDefinition level_two;

extern const LevelDefinition *const levels[LEVEL_COUNT];

#endif
