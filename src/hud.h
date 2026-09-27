#ifndef HUD_H
#define HUD_H

#include <stdint.h>

#include "game_config.h"

#define HUD_TOP ((uint8_t)GAME_PLAYFIELD_HEIGHT)

void hud_init(void);
void hud_render(uint8_t lives, uint32_t score);
void hud_hide(void);

#endif
