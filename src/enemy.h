#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

typedef enum
{
    ENEMY_MOVE_DOWN,
    ENEMY_MOVE_DIAGONAL_LEFT,
    ENEMY_MOVE_DIAGONAL_RIGHT,
    ENEMY_MOVE_ZIGZAG
} EnemyMovement;

void enemy_init(void);

uint8_t enemy_spawn(
    uint8_t x,
    uint8_t y,
    EnemyMovement movement);

void enemy_update(void);
void enemy_render(void);

uint8_t enemy_is_clear(void);

uint8_t enemy_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
