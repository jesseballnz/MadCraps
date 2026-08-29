#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MadCrapsTypes.generated.h"

/**
 * EMadCrapsBetType
 * All supported craps bet types
 */
UENUM(BlueprintType)
enum class EMadCrapsBetType : uint8
{
    PassLine = 0 UMETA(DisplayName = "Pass Line"),
    DontPass UMETA(DisplayName = "Don't Pass"),
    Come UMETA(DisplayName = "Come"),
    DontCome UMETA(DisplayName = "Don't Come"),
    Odds UMETA(DisplayName = "Odds"),
    Field UMETA(DisplayName = "Field"),
    Place UMETA(DisplayName = "Place"),
    Buy UMETA(DisplayName = "Buy"),
    Lay UMETA(DisplayName = "Lay"),
    Hardway UMETA(DisplayName = "Hardway"),
    Big6 UMETA(DisplayName = "Big 6"),
    Big8 UMETA(DisplayName = "Big 8"),
    Any7 UMETA(DisplayName = "Any 7"),
    AnyCraps UMETA(DisplayName = "Any Craps"),
    Horn UMETA(DisplayName = "Horn")
};

/**
 * FMadCrapsRollResult
 * Result of a single dice roll
 */
USTRUCT(BlueprintType)
struct FMadCrapsRollResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Roll")
    int32 Die1 = 0;

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Roll")
    int32 Die2 = 0;

    int32 Total() const { return Die1 + Die2; }
    bool IsHardway(int32 Target) const { return (Die1 == Die2) && (Total() == Target); }
};

/**
 * FMadCrapsBet
 * Represents a single placed bet
 */
USTRUCT(BlueprintType)
struct FMadCrapsBet
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Bet")
    EMadCrapsBetType Type;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Bet")
    float Amount = 0.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Bet")
    int32 Target = 0;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Bet")
    FString Tag;
};

/**
 * FMadCrapsPayout
 * Result of a bet resolution
 */
USTRUCT(BlueprintType)
struct FMadCrapsPayout
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Payout")
    float Net = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Payout")
    FString Description;
};

/**
 * FMadCrapsTableConfig
 * Configurable table payouts and rules
 */
USTRUCT(BlueprintType)
struct FMadCrapsTableConfig
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config")
    FString TableName = FString(TEXT("Vegas Classic"));

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Field")
    float Field2Payout = 2.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Field")
    float Field12Payout = 3.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint4 = 2.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint5 = 1.5f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint6 = 1.2f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint8 = 1.2f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint9 = 1.5f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Odds")
    float OddsPoint10 = 2.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Buy")
    float BuyCommissionPercent = 0.05f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Hardway")
    float Hardway4 = 7.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Hardway")
    float Hardway6 = 9.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Hardway")
    float Hardway8 = 9.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Hardway")
    float Hardway10 = 7.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Props")
    float Any7Payout = 4.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Props")
    float AnyCrapsPayout = 7.0f;

    UPROPERTY(BlueprintReadWrite, Category = "MadCraps|Config|Props")
    float HornPayout = 30.0f;
};

/**
 * UMadCrapsGameState
 * Encapsulates the internal rules engine state for Blueprint access
 */
UCLASS(Blueprintable)
class MADCRAPSRULES_API UMadCrapsGameState : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|State")
    FMadCrapsTableConfig CurrentTableConfig;

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|State")
    int32 CurrentPoint = 0;

    UPROPERTY(BlueprintReadOnly, Category = "MadCraps|State")
    TArray<FMadCrapsBet> ActiveBets;

private:
    class FRulesEngineHandle* RulesEnginePtr = nullptr;

    friend class UMadCrapsRulesLibrary;
};
