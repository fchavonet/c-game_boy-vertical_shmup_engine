#include "level_data.h"
#include "enemy_shots.h"

/*
 * Reusable movement paths.
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
    35, /* Phase two shot interval */

    BOSS_SHOT_STRAIGHT,
    BOSS_SHOT_DOUBLE,

    SHOT_SPEED_2, /* Phase one projectile speed */
    SHOT_SPEED_2  /* Phase two projectile speed */
};

/* Reusable recipes: type, movement, path, formation, count, interval. */
static const WaveDefinition standard_train = {
    &enemy_standard, ENEMY_MOVE_DOWN, 0, &wave_single, 5, 20};
static const WaveDefinition standard_line = {
    &enemy_standard, ENEMY_MOVE_DOWN, 0, &wave_line_5, 5, 0};
static const WaveDefinition standard_v = {
    &enemy_standard, ENEMY_MOVE_DOWN, 0, &wave_v_5, 5, 0};
static const WaveDefinition standard_inverted_v = {
    &enemy_standard, ENEMY_MOVE_DOWN, 0, &wave_inverted_v_5, 5, 0};
static const WaveDefinition crossing_right = {
    &enemy_standard, ENEMY_MOVE_DIAGONAL_RIGHT, 0, &wave_single, 3, 30};
static const WaveDefinition crossing_left = {
    &enemy_standard, ENEMY_MOVE_DIAGONAL_LEFT, 0, &wave_single, 3, 30};
static const WaveDefinition resistant_train = {
    &enemy_resistant, ENEMY_MOVE_SEQUENCE, &attack_exit_left,
    &wave_single, 3, 45};

/* Individual events remain available for unique enemies or escorts. */
static const SpawnEvent level_one_events[] = {
    {800, 76, 0, ENEMY_MOVE_SEQUENCE, &enemy_resistant, &attack_exit_left},
    {800, 120, 0, ENEMY_MOVE_SEQUENCE, &enemy_spread, &attack_exit_right}};

static const WaveEvent level_one_waves[] = {
    {60, 76, 0, &standard_train},
    {300, 76, 0, &standard_line},
    {480, 76, 0, &standard_v},
    {660, 76, 0, &standard_inverted_v}};

const LevelDefinition level_one = {
    level_one_events,

    sizeof(level_one_events) /
        sizeof(level_one_events[0]),

    900,

    &level_one_boss,
    level_one_waves,
    sizeof(level_one_waves) / sizeof(level_one_waves[0])};

/*
 * Level two.
 */

static const BossDefinition level_two_boss = {
    32, /* Initial health */
    16, /* Phase two health threshold */

    1, /* Phase one movement speed */
    2, /* Phase two movement speed */

    45, /* Phase one shot interval */
    60, /* Phase two shot interval */

    BOSS_SHOT_AIMED,
    BOSS_SHOT_SPREAD,

    SHOT_SPEED_1_5,  /* Phase one projectile speed */
    SHOT_SPEED_1_75  /* Phase two projectile speed */
};

static const SpawnEvent level_two_events[] = {
    {540, 112, 0, ENEMY_MOVE_ZIGZAG, &enemy_standard, 0},
    {780, 112, 0, ENEMY_MOVE_SEQUENCE, &enemy_spread, &attack_exit_right}};

static const WaveEvent level_two_waves[] = {
    {60, 24, 0, &crossing_right},
    {60, 128, 0, &crossing_left},
    {300, 76, 0, &standard_v},
    {540, 40, 0, &resistant_train},
    {780, 76, 0, &standard_line}};

const LevelDefinition level_two = {
    level_two_events,

    sizeof(level_two_events) /
        sizeof(level_two_events[0]),

    960,

    &level_two_boss,
    level_two_waves,
    sizeof(level_two_waves) / sizeof(level_two_waves[0])};

/*
 * Campaign order.
 */

const LevelDefinition *const levels[LEVEL_COUNT] = {
    &level_one,
    &level_two};
