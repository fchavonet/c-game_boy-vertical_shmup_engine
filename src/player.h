#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void player_init(void);
void player_update(uint8_t buttons);
void player_render(void);

#endif
