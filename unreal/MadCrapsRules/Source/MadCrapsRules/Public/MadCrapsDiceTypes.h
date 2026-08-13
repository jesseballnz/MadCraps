#pragma once

#include "CoreMinimal.h"
#include "MadCrapsDiceTypes.generated.h"

UENUM(BlueprintType)
enum class EMadCrapsDiceRollMode : uint8
{
	Animated UMETA(DisplayName = "Animated"),
	Physics UMETA(DisplayName = "Physics")
};

UENUM(BlueprintType)
enum class EMadCrapsDiceRollPhase : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Rolling UMETA(DisplayName = "Rolling"),
	AwaitingAuthority UMETA(DisplayName = "Awaiting Authority"),
	Settling UMETA(DisplayName = "Settling"),
	Settled UMETA(DisplayName = "Settled")
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsDiePose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FRotator Rotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct MADCRAPSRULES_API FMadCrapsAuthoritativeRoll
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	int32 Die1 = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	int32 Die2 = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	int32 CurrentPoint = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FString RollToken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FString Signature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	float RevealDelaySeconds = 1.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FMadCrapsDiePose Die1Pose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FMadCrapsDiePose Die2Pose;

	int32 Total() const
	{
		return Die1 + Die2;
	}

	bool IsValid() const
	{
		return Die1 >= 1 && Die1 <= 6 && Die2 >= 1 && Die2 <= 6;
	}
};
