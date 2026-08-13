#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MadCrapsRulesWrapper.generated.h"

UENUM(BlueprintType)
enum class EMadCrapsBetType : uint8
{
	PassLine UMETA(DisplayName = "Pass Line"),
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
	Horn UMETA(DisplayName = "Horn"),
	Yo UMETA(DisplayName = "Yo"),
	AceDeuce UMETA(DisplayName = "Ace Deuce")
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsBet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	EMadCrapsBetType Type = EMadCrapsBetType::PassLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	double Amount = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	int32 Target = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	FString Tag;
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsRollResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	int32 Die1 = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	int32 Die2 = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MadCraps|Rules")
	int32 Total = 2;
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsPayout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	double Net = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	FString Description;
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsTableConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	double FieldTwoPayout = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	double FieldTwelvePayout = 3.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<int32, double> OddsPayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<int32, double> PlacePayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<int32, double> BuyPayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<int32, double> LayPayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<int32, double> HardwayPayout;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	double BuyCommissionPct = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Rules")
	TMap<FString, double> PropPayouts;
};

/**
 * Thin Blueprint-exposed wrapper around the native MadCraps rules engine.
 * The actual rules logic lives in the native C++ library under rules/src.
 */
UCLASS(BlueprintType)
class MADCRAPSRULES_API UMadCrapsRulesWrapper : public UObject
{
	GENERATED_BODY()

public:
	UMadCrapsRulesWrapper();

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	void SetTableConfig(const FMadCrapsTableConfig& TableConfig);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	bool SetTableConfigFromJSON(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	void Seed(int64 Seed);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	FMadCrapsRollResult RollDice();

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	TArray<FMadCrapsPayout> ResolveBets(const TArray<FMadCrapsBet>& Bets, const FMadCrapsRollResult& Roll, int32 CurrentPoint);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Rules")
	TArray<FMadCrapsPayout> ResolveRoll(int32 Die1, int32 Die2, int32 CurrentPoint, const TArray<FMadCrapsBet>& Bets);

private:
	void ApplyTableConfig(const FMadCrapsTableConfig& TableConfig);
};
