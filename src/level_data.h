#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <stdint.h>

#include "enemy.h"

typedef struct
{
    uint16_t frame;
    uint8_t x;
    uint8_t y;
    EnemyMovement movement;
} SpawnEvent;

typedef struct
{
    const SpawnEvent *events;
    uint16_t event_count;
    uint16_t boss_start_frame;
} LevelDefinition;

extern const LevelDefinition level_one;

#endif
