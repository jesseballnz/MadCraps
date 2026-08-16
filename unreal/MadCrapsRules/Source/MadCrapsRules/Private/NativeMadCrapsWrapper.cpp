// C wrapper for the madcraps_rules C API so the UE module can call it easily.
#include "CoreMinimal.h"
#include "madcraps.h"

extern "C" int NativeMadCrapsRoll(uint64_t seed, int* out_a, int* out_b)
{
    return madcraps_roll(seed, out_a, out_b);
}
