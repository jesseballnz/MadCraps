#include "CrapsCamera.h"
#include "CrapsTableActor.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ACrapsCamera::ACrapsCamera()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
    SetRootComponent(CameraRoot);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(CameraRoot);
    SpringArm->bDoCollisionTest        = false;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritPitch           = false;
    SpringArm->bInheritRoll            = false;
    SpringArm->bInheritYaw             = false;
    SpringArm->TargetArmLength         = DealerArmLength;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->FieldOfView = 72.f;

    PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
    PostProcess->SetupAttachment(CameraRoot);
    PostProcess->bUnbound = true;

    // Default PP presets
    // -- Dealer view: minimal DOF, wide scene --
    DealerViewPP.FocalDistance  = 300.f;
    DealerViewPP.Aperture       = 11.f;
    DealerViewPP.VignetteIntensity = 0.15f;
    DealerViewPP.ExposureBias   = 0.2f;

    // -- Player view: shallow DOF, intimate --
    PlayerViewPP.FocalDistance  = 120.f;
    PlayerViewPP.Aperture       = 2.4f;
    PlayerViewPP.VignetteIntensity = 0.35f;
    PlayerViewPP.ExposureBias   = 0.4f;
    PlayerViewPP.ChromaticAberrationIntensity = 0.8f;

    // -- Cinematic dolly: balanced --
    CinematicDollyPP.FocalDistance  = 220.f;
    CinematicDollyPP.Aperture       = 3.5f;
    CinematicDollyPP.VignetteIntensity = 0.30f;
    CinematicDollyPP.ExposureBias   = 0.1f;

    // -- Dice close-up: very shallow DOF --
    DiceCloseUpPP.FocalDistance  = 40.f;
    DiceCloseUpPP.Aperture       = 1.4f;
    DiceCloseUpPP.VignetteIntensity = 0.45f;
    DiceCloseUpPP.ExposureBias   = 0.6f;
    DiceCloseUpPP.ChromaticAberrationIntensity = 1.2f;

    // -- Side perspective: moderate --
    SidePerspectivePP.FocalDistance  = 180.f;
    SidePerspectivePP.Aperture       = 5.6f;
    SidePerspectivePP.VignetteIntensity = 0.2f;
}

void ACrapsCamera::BeginPlay()
{
    Super::BeginPlay();
    SnapToPreset(ActivePreset);
}

void ACrapsCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // -- Smooth transition --
    if (bTransitioning)
    {
        TransitionTimer += DeltaTime;
        const float RawAlpha  = FMath::Clamp(TransitionTimer / FMath::Max(TransitionDuration, 0.01f), 0.f, 1.f);
        const float Alpha     = FMath::Pow(RawAlpha, TransitionEasingExp);

        FVector  TargetLoc;
        FRotator TargetRot;
        float    TargetArmLen;
        float    TargetFOV;
        ComputePresetTransform(ActivePreset, TargetLoc, TargetRot, TargetArmLen, TargetFOV);

        SpringArm->SetWorldLocation(FMath::Lerp(TransitionFromLoc, TargetLoc, Alpha));
        SpringArm->SetWorldRotation(FMath::Lerp(TransitionFromRot, TargetRot, Alpha));
        SpringArm->TargetArmLength = FMath::Lerp(TransitionFromArmLen, TargetArmLen, Alpha);
        Camera->FieldOfView        = FMath::Lerp(TransitionFromFOV,    TargetFOV,    Alpha);

        const FCrapsPostProcessConfig PP = LerpPostProcess(
            GetPostProcessForPreset(PreTransitionPreset),
            GetPostProcessForPreset(ActivePreset),
            Alpha);
        ApplyPostProcess(PP);

        if (RawAlpha >= 1.f)
        {
            bTransitioning = false;
        }
    }

    // -- Cinematic dolly --
    if (bDollying)
    {
        DollyAngle += DollyOrbitSpeed * DeltaTime;
        if (DollyAngle >= 360.f) { DollyAngle -= 360.f; }

        FVector TableCenter = TableActor
            ? TableActor->GetActorLocation()
            : GetActorLocation();

        const float Rad = FMath::DegreesToRadians(DollyAngle);
        const FVector Offset(
            FMath::Cos(Rad) * DollyArmLength,
            FMath::Sin(Rad) * DollyArmLength,
            60.f);

        SpringArm->SetWorldLocation(TableCenter + Offset);
        const FRotator LookAt = (TableCenter - (TableCenter + Offset)).Rotation();
        SpringArm->SetWorldRotation(LookAt);
    }

    // -- Dice close-up hold --
    if (bHoldingCloseUp)
    {
        CloseUpHoldTimer += DeltaTime;
        if (CloseUpHoldTimer >= CloseUpHoldDuration)
        {
            bHoldingCloseUp = false;
            SetPreset(PreTransitionPreset); // return to previous view
        }
    }
}

void ACrapsCamera::SnapToPreset(ECrapsCameraPreset Preset)
{
    ActivePreset = Preset;
    bTransitioning = false;

    FVector  TargetLoc;
    FRotator TargetRot;
    float    TargetArmLen;
    float    TargetFOV;
    ComputePresetTransform(Preset, TargetLoc, TargetRot, TargetArmLen, TargetFOV);

    SpringArm->SetWorldLocation(TargetLoc);
    SpringArm->SetWorldRotation(TargetRot);
    SpringArm->TargetArmLength = TargetArmLen;
    Camera->FieldOfView        = TargetFOV;

    ApplyPostProcess(GetPostProcessForPreset(Preset));
}

void ACrapsCamera::SetPreset(ECrapsCameraPreset Preset)
{
    if (Preset == ActivePreset && !bTransitioning)
    {
        return;
    }

    PreTransitionPreset = ActivePreset;
    ActivePreset        = Preset;

    TransitionFromLoc    = SpringArm->GetComponentLocation();
    TransitionFromRot    = SpringArm->GetComponentRotation();
    TransitionFromArmLen = SpringArm->TargetArmLength;
    TransitionFromFOV    = Camera->FieldOfView;

    TransitionTimer  = 0.f;
    bTransitioning   = true;

    // Disable dolly when manually switching presets (unless to Cinematic)
    if (Preset != ECrapsCameraPreset::CinematicDolly)
    {
        bDollying = false;
    }
}

void ACrapsCamera::CutToDiceCloseUp(float HoldDuration)
{
    PreTransitionPreset    = ActivePreset;
    CloseUpHoldDuration    = HoldDuration;
    CloseUpHoldTimer       = 0.f;
    bHoldingCloseUp        = true;
    SnapToPreset(ECrapsCameraPreset::DiceCloseUp);
}

void ACrapsCamera::SetDollying(bool bEnabled)
{
    bDollying = bEnabled;
    if (bEnabled)
    {
        SetPreset(ECrapsCameraPreset::CinematicDolly);
    }
}

void ACrapsCamera::SetAsViewTarget()
{
    APlayerController* PC = GetWorld()
        ? GetWorld()->GetFirstPlayerController()
        : nullptr;
    if (PC)
    {
        PC->SetViewTarget(this);
    }
}

void ACrapsCamera::ComputePresetTransform(ECrapsCameraPreset Preset,
                                          FVector& OutLocation,
                                          FRotator& OutRotation,
                                          float& OutArmLength,
                                          float& OutFOV) const
{
    const FVector TableCenter = TableActor
        ? TableActor->GetActorLocation()
        : GetActorLocation();

    switch (Preset)
    {
    case ECrapsCameraPreset::DealerView:
        OutLocation   = TableCenter + FVector(0.f, 0.f, DealerArmLength);
        OutRotation   = FRotator(-90.f, 0.f, 0.f);
        OutArmLength  = DealerArmLength;
        OutFOV        = 78.f;
        break;

    case ECrapsCameraPreset::PlayerView:
        OutLocation   = TableCenter + FVector(0.f, -PlayerArmLength * 0.9f, 60.f);
        OutRotation   = FRotator(-18.f, 90.f, 0.f);
        OutArmLength  = PlayerArmLength;
        OutFOV        = 58.f;
        break;

    case ECrapsCameraPreset::CinematicDolly:
        OutLocation   = TableCenter + FVector(DollyArmLength, 0.f, 80.f);
        OutRotation   = FRotator(-18.f, 180.f, 0.f);
        OutArmLength  = DollyArmLength;
        OutFOV        = 65.f;
        break;

    case ECrapsCameraPreset::DiceCloseUp:
        // Focus on shooter-end dice landing zone
        OutLocation   = TableCenter + FVector(80.f, 0.f, 50.f);
        OutRotation   = FRotator(-55.f, 180.f, 0.f);
        OutArmLength  = DiceCloseUpArmLength;
        OutFOV        = 40.f;
        break;

    case ECrapsCameraPreset::SidePerspective:
        OutLocation   = TableCenter + FVector(-SideArmLength * 0.6f, SideArmLength * 0.8f, 70.f);
        OutRotation   = FRotator(-20.f, 140.f, 0.f);
        OutArmLength  = SideArmLength;
        OutFOV        = 62.f;
        break;

    default:
        OutLocation   = TableCenter + FVector(0.f, 0.f, DealerArmLength);
        OutRotation   = FRotator(-90.f, 0.f, 0.f);
        OutArmLength  = DealerArmLength;
        OutFOV        = 78.f;
        break;
    }
}

void ACrapsCamera::ApplyPostProcess(const FCrapsPostProcessConfig& Config)
{
    if (!PostProcess) { return; }

    FPostProcessSettings& PP = PostProcess->Settings;

    PP.bOverride_AutoExposureBias                   = true;
    PP.AutoExposureBias                             = Config.ExposureBias;

    PP.bOverride_VignetteIntensity                  = true;
    PP.VignetteIntensity                            = Config.VignetteIntensity;

    PP.bOverride_DepthOfFieldFstop                  = true;
    PP.DepthOfFieldFstop                            = Config.Aperture;

    PP.bOverride_DepthOfFieldFocalDistance          = true;
    PP.DepthOfFieldFocalDistance                    = Config.FocalDistance;

    PP.bOverride_SceneFringeIntensity               = Config.bChromaticAberration;
    PP.SceneFringeIntensity                         = Config.ChromaticAberrationIntensity;

    PP.bOverride_FilmShoulder                       = true;
    PP.FilmShoulder                                 = Config.ToneMapperShoulder;
}

FCrapsPostProcessConfig ACrapsCamera::LerpPostProcess(
    const FCrapsPostProcessConfig& A,
    const FCrapsPostProcessConfig& B,
    float Alpha)
{
    FCrapsPostProcessConfig Out;
    Out.ExposureBias          = FMath::Lerp(A.ExposureBias,          B.ExposureBias,          Alpha);
    Out.VignetteIntensity     = FMath::Lerp(A.VignetteIntensity,     B.VignetteIntensity,     Alpha);
    Out.Aperture              = FMath::Lerp(A.Aperture,              B.Aperture,              Alpha);
    Out.FocalDistance         = FMath::Lerp(A.FocalDistance,         B.FocalDistance,         Alpha);
    Out.ToneMapperShoulder    = FMath::Lerp(A.ToneMapperShoulder,    B.ToneMapperShoulder,    Alpha);
    Out.ChromaticAberrationIntensity =
        FMath::Lerp(A.ChromaticAberrationIntensity, B.ChromaticAberrationIntensity, Alpha);
    Out.bChromaticAberration  = (Alpha >= 0.5f) ? B.bChromaticAberration : A.bChromaticAberration;
    return Out;
}

const FCrapsPostProcessConfig& ACrapsCamera::GetPostProcessForPreset(ECrapsCameraPreset Preset) const
{
    switch (Preset)
    {
    case ECrapsCameraPreset::DealerView:      return DealerViewPP;
    case ECrapsCameraPreset::PlayerView:      return PlayerViewPP;
    case ECrapsCameraPreset::CinematicDolly:  return CinematicDollyPP;
    case ECrapsCameraPreset::DiceCloseUp:     return DiceCloseUpPP;
    case ECrapsCameraPreset::SidePerspective: return SidePerspectivePP;
    default:                                  return DealerViewPP;
    }
}
