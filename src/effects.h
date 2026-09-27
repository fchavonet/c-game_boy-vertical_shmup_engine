#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdint.h>

void effects_init(void);
void effects_update(void);
void effects_render(void);

void effects_spawn_explosion(int16_t x, int16_t y);

#endif
