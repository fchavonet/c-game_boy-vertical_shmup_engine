#ifndef BOSS_H
#define BOSS_H

#include <stdint.h>

typedef enum
{
    BOSS_SHOT_NONE,
    BOSS_SHOT_STRAIGHT,
    BOSS_SHOT_DOUBLE,
    BOSS_SHOT_AIMED,
    BOSS_SHOT_SPREAD
} BossShotMode;

typedef struct
{
    uint8_t start_hp;
    uint8_t phase_two_hp;

    uint8_t phase_one_speed;
    uint8_t phase_two_speed;

    uint8_t phase_one_shot_interval;
    uint8_t phase_two_shot_interval;

    BossShotMode phase_one_shot_mode;
    BossShotMode phase_two_shot_mode;

    uint8_t phase_one_shot_speed; /* Sixteenths of a pixel */
    uint8_t phase_two_shot_speed;
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
