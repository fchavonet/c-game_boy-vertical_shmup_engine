#ifndef ENEMY_SHOTS_H
#define ENEMY_SHOTS_H

#include <stdint.h>

/* Speeds in sixteenths of a pixel per update. */
#define SHOT_SPEED_1 16u
#define SHOT_SPEED_1_5 24u
#define SHOT_SPEED_1_75 28u
#define SHOT_SPEED_2 32u
#define SHOT_SPEED_MIN 8u
#define SHOT_SPEED_MAX 64u

void enemy_shots_init(void);
void enemy_shots_update(void);
void enemy_shots_render(void);

void enemy_shots_spawn(int16_t x, int16_t y, uint8_t speed);

void enemy_shots_spawn_aimed(
    int16_t x,
    int16_t y,
    int16_t target_x,
    int16_t target_y,
    uint8_t speed);

void enemy_shots_spawn_spread(int16_t x, int16_t y, uint8_t speed);

uint8_t enemy_shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
