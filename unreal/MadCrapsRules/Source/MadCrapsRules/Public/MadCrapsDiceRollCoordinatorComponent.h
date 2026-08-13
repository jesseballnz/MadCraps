#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MadCrapsDiceTypes.h"
#include "MadCrapsDiceRollCoordinatorComponent.generated.h"

class AMadCrapsDieActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMadCrapsAuthoritativeRollResolvedSignature, const FMadCrapsAuthoritativeRoll&, Roll);

UCLASS(ClassGroup=(MadCraps), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class MADCRAPSRULES_API UMadCrapsDiceRollCoordinatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMadCrapsDiceRollCoordinatorComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	void StartLocalRoll(int32 RollId, int32 RandomSeedOverride = INDEX_NONE);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	bool SubmitAuthoritativeRoll(const FMadCrapsAuthoritativeRoll& Roll);

	UFUNCTION(BlueprintPure, Category = "MadCraps|Dice")
	EMadCrapsDiceRollPhase GetRollPhase() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	TObjectPtr<AMadCrapsDieActor> DieOneActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	TObjectPtr<AMadCrapsDieActor> DieTwoActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	EMadCrapsDiceRollMode RollMode = EMadCrapsDiceRollMode::Animated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	float MinimumVisualRollSeconds = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FVector BaseImpulse = FVector(900.0f, 0.0f, 300.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FVector BaseTorqueImpulse = FVector(1800.0f, 2000.0f, 1600.0f);

	UPROPERTY(BlueprintAssignable, Category = "MadCraps|Dice")
	FMadCrapsAuthoritativeRollResolvedSignature OnAuthoritativeRollSettled;

private:
	void ApplyAuthoritativeRoll();
	bool CanSettleNow() const;
	FVector BuildImpulseForDie(int32 DieIndex, FRandomStream& Stream) const;
	FVector BuildTorqueForDie(FRandomStream& Stream) const;

	EMadCrapsDiceRollPhase RollPhase = EMadCrapsDiceRollPhase::Idle;
	TOptional<FMadCrapsAuthoritativeRoll> PendingRoll;
	float VisualRollElapsed = 0.0f;
	int32 ActiveRollId = INDEX_NONE;
	int32 ActiveSeed = 0;
};
