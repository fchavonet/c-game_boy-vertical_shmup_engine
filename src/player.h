#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void player_init(void);
void player_update(uint8_t buttons);
void player_check_collision(void);
void player_render(void);

uint8_t player_is_alive(void);
uint8_t player_is_destroying(void);
uint8_t player_get_lives(void);

uint8_t player_get_center_x(void);
uint8_t player_get_center_y(void);

#endif
