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
    /* Introduce the three enemy types */
    {60, 32, 0, ENEMY_MOVE_DOWN, &enemy_standard},
    {60, 76, 0, ENEMY_MOVE_DOWN, &enemy_resistant},
    {60, 120, 0, ENEMY_MOVE_DOWN, &enemy_spread},

    /* Left diagonals */
    {240, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard},
    {285, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard},

    /* Right diagonals */
    {480, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard},
    {525, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard},

    /* Mixed zigzag pair */
    {720, 32, 0, ENEMY_MOVE_ZIGZAG, &enemy_spread},
    {720, 96, 0, ENEMY_MOVE_ZIGZAG, &enemy_resistant}};

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
    {60, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard},
    {60, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard},

    {110, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard},
    {110, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard},

    /* Mixed straight formation */
    {300, 16, 0, ENEMY_MOVE_DOWN, &enemy_standard},
    {300, 56, 0, ENEMY_MOVE_DOWN, &enemy_resistant},
    {300, 96, 0, ENEMY_MOVE_DOWN, &enemy_spread},
    {300, 136, 0, ENEMY_MOVE_DOWN, &enemy_standard},

    /* Mixed zigzag formation */
    {540, 16, 0, ENEMY_MOVE_ZIGZAG, &enemy_standard},
    {540, 64, 0, ENEMY_MOVE_ZIGZAG, &enemy_spread},
    {540, 112, 0, ENEMY_MOVE_ZIGZAG, &enemy_standard},

    /* Final armed pair */
    {780, 40, 0, ENEMY_MOVE_DOWN, &enemy_resistant},
    {780, 112, 0, ENEMY_MOVE_DOWN, &enemy_spread}};

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
