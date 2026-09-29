#ifndef LEVEL_H
#define LEVEL_H

#include <stdint.h>

#include "level_data.h"

void level_init(const LevelDefinition *definition);
void level_update(void);

uint8_t level_is_complete(void);
/* Saturating counter of invalid spawn/wave events skipped this level. */
uint8_t level_get_invalid_event_count(void);

#endif
