#ifndef ENEMY_SHOTS_H
#define ENEMY_SHOTS_H

#include <stdint.h>

void enemy_shots_init(void);
void enemy_shots_update(void);
void enemy_shots_spawn(int16_t x, int16_t y);
void enemy_shots_render(void);

uint8_t enemy_shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif