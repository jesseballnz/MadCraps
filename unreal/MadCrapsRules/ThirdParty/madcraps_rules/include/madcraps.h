#ifndef MADCRAPS_H
#define MADCRAPS_H

#ifdef __cplusplus
extern "C" {
#endif

// Simple C API for the rules engine stub used for plugin integration.
// In production, replace with the real rules engine header.

// Execute a roll with a 64-bit seed. Outputs dice values (1-6). Returns 0 on success.
int madcraps_roll(uint64_t seed, int* out_dice_a, int* out_dice_b);

#ifdef __cplusplus
}
#endif

#endif // MADCRAPS_H
