#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RulesBridge.generated.h"

USTRUCT(BlueprintType)
struct FRollResult
{
    GENERATED_BODY()

    // Example fields — extend to match your rules engine's output
    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Rules")
    int32 Outcome = 0;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Rules")
    int32 DiceA = 0;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Rules")
    int32 DiceB = 0;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Rules")
    FString DebugInfo;
};

UCLASS()
class UMadCrapsRulesBridge : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Execute a roll on the authoritative rules engine (server). Seed is optional — server should control seeding in production.
    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static FRollResult ExecuteRoll(int32 Seed, const FString& BetConfig, FString& OutSignedToken);

    // Verify a signed roll token and return the authoritative result. Clients should call this to validate server-sent tokens.
    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static bool VerifySignedRoll(const FString& SignedToken, FRollResult& OutResult);
};
