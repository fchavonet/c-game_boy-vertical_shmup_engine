#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

typedef enum
{
    ENEMY_MOVE_DOWN,
    ENEMY_MOVE_DIAGONAL_LEFT,
    ENEMY_MOVE_DIAGONAL_RIGHT,
    ENEMY_MOVE_ZIGZAG,
    ENEMY_MOVE_SEQUENCE
} EnemyMovement;

typedef enum
{
    ENEMY_SHOT_NONE,
    ENEMY_SHOT_STRAIGHT,
    ENEMY_SHOT_AIMED,
    ENEMY_SHOT_SPREAD
} EnemyShotMode;

typedef struct
{
    uint16_t duration;
    int8_t direction_x;
    int8_t direction_y;
    uint8_t can_shoot;
} EnemyMovementStep;

typedef struct
{
    const EnemyMovementStep *steps;
    uint8_t step_count;
} EnemyPath;

typedef struct
{
    uint8_t start_hp;
    uint8_t speed;

    EnemyShotMode shot_mode;
    uint8_t first_shot_delay;
    uint8_t shot_interval;
    uint8_t shot_speed; /* Sixteenths of a pixel per update */

    uint16_t score_value;
    uint8_t tile_id;
} EnemyDefinition;

extern const EnemyDefinition enemy_standard;
extern const EnemyDefinition enemy_resistant;
extern const EnemyDefinition enemy_spread;

void enemy_init(void);

uint8_t enemy_spawn(
    uint8_t x,
    uint8_t y,
    EnemyMovement movement,
    const EnemyDefinition *definition,
    const EnemyPath *path);

void enemy_update(void);
void enemy_render(void);

uint8_t enemy_is_clear(void);

uint8_t enemy_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
