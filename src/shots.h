#ifndef SHOTS_H
#define SHOTS_H

#include <stdint.h>

void shots_init(void);
void shots_update(void);
void shots_spawn(uint8_t x, uint8_t y);
void shots_render(void);

uint8_t shots_hit(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height
);

#endif
