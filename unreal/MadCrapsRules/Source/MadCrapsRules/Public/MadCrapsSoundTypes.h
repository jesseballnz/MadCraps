#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"

#include "MadCrapsSoundTypes.generated.h"

UENUM(BlueprintType)
enum class EMadCrapsSoundCueType : uint8
{
	DiceRollStart,
	DiceBounce,
	DiceSettle,
	ChipSingle,
	ChipStack,
	ChipSweep,
	AmbientLoop,
	DealerComingOut,
	DealerSevenOut,
	DealerPointMade,
	DealerNoMoreBets,
	DealerPlaceYourBets
};

USTRUCT(BlueprintType)
struct FMadCrapsSoundVariant
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	float PitchMin = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	float PitchMax = 1.0f;
};

USTRUCT(BlueprintType)
struct FMadCrapsSoundSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	EMadCrapsSoundCueType CueType = EMadCrapsSoundCueType::DiceRollStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	TArray<FMadCrapsSoundVariant> Variants;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	float CooldownSeconds = 0.0f;
};

UCLASS(BlueprintType)
class MADCRAPSRULES_API UMadCrapsSoundProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MadCraps|Audio")
	TArray<FMadCrapsSoundSlot> SoundSlots;
};
