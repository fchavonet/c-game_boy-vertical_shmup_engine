#ifndef HUD_H
#define HUD_H

#include <stdint.h>

#define HUD_TOP 132

void hud_init(void);
void hud_render(uint8_t lives, uint32_t score);
void hud_hide(void);

#endif
