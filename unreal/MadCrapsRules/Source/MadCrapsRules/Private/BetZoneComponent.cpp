#include "BetZoneComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UBetZoneComponent::UBetZoneComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBetZoneComponent::BeginPlay()
{
    Super::BeginPlay();
    ApplyStateMaterial();
}

void UBetZoneComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Flash animation (win/lose)
    if (bFlashing)
    {
        FlashTimer += DeltaTime;
        const float Alpha = FMath::Clamp(FlashTimer / FlashFadeDuration, 0.f, 1.f);
        const FLinearColor FlashColor = bFlashIsWin ? WinningColor : LosingColor;
        const FLinearColor FadeTarget = IdleColor;
        const FLinearColor Blended    = FLinearColor::LerpUsingHSV(FlashColor, FadeTarget, Alpha);

        if (HighlightDecal)
        {
            UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(
                HighlightDecal->GetDecalMaterial());
            if (MID)
            {
                MID->SetVectorParameterValue(TEXT("HighlightColor"), Blended);
            }
        }

        if (Alpha >= 1.f)
        {
            bFlashing = false;
            SetZoneState(EBetZoneState::Idle);
            SetComponentTickEnabled(false);
        }
        return;
    }

    // Pulsing point highlight
    if (CurrentState == EBetZoneState::CurrentPoint)
    {
        PulsePhase += DeltaTime * PointPulseRate * 2.f * PI;
        const float PulseValue = (FMath::Sin(PulsePhase) + 1.f) * 0.5f;
        const FLinearColor PulsedColor = PointColor * PulseValue;

        if (HighlightDecal)
        {
            UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(
                HighlightDecal->GetDecalMaterial());
            if (MID)
            {
                MID->SetVectorParameterValue(TEXT("HighlightColor"), PulsedColor);
            }
        }
    }
}

void UBetZoneComponent::SetZoneState(EBetZoneState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    ApplyStateMaterial();

    // Enable tick for animated states
    const bool bNeedsTick = (NewState == EBetZoneState::CurrentPoint);
    SetComponentTickEnabled(bNeedsTick);
}

void UBetZoneComponent::FlashResult(bool bWin)
{
    bFlashing     = true;
    bFlashIsWin   = bWin;
    FlashTimer    = 0.f;
    SetComponentTickEnabled(true);
    ApplyStateMaterial();
}

void UBetZoneComponent::AddBetAmount(float Delta)
{
    CurrentBetAmount = FMath::Max(0.f, CurrentBetAmount + Delta);
    if (CurrentBetAmount > 0.f && CurrentState == EBetZoneState::Idle)
    {
        SetZoneState(EBetZoneState::Active);
    }
    else if (CurrentBetAmount <= 0.f)
    {
        SetZoneState(EBetZoneState::Idle);
    }
}

void UBetZoneComponent::ClearBet()
{
    CurrentBetAmount = 0.f;
    if (CurrentState == EBetZoneState::Active)
    {
        SetZoneState(EBetZoneState::Idle);
    }
}

void UBetZoneComponent::ApplyStateMaterial()
{
    if (!HighlightDecal)
    {
        return;
    }

    UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(
        HighlightDecal->GetDecalMaterial());
    if (!MID)
    {
        // Create a dynamic instance from the base material
        UMaterialInterface* BaseMat = HighlightDecal->GetDecalMaterial();
        if (BaseMat)
        {
            MID = UMaterialInstanceDynamic::Create(BaseMat, HighlightDecal);
            HighlightDecal->SetDecalMaterial(MID);
        }
    }

    if (MID)
    {
        const FLinearColor StateColor = bFlashing
            ? (bFlashIsWin ? WinningColor : LosingColor)
            : GetColorForState(CurrentState);
        MID->SetVectorParameterValue(TEXT("HighlightColor"), StateColor);
    }
}

FLinearColor UBetZoneComponent::GetColorForState(EBetZoneState State) const
{
    switch (State)
    {
    case EBetZoneState::Idle:         return IdleColor;
    case EBetZoneState::Hovered:      return HoveredColor;
    case EBetZoneState::Active:       return ActiveColor;
    case EBetZoneState::Winning:      return WinningColor;
    case EBetZoneState::Losing:       return LosingColor;
    case EBetZoneState::CurrentPoint: return PointColor;
    default:                          return IdleColor;
    }
}
