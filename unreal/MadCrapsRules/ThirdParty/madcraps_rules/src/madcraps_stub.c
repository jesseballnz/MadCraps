#include "madcraps.h"
#include <stdint.h>

// Very small deterministic stub for the rules engine: uses the seed to return two dice.
int madcraps_roll(uint64_t seed, int* out_dice_a, int* out_dice_b)
{
    if (!out_dice_a || !out_dice_b) return -1;
    uint64_t s = seed;
    int a = (int)((s % 6) + 1);
    int b = (int)(((s / 7) % 6) + 1);
    *out_dice_a = a;
    *out_dice_b = b;
    return 0;
}
