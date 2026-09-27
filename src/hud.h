#ifndef HUD_H
#define HUD_H

#include <stdint.h>

void hud_init(void);
void hud_render(uint8_t lives, uint32_t score);

#endif
