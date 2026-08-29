#include "ChipActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AChipActor::AChipActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    ChipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChipMesh"));
    RootComponent = ChipMesh;

    // Recommended: assign SM_Chip in the Blueprint and enable Nanite.
    // Mesh scale: diameter ~3.5 cm, thickness ~0.32 cm.
    ChipMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    ChipMesh->bCastDynamicShadow = true;
    ChipMesh->bReceivesDecals    = false; // chips don't need decal projection
}

void AChipActor::BeginPlay()
{
    Super::BeginPlay();
    InitMaterial();
}

void AChipActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // -- Win animation --
    if (bPlayingWinAnim)
    {
        AnimTimer += DeltaTime;
        const float Phase = AnimTimer * WinPulseRate * 2.f * PI;
        const float PulseAlpha = (FMath::Sin(Phase) + 1.f) * 0.5f;

        if (ChipMID)
        {
            ChipMID->SetScalarParameterValue(TEXT("EmissiveIntensity"), PulseAlpha * 8.f);
        }

        // Scale bounce
        const float ScaleBoost = 1.f + FMath::Sin(Phase * 0.5f) * 0.07f;
        SetActorScale3D(FVector(ScaleBoost, ScaleBoost, ScaleBoost));

        if (AnimTimer >= WinPulseDuration)
        {
            bPlayingWinAnim = false;
            SetActorScale3D(FVector::OneVector);
            if (ChipMID) ChipMID->SetScalarParameterValue(TEXT("EmissiveIntensity"), 0.f);
            SetActorTickEnabled(false);
        }
    }

    // -- Lose animation --
    if (bPlayingLoseAnim)
    {
        AnimTimer += DeltaTime;
        const float FadeFraction = FMath::Clamp(AnimTimer / 0.4f, 0.f, 1.f);
        if (ChipMID)
        {
            const FLinearColor LoseRed(0.9f, 0.05f, 0.05f, 1.f);
            ChipMID->SetVectorParameterValue(TEXT("BaseColorTint"),
                FLinearColor::LerpUsingHSV(GetDenominationColor(), LoseRed, FadeFraction));
        }

        if (AnimTimer >= 0.5f)
        {
            bPlayingLoseAnim = false;
            Destroy(); // chip is removed from table
        }
    }

    // -- Move animation --
    if (bMoving)
    {
        MoveTimer += DeltaTime;
        const float Alpha = FMath::Clamp(MoveTimer / FMath::Max(MoveDuration, 0.01f), 0.f, 1.f);
        const float EasedAlpha = Alpha * Alpha * (3.f - 2.f * Alpha); // smoothstep
        SetActorLocation(FMath::Lerp(MoveStartLocation, MoveTargetLocation, EasedAlpha));

        if (Alpha >= 1.f)
        {
            bMoving = false;
            if (!bPlayingWinAnim && !bPlayingLoseAnim)
            {
                SetActorTickEnabled(false);
            }
        }
    }
}

void AChipActor::PlayWinAnimation()
{
    bPlayingWinAnim = true;
    bPlayingLoseAnim = false;
    AnimTimer = 0.f;
    SetActorTickEnabled(true);
}

void AChipActor::PlayLoseAnimation()
{
    bPlayingLoseAnim = true;
    bPlayingWinAnim  = false;
    AnimTimer = 0.f;
    SetActorTickEnabled(true);
}

void AChipActor::MoveTo(FVector TargetLocation, float Duration)
{
    MoveStartLocation  = GetActorLocation();
    MoveTargetLocation = TargetLocation;
    MoveDuration       = FMath::Max(Duration, 0.05f);
    MoveTimer          = 0.f;
    bMoving            = true;
    SetActorTickEnabled(true);
}

FLinearColor AChipActor::GetDenominationColor() const
{
    if (bOverrideColor)
    {
        return CustomColor;
    }

    // US casino colour conventions
    if (Denomination <=   1.f) return FLinearColor(0.85f, 0.85f, 0.85f, 1.f); // white/grey
    if (Denomination <=   5.f) return FLinearColor(0.75f, 0.05f, 0.05f, 1.f); // red
    if (Denomination <=  25.f) return FLinearColor(0.05f, 0.50f, 0.15f, 1.f); // green
    if (Denomination <= 100.f) return FLinearColor(0.05f, 0.05f, 0.05f, 1.f); // black
    if (Denomination <= 500.f) return FLinearColor(0.45f, 0.05f, 0.55f, 1.f); // purple
    return FLinearColor(0.90f, 0.55f, 0.05f, 1.f);                             // orange/yellow
}

void AChipActor::InitMaterial()
{
    if (ChipMesh && ChipMesh->GetMaterial(0))
    {
        ChipMID = UMaterialInstanceDynamic::Create(ChipMesh->GetMaterial(0), ChipMesh);
        ChipMesh->SetMaterial(0, ChipMID);
    }

    if (ChipMID)
    {
        ChipMID->SetVectorParameterValue(TEXT("BaseColorTint"),  GetDenominationColor());
        ChipMID->SetVectorParameterValue(TEXT("EdgeColor"),      EdgeColor);
        ChipMID->SetScalarParameterValue(TEXT("Roughness"),      ChipRoughness);
        ChipMID->SetScalarParameterValue(TEXT("EmissiveIntensity"), 0.f);
    }
}
