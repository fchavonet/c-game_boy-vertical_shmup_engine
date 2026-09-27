#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

void level_init(void);
void level_update(void);

uint8_t level_is_complete(void);

#endif
