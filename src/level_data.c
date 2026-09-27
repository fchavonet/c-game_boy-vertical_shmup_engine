#include "level_data.h"

/*
 * Level one.
 */

static const BossDefinition level_one_boss = {
    24, /* Initial health */
    12, /* Phase two health threshold */

    1, /* Phase one movement speed */
    2, /* Phase two movement speed */

    60, /* Phase one shot interval */
    35  /* Phase two shot interval */
};

static const SpawnEvent level_one_events[] = {
    {60, 32, 0, ENEMY_MOVE_DOWN},
    {60, 76, 0, ENEMY_MOVE_DOWN},
    {60, 120, 0, ENEMY_MOVE_DOWN},

    {240, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},
    {285, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT},

    {480, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},
    {525, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT},

    {720, 32, 0, ENEMY_MOVE_ZIGZAG},
    {720, 96, 0, ENEMY_MOVE_ZIGZAG}};

const LevelDefinition level_one = {
    level_one_events,

    sizeof(level_one_events) /
        sizeof(level_one_events[0]),

    900,

    &level_one_boss};

/*
 * Level two.
 */

static const BossDefinition level_two_boss = {
    32, /* Initial health */
    16, /* Phase two health threshold */

    1, /* Phase one movement speed */
    2, /* Phase two movement speed */

    45, /* Phase one shot interval */
    30  /* Phase two shot interval */
};

static const SpawnEvent level_two_events[] = {
    /* Crossing diagonals */
    {60, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT},
    {60, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT},

    {110, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT},
    {110, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT},

    /* Straight formation */
    {300, 16, 0, ENEMY_MOVE_DOWN},
    {300, 56, 0, ENEMY_MOVE_DOWN},
    {300, 96, 0, ENEMY_MOVE_DOWN},
    {300, 136, 0, ENEMY_MOVE_DOWN},

    /* Zigzag formation */
    {540, 16, 0, ENEMY_MOVE_ZIGZAG},
    {540, 64, 0, ENEMY_MOVE_ZIGZAG},
    {540, 112, 0, ENEMY_MOVE_ZIGZAG},

    /* Final pair */
    {780, 40, 0, ENEMY_MOVE_DOWN},
    {780, 112, 0, ENEMY_MOVE_DOWN}};

const LevelDefinition level_two = {
    level_two_events,

    sizeof(level_two_events) /
        sizeof(level_two_events[0]),

    960,

    &level_two_boss};

/*
 * Campaign order.
 */

const LevelDefinition *const levels[LEVEL_COUNT] = {
    &level_one,
    &level_two};
