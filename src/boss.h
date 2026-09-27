#ifndef BOSS_H
#define BOSS_H

#include <stdint.h>

typedef struct
{
    uint8_t start_hp;
    uint8_t phase_two_hp;

    uint8_t phase_one_speed;
    uint8_t phase_two_speed;

    uint8_t phase_one_shot_interval;
    uint8_t phase_two_shot_interval;
} BossDefinition;

void boss_init(void);
void boss_start(const BossDefinition *definition);

void boss_update(void);
void boss_render(void);

uint8_t boss_is_defeated(void);

uint8_t boss_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
