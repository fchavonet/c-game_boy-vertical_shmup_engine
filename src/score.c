#include "score.h"
#include "player.h"

#define SCORE_MAX 99999UL
#define SCORE_EXTRA_LIFE_INTERVAL 1000UL

#if SCORE_EXTRA_LIFE_INTERVAL == 0
#error SCORE_EXTRA_LIFE_INTERVAL must be greater than zero
#endif

static uint32_t current_score;
static uint32_t next_extra_life_score;

void score_init(void)
{
    current_score = 0;
    next_extra_life_score = SCORE_EXTRA_LIFE_INTERVAL;
}

void score_add(uint16_t points)
{
    if (points > SCORE_MAX - current_score)
    {
        current_score = SCORE_MAX;
    }
    else
    {
        current_score += points;
    }

    /*
     * Consume every crossed milestone, including when
     * the player already has the maximum number of lives.
     */
    while (current_score >= next_extra_life_score)
    {
        player_add_life();

        next_extra_life_score += SCORE_EXTRA_LIFE_INTERVAL;
    }
}

uint32_t score_get(void)
{
    return current_score;
}
