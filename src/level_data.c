#include "level_data.h"

/*
 * Reusable movement paths.
 *
 * Each step contains:
 * duration, horizontal direction, vertical direction,
 * shooting permission.
 */

static const EnemyMovementStep attack_exit_left_steps[] = {
    {32, 0, 1, 0},
    {60, 0, 0, 1},
    {120, -1, 1, 0}};

static const EnemyPath attack_exit_left = {
    attack_exit_left_steps,

    sizeof(attack_exit_left_steps) /
        sizeof(attack_exit_left_steps[0])};

static const EnemyMovementStep attack_exit_right_steps[] = {
    {32, 0, 1, 0},
    {60, 0, 0, 1},
    {120, 1, 1, 0}};

static const EnemyPath attack_exit_right = {
    attack_exit_right_steps,

    sizeof(attack_exit_right_steps) /
        sizeof(attack_exit_right_steps[0])};

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
    /* Compare both paths immediately */
    {60, 32, 0, ENEMY_MOVE_DOWN, &enemy_standard, 0},
    {60, 76, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_resistant,
     &attack_exit_left},
    {60, 120, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_spread,
     &attack_exit_right},

    /* Left diagonals */
    {240, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard, 0},
    {285, 120, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard, 0},

    /* Right diagonals */
    {480, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard, 0},
    {525, 32, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard, 0},

    /* Two attackers sharing the same timing */
    {
        720, 32, 0,
        ENEMY_MOVE_SEQUENCE,
        &enemy_spread,
        &attack_exit_left},
    {720, 96, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_resistant,
     &attack_exit_right}};

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
    {60, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard, 0},
    {60, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard, 0},

    {110, 24, 0, ENEMY_MOVE_DIAGONAL_RIGHT, &enemy_standard, 0},
    {110, 128, 0, ENEMY_MOVE_DIAGONAL_LEFT, &enemy_standard, 0},

    /* Attackers leave in opposite directions */
    {300, 16, 0, ENEMY_MOVE_DOWN, &enemy_standard, 0},
    {300, 56, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_resistant,
     &attack_exit_left},
    {300, 96, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_spread,
     &attack_exit_right},
    {300, 136, 0, ENEMY_MOVE_DOWN, &enemy_standard, 0},

    /* Stopping attacker and zigzag escorts */
    {540, 16, 0, ENEMY_MOVE_ZIGZAG, &enemy_standard, 0},
    {540, 64, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_spread,
     &attack_exit_right},
    {540, 112, 0, ENEMY_MOVE_ZIGZAG, &enemy_standard, 0},

    /* Final armed pair */
    {
        780, 40, 0,
        ENEMY_MOVE_SEQUENCE,
        &enemy_resistant,
        &attack_exit_left},
    {780, 112, 0,
     ENEMY_MOVE_SEQUENCE,
     &enemy_spread,
     &attack_exit_right}};

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
