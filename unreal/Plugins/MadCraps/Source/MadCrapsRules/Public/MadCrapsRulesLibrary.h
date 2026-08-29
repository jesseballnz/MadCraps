#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MadCrapsTypes.h"
#include "MadCrapsRulesLibrary.generated.h"

/**
 * MadCrapsRulesLibrary
 * Blueprint-accessible C++ wrapper around the core rules engine.
 */
UCLASS()
class MADCRAPSRULES_API UMadCrapsRulesLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static UMadCrapsGameState* CreateGameState();

    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static void SetTableConfig(UMadCrapsGameState* GameState, const FMadCrapsTableConfig& Config);

    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static FMadCrapsRollResult RollDice(UMadCrapsGameState* GameState);

    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static TArray<FMadCrapsPayout> ResolveBetsOnRoll(
        UMadCrapsGameState* GameState,
        const TArray<FMadCrapsBet>& Bets,
        const FMadCrapsRollResult& Roll,
        int32 CurrentPoint
    );

    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static float CalculateHouseEdge(UMadCrapsGameState* GameState, EMadCrapsBetType BetType, int32 Trials = 100000);

    UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
    static void SeedRNG(UMadCrapsGameState* GameState, int32 Seed);
};
