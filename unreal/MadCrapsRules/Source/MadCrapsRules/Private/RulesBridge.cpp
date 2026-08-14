#include "RulesBridge.h"
#include "Misc/DefaultValueHelper.h"
#include "Misc/DateTime.h"

FRollResult UMadCrapsRulesBridge::ExecuteRoll(int32 Seed, const FString& BetConfig, FString& OutSignedToken)
{
    FRollResult Result;

    // TODO: Replace with actual call into the native rules engine.
    // If you're linking a native library, this is where you'd call the C API.
    // For now we provide a deterministic placeholder for testing.

    int32 s = Seed;
    if (s == 0)
    {
        s = (int32)FDateTime::UtcNow().ToUnixTimestamp();
    }

    // Simple pseudo-roll for prototype
    Result.DiceA = (s % 6) + 1;
    Result.DiceB = ((s / 7) % 6) + 1;
    Result.Outcome = Result.DiceA + Result.DiceB;
    Result.DebugInfo = FString::Printf(TEXT("Prototype roll with seed=%d"), s);

    // TODO: Call server signing routine and set OutSignedToken to the signed blob
    OutSignedToken = FString::Printf(TEXT("PROTOTYPE_TOKEN_%d"), s);

    return Result;
}

bool UMadCrapsRulesBridge::VerifySignedRoll(const FString& SignedToken, FRollResult& OutResult)
{
    // TODO: Implement signature verification logic (e.g., Ed25519 or ECDSA) using bundled public key.
    // For now, accept prototype tokens and parse seed.
    if (SignedToken.StartsWith(TEXT("PROTOTYPE_TOKEN_")))
    {
        FString SeedStr = SignedToken.RightChop(16);
        int32 Seed = FCString::Atoi(*SeedStr);
        int32 s = Seed;
        OutResult.DiceA = (s % 6) + 1;
        OutResult.DiceB = ((s / 7) % 6) + 1;
        OutResult.Outcome = OutResult.DiceA + OutResult.DiceB;
        OutResult.DebugInfo = TEXT("Prototype verified token");
        return true;
    }

    return false;
}
