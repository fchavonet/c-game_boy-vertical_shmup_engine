#ifndef POWERUP_H
#define POWERUP_H

#include <stdint.h>

void powerup_init(void);
void powerup_update(void);
void powerup_render(void);

void powerup_on_enemy_destroyed(int16_t x, int16_t y);

uint8_t powerup_collect(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
