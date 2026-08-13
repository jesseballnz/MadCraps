#include "MadCrapsDieActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
FRotator MakeFaceRotation(const int32 FaceValue)
{
	switch (FaceValue)
	{
	case 1:
		return FRotator(0.0f, 0.0f, 0.0f);
	case 2:
		return FRotator(0.0f, 0.0f, -90.0f);
	case 3:
		return FRotator(-90.0f, 0.0f, 0.0f);
	case 4:
		return FRotator(90.0f, 0.0f, 0.0f);
	case 5:
		return FRotator(0.0f, 0.0f, 90.0f);
	case 6:
	default:
		return FRotator(180.0f, 0.0f, 0.0f);
	}
}
}

AMadCrapsDieActor::AMadCrapsDieActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DieMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DieMesh"));
	DieMesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")));
	DieMesh->SetSimulatePhysics(false);
	DieMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	DieMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DieMesh->SetRelativeScale3D(FVector(DieSize / 100.0f));
	SetRootComponent(DieMesh);
}

void AMadCrapsDieActor::BeginPlay()
{
	Super::BeginPlay();
	DieMesh->SetRelativeScale3D(FVector(DieSize / 100.0f));

	if (UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
	{
		if (UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this))
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), DieColor);
			DieMesh->SetMaterial(0, DynamicMaterial);
		}
	}
}

void AMadCrapsDieActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bRolling && ActiveRollMode == EMadCrapsDiceRollMode::Animated)
	{
		TickAnimatedRoll(DeltaSeconds);
	}
}

void AMadCrapsDieActor::BeginVisualRoll(const int32 RandomSeed, const EMadCrapsDiceRollMode RollMode, const FVector& Impulse, const FVector& TorqueImpulse)
{
	FRandomStream Stream(RandomSeed);

	ActiveRollMode = RollMode;
	bRolling = true;
	AnimatedElapsedSeconds = 0.0f;
	AnimatedStartLocation = GetActorLocation();
	AnimatedTravelDirection = FVector(Stream.FRandRange(-1.0f, 1.0f), Stream.FRandRange(-1.0f, 1.0f), 0.0f).GetSafeNormal();
	if (AnimatedTravelDirection.IsNearlyZero())
	{
		AnimatedTravelDirection = FVector::ForwardVector;
	}

	SpinPitchRate = Stream.FRandRange(540.0f, 1080.0f);
	SpinYawRate = Stream.FRandRange(720.0f, 1320.0f);
	SpinRollRate = Stream.FRandRange(540.0f, 1200.0f);

	if (RollMode == EMadCrapsDiceRollMode::Physics)
	{
		DieMesh->SetSimulatePhysics(true);
		DieMesh->WakeAllRigidBodies();
		DieMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		DieMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		DieMesh->AddImpulse(Impulse, NAME_None, true);
		DieMesh->AddAngularImpulseInDegrees(TorqueImpulse, NAME_None, true);
		return;
	}

	DieMesh->SetSimulatePhysics(false);
	SetActorRotation(FRotator(Stream.FRandRange(0.0f, 360.0f), Stream.FRandRange(0.0f, 360.0f), Stream.FRandRange(0.0f, 360.0f)));
}

void AMadCrapsDieActor::SnapToFace(const int32 FaceValue, const FMadCrapsDiePose& TargetPose, const bool bPreservePhysicsVelocity)
{
	CurrentFaceValue = FMath::Clamp(FaceValue, 1, 6);
	bRolling = false;

	if (!bPreservePhysicsVelocity)
	{
		StopPhysicsMotion();
	}

	FVector FinalLocation = TargetPose.Location.IsNearlyZero() ? GetActorLocation() : TargetPose.Location;
	FRotator FinalRotation = bEnableFaceOrientationSnap ? GetFaceRotation(CurrentFaceValue) : GetActorRotation();
	if (!TargetPose.Rotation.IsNearlyZero())
	{
		FinalRotation = TargetPose.Rotation;
	}

	SetActorLocationAndRotation(FinalLocation, FinalRotation, false, nullptr, ETeleportType::TeleportPhysics);
	OnDieSettled.Broadcast(CurrentFaceValue);
}

bool AMadCrapsDieActor::IsVisuallyRolling() const
{
	return bRolling;
}

int32 AMadCrapsDieActor::GetCurrentFaceValue() const
{
	return CurrentFaceValue;
}

FRotator AMadCrapsDieActor::GetFaceRotation(const int32 FaceValue) const
{
	return MakeFaceRotation(FaceValue);
}

void AMadCrapsDieActor::TickAnimatedRoll(const float DeltaSeconds)
{
	AnimatedElapsedSeconds += DeltaSeconds;
	const float Alpha = FMath::Clamp(AnimatedElapsedSeconds / FMath::Max(AnimatedRollDuration, KINDA_SMALL_NUMBER), 0.0f, 1.0f);

	const FVector HorizontalOffset = AnimatedTravelDirection * 48.0f * Alpha;
	const float ArcOffset = FMath::Sin(Alpha * PI) * AnimatedArcHeight;
	const FVector NewLocation = AnimatedStartLocation + HorizontalOffset + FVector(0.0f, 0.0f, ArcOffset);
	SetActorLocation(NewLocation);

	const FRotator DeltaRotation(SpinPitchRate * DeltaSeconds, SpinYawRate * DeltaSeconds, SpinRollRate * DeltaSeconds);
	AddActorLocalRotation(DeltaRotation);

	if (Alpha >= 1.0f)
	{
		bRolling = false;
	}
}

void AMadCrapsDieActor::StopPhysicsMotion()
{
	if (!DieMesh->IsSimulatingPhysics())
	{
		return;
	}

	DieMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	DieMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	DieMesh->SetSimulatePhysics(false);
}
