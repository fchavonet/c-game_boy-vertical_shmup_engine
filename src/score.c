#include "score.h"

#define SCORE_MAX 99999UL

static uint32_t current_score;

void score_init(void)
{
    current_score = 0;
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
}

uint32_t score_get(void)
{
    return current_score;
}
