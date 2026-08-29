#include "DiceActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// ---------------------------------------------------------------------------
// Face orientation lookup
// Standard Western die orientation (opposite faces sum to 7)
// ---------------------------------------------------------------------------
const FRotator ADiceActor::FaceRotations[6] =
{
    FRotator(  0.f,   0.f, 0.f),   // Face 1 up
    FRotator(  0.f,  90.f, 0.f),   // Face 2 up
    FRotator(-90.f,   0.f, 0.f),   // Face 3 up
    FRotator( 90.f,   0.f, 0.f),   // Face 4 up
    FRotator(  0.f, -90.f, 0.f),   // Face 5 up
    FRotator(180.f,   0.f, 0.f),   // Face 6 up
};

ADiceActor::ADiceActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
    RootComponent = DiceMesh;

    // Casino dice are ~19 mm cubed in real life — scaled to ~2 cm UU here.
    DiceMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    DiceMesh->bCastDynamicShadow = true;
    DiceMesh->SetGenerateOverlapEvents(false);
}

void ADiceActor::BeginPlay()
{
    Super::BeginPlay();
    InitMaterial();
    // Start face-1-up as default
    SnapToFaces(1, 0);
}

void ADiceActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // -- Roll tumble animation --
    if (bRolling)
    {
        RollAnimTimer += DeltaTime;
        const float Alpha = FMath::Clamp(RollAnimTimer / FMath::Max(RollAnimDuration, 0.01f), 0.f, 1.f);

        // Ease-out spin: fast at start, decelerates toward target
        const float EaseOut = 1.f - FMath::Pow(1.f - Alpha, 3.f);
        const FRotator TargetRot = (PendingFace >= 1 && PendingFace <= 6)
            ? FaceRotations[PendingFace - 1]
            : FRotator::ZeroRotator;

        // Add turbulent spin while animating
        const float TurbulenceFade = 1.f - EaseOut;
        const FRotator Turbulence(
            RollSpinSpeed.Pitch * TurbulenceFade * DeltaTime,
            RollSpinSpeed.Yaw   * TurbulenceFade * DeltaTime,
            RollSpinSpeed.Roll  * TurbulenceFade * DeltaTime);

        FRotator Current = DiceMesh->GetRelativeRotation() + Turbulence;
        DiceMesh->SetRelativeRotation(FMath::Lerp(Current, TargetRot, EaseOut * 0.15f));

        if (Alpha >= 1.f)
        {
            bRolling = false;
            DiceMesh->SetRelativeRotation(TargetRot);
            CurrentFace = PendingFace;
            SetActorTickEnabled(false);
            HighlightTopFace();
        }
        return;
    }

    // -- Face highlight pulse --
    if (bHighlighting)
    {
        HighlightTimer += DeltaTime;
        const float Alpha = FMath::Clamp(HighlightTimer / FMath::Max(FaceHighlightDuration, 0.01f), 0.f, 1.f);
        const float PulseVal = FMath::Sin(Alpha * PI); // arc 0→1→0
        if (DiceMID)
        {
            DiceMID->SetScalarParameterValue(TEXT("FaceEmissive"), PulseVal * 5.f);
        }
        if (Alpha >= 1.f)
        {
            bHighlighting = false;
            if (DiceMID) DiceMID->SetScalarParameterValue(TEXT("FaceEmissive"), 0.f);
            SetActorTickEnabled(false);
        }
    }
}

// ---------------------------------------------------------------------------
// API
// ---------------------------------------------------------------------------

void ADiceActor::SnapToFaces(int32 FaceUp, int32 /*Unused*/)
{
    FaceUp = FMath::Clamp(FaceUp, 1, 6);
    CurrentFace = FaceUp;
    DiceMesh->SetRelativeRotation(FaceRotations[FaceUp - 1]);

    UE_LOG(LogTemp, Log, TEXT("ADiceActor::SnapToFaces — face %d up"), FaceUp);
}

void ADiceActor::PlayRollAnimation()
{
    bRolling      = true;
    RollAnimTimer = 0.f;
    RollStartRotation = DiceMesh->GetRelativeRotation();

    // Random tumble axis and speed
    RollSpinSpeed = FRotator(
        FMath::RandRange(300.f, 720.f),
        FMath::RandRange(300.f, 720.f),
        FMath::RandRange(100.f, 360.f));

    SetActorTickEnabled(true);
    UE_LOG(LogTemp, Log, TEXT("ADiceActor::PlayRollAnimation — rolling toward face %d"), PendingFace);
}

void ADiceActor::SetPendingFace(int32 Face)
{
    PendingFace = FMath::Clamp(Face, 1, 6);
}

void ADiceActor::HighlightTopFace()
{
    bHighlighting  = true;
    HighlightTimer = 0.f;
    SetActorTickEnabled(true);
}

// ---------------------------------------------------------------------------
// Private
// ---------------------------------------------------------------------------

void ADiceActor::InitMaterial()
{
    if (DiceMesh && DiceMesh->GetMaterial(0))
    {
        DiceMID = UMaterialInstanceDynamic::Create(DiceMesh->GetMaterial(0), DiceMesh);
        DiceMesh->SetMaterial(0, DiceMID);
    }

    if (DiceMID)
    {
        DiceMID->SetVectorParameterValue(TEXT("DiceBodyColor"), DiceBodyColor);
        DiceMID->SetVectorParameterValue(TEXT("PipColor"),      PipColor);
        DiceMID->SetScalarParameterValue(TEXT("Roughness"),     DiceRoughness);
        DiceMID->SetScalarParameterValue(TEXT("FaceEmissive"),  0.f);
    }
}
