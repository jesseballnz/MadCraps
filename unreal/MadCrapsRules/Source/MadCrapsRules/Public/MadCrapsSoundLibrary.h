#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MadCrapsSoundTypes.h"

#include "MadCrapsSoundLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMadCrapsResolvedSound
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Audio")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Audio")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "MadCraps|Audio")
	float PitchMultiplier = 1.0f;
};

UCLASS()
class MADCRAPSRULES_API UMadCrapsSoundLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MadCraps|Audio")
	static bool ResolveSoundCue(const UMadCrapsSoundProfile* Profile, EMadCrapsSoundCueType CueType, FMadCrapsResolvedSound& ResolvedSound);
};
