#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>

void score_init(void);
void score_add(uint16_t points);
uint32_t score_get(void);

#endif
