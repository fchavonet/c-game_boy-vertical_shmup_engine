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

#define BOSS_SEQUENCE_MAX_ACTIONS 16u

typedef enum
{
    BOSS_MOVE_HOLD,
    BOSS_MOVE_SWEEP,
    BOSS_MOVE_LEFT,
    BOSS_MOVE_RIGHT,
    BOSS_MOVE_UP,
    BOSS_MOVE_DOWN
} BossMovement;

typedef struct
{
    uint16_t duration; /* Number of gameplay updates; must be nonzero. */
    BossMovement movement;
    uint8_t speed; /* Whole pixels per update, 0..4. */
    BossShotMode shot_mode;
    uint8_t first_shot_delay;
    uint8_t shot_interval;
    uint8_t shot_speed; /* Sixteenths of a pixel. */
    uint8_t salvos; /* 0 = repeat throughout action; otherwise attempt limit. */
} BossAction;

typedef struct
{
    const BossAction *actions;
    uint8_t action_count;
} BossSequence;

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
    /* Null or invalid sequences use the legacy phase parameters above. */
    const BossSequence *phase_one_sequence;
    const BossSequence *phase_two_sequence;
} BossDefinition;

void boss_init(void);
void boss_start(const BossDefinition *definition);

void boss_update(void);
void boss_render(void);

uint8_t boss_is_defeated(void);
/* Number of invalid non-null sequences rejected at boss_start(), 0..2. */
uint8_t boss_get_invalid_sequence_count(void);

uint8_t boss_touch(
    int16_t x,
    int16_t y,
    uint8_t width,
    uint8_t height);

#endif
