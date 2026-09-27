#include "level_data.h"

static const SpawnEvent level_one_events[] = {
    /* First wave */
    {60, 32, 0, ENEMY_MOVE_DOWN},
    {60, 76, 0, ENEMY_MOVE_DOWN},
    {60, 120, 0, ENEMY_MOVE_DOWN},

    /* Second wave */
    {240, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},
    {285, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},

    /* Third wave */
    {480, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},
    {525, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},

    /* Fourth wave */
    {720, 32, 0, ENEMY_MOVE_ZIGZAG},
    {720, 96, 0, ENEMY_MOVE_ZIGZAG}};

const LevelDefinition level_one = {
    level_one_events,

    sizeof(level_one_events) /
        sizeof(level_one_events[0]),

    900};
