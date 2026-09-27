#ifndef PAUSE_INDICATOR_H
#define PAUSE_INDICATOR_H

#include <stdint.h>

void pause_indicator_init(void);
void pause_indicator_render(uint8_t paused);

#endif
