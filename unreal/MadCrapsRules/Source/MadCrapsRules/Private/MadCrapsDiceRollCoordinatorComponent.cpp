#include "MadCrapsDiceRollCoordinatorComponent.h"

#include "MadCrapsDieActor.h"

UMadCrapsDiceRollCoordinatorComponent::UMadCrapsDiceRollCoordinatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMadCrapsDiceRollCoordinatorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMadCrapsDiceRollCoordinatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (RollPhase == EMadCrapsDiceRollPhase::Idle || RollPhase == EMadCrapsDiceRollPhase::Settled)
	{
		return;
	}

	VisualRollElapsed += DeltaTime;
	if (PendingRoll.IsSet() && CanSettleNow())
	{
		ApplyAuthoritativeRoll();
	}
}

void UMadCrapsDiceRollCoordinatorComponent::StartLocalRoll(const int32 RollId, const int32 RandomSeedOverride)
{
	ActiveRollId = RollId;
	ActiveSeed = RandomSeedOverride == INDEX_NONE ? FMath::Rand() : RandomSeedOverride;
	VisualRollElapsed = 0.0f;
	PendingRoll.Reset();
	RollPhase = EMadCrapsDiceRollPhase::Rolling;

	FRandomStream Stream(ActiveSeed);
	if (DieOneActor)
	{
		DieOneActor->BeginVisualRoll(ActiveSeed + 17, RollMode, BuildImpulseForDie(0, Stream), BuildTorqueForDie(Stream));
	}

	if (DieTwoActor)
	{
		DieTwoActor->BeginVisualRoll(ActiveSeed + 31, RollMode, BuildImpulseForDie(1, Stream), BuildTorqueForDie(Stream));
	}
}

bool UMadCrapsDiceRollCoordinatorComponent::SubmitAuthoritativeRoll(const FMadCrapsAuthoritativeRoll& Roll)
{
	if (!Roll.IsValid())
	{
		return false;
	}

	PendingRoll = Roll;
	RollPhase = CanSettleNow() ? EMadCrapsDiceRollPhase::Settling : EMadCrapsDiceRollPhase::AwaitingAuthority;
	if (CanSettleNow())
	{
		ApplyAuthoritativeRoll();
	}
	return true;
}

EMadCrapsDiceRollPhase UMadCrapsDiceRollCoordinatorComponent::GetRollPhase() const
{
	return RollPhase;
}

void UMadCrapsDiceRollCoordinatorComponent::ApplyAuthoritativeRoll()
{
	if (!PendingRoll.IsSet())
	{
		return;
	}

	const FMadCrapsAuthoritativeRoll Roll = PendingRoll.GetValue();
	if (DieOneActor)
	{
		DieOneActor->SnapToFace(Roll.Die1, Roll.Die1Pose);
	}

	if (DieTwoActor)
	{
		DieTwoActor->SnapToFace(Roll.Die2, Roll.Die2Pose);
	}

	PendingRoll.Reset();
	RollPhase = EMadCrapsDiceRollPhase::Settled;
	OnAuthoritativeRollSettled.Broadcast(Roll);
}

bool UMadCrapsDiceRollCoordinatorComponent::CanSettleNow() const
{
	return PendingRoll.IsSet() && VisualRollElapsed >= FMath::Max(PendingRoll->RevealDelaySeconds, MinimumVisualRollSeconds);
}

FVector UMadCrapsDiceRollCoordinatorComponent::BuildImpulseForDie(const int32 DieIndex, FRandomStream& Stream) const
{
	const float SideSign = DieIndex == 0 ? -1.0f : 1.0f;
	return BaseImpulse + FVector(Stream.FRandRange(-150.0f, 150.0f), SideSign * Stream.FRandRange(80.0f, 220.0f), Stream.FRandRange(0.0f, 120.0f));
}

FVector UMadCrapsDiceRollCoordinatorComponent::BuildTorqueForDie(FRandomStream& Stream) const
{
	return BaseTorqueImpulse + FVector(Stream.FRandRange(-600.0f, 600.0f), Stream.FRandRange(-600.0f, 600.0f), Stream.FRandRange(-600.0f, 600.0f));
}
