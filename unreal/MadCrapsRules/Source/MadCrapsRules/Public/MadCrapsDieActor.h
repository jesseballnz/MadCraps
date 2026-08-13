#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MadCrapsDiceTypes.h"
#include "MadCrapsDieActor.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMadCrapsDieSettledSignature, int32, FaceValue);

UCLASS(Blueprintable)
class MADCRAPSRULES_API AMadCrapsDieActor : public AActor
{
	GENERATED_BODY()

public:
	AMadCrapsDieActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	void BeginVisualRoll(int32 RandomSeed, EMadCrapsDiceRollMode RollMode, const FVector& Impulse, const FVector& TorqueImpulse);

	UFUNCTION(BlueprintCallable, Category = "MadCraps|Dice")
	void SnapToFace(int32 FaceValue, const FMadCrapsDiePose& TargetPose, bool bPreservePhysicsVelocity = false);

	UFUNCTION(BlueprintPure, Category = "MadCraps|Dice")
	bool IsVisuallyRolling() const;

	UFUNCTION(BlueprintPure, Category = "MadCraps|Dice")
	int32 GetCurrentFaceValue() const;

	UPROPERTY(BlueprintAssignable, Category = "MadCraps|Dice")
	FMadCrapsDieSettledSignature OnDieSettled;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MadCraps|Dice")
	TObjectPtr<UStaticMeshComponent> DieMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	float DieSize = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	FLinearColor DieColor = FLinearColor(0.92f, 0.92f, 0.92f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	float AnimatedRollDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	float AnimatedArcHeight = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MadCraps|Dice")
	bool bEnableFaceOrientationSnap = true;

private:
	FRotator GetFaceRotation(int32 FaceValue) const;
	void TickAnimatedRoll(float DeltaSeconds);
	void StopPhysicsMotion();

	EMadCrapsDiceRollMode ActiveRollMode = EMadCrapsDiceRollMode::Animated;
	FVector AnimatedStartLocation = FVector::ZeroVector;
	FVector AnimatedTravelDirection = FVector::ForwardVector;
	float AnimatedElapsedSeconds = 0.0f;
	float SpinPitchRate = 720.0f;
	float SpinYawRate = 960.0f;
	float SpinRollRate = 840.0f;
	bool bRolling = false;
	int32 CurrentFaceValue = 1;
};
