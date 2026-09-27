#ifndef BOSS_H
#define BOSS_H

#include <stdint.h>

void boss_init(void);
void boss_start(void);
void boss_update(void);
void boss_render(void);

uint8_t boss_is_defeated(void);

uint8_t boss_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
