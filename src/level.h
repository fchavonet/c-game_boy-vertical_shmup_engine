#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

#include "level_data.h"

void level_init(const LevelDefinition *definition);
void level_update(void);

uint8_t level_is_complete(void);

#endif
