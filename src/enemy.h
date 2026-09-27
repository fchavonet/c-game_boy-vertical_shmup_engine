#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

void enemy_init(void);
uint8_t enemy_spawn(uint8_t x, uint8_t y);
void enemy_update(void);
void enemy_render(void);

#endif
