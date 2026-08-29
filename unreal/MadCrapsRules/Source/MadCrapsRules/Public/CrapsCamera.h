#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "CrapsTableTypes.h"
#include "CrapsCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPostProcessComponent;
class ACrapsTableActor;

/**
 * ACrapsCamera
 *
 * Multi-preset camera rig for the craps table scene.  Provides:
 *
 *   DealerView      — Overhead/isometric view, wide FOV, minimal DOF
 *   PlayerView      — Low-angle, intimate first-person-like, shallow DOF
 *   CinematicDolly  — Sweeping arc around the table with animated position
 *   DiceCloseUp     — Macro focus on the dice landing zone
 *   SidePerspective — 45° side angle showing the full table breadth
 *
 * Transitions between presets use a smooth lerp over TransitionDuration
 * seconds.  Post-process (DOF, vignette, exposure) is driven per preset
 * through a UPostProcessComponent whose settings are blended in code.
 *
 * -------------------------------------------------------------------------
 * Usage in Blueprint:
 *   1. Place ACrapsCamera in the level, assign TableActor reference.
 *   2. Call SetPreset(ECrapsCameraPreset) to switch views.
 *   3. Call SetDollying(true) to start the automated cinematic sweep.
 *   4. Bind the table manager's OnRollResolved event to
 *      CutToDiceCloseUp() for dramatic roll reveals.
 * -------------------------------------------------------------------------
 */
UCLASS(BlueprintType, Blueprintable)
class MADCRAPSRULES_API ACrapsCamera : public AActor
{
    GENERATED_BODY()

public:
    ACrapsCamera();

    // ------------------------------------------------------------------
    // Components
    // ------------------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
    USceneComponent* CameraRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Components")
    UPostProcessComponent* PostProcess;

    // ------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------

    /** The craps table actor this camera orbits around. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Config")
    ACrapsTableActor* TableActor;

    /** Time (s) to smoothly interpolate between camera presets. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Config",
              meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float TransitionDuration = 1.0f;

    /** Easing exponent for camera transitions (1=linear, 2=quadratic, 3=cubic). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Config",
              meta = (ClampMin = "1.0", ClampMax = "4.0"))
    float TransitionEasingExp = 2.f;

    /** Speed of the cinematic dolly orbit (degrees per second). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Config",
              meta = (ClampMin = "1.0", ClampMax = "90.0"))
    float DollyOrbitSpeed = 12.f;

    /** Arm length for the dealer (overhead) view (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
    float DealerArmLength = 420.f;

    /** Arm length for the player (intimate low-angle) view (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
    float PlayerArmLength = 180.f;

    /** Arm length for the dice close-up macro view (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
    float DiceCloseUpArmLength = 55.f;

    /** Arm length for the cinematic dolly view (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
    float DollyArmLength = 340.f;

    /** Arm length for the side perspective view (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Presets")
    float SideArmLength = 300.f;

    // ------------------------------------------------------------------
    // Post-process per preset
    // ------------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    FCrapsPostProcessConfig DealerViewPP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    FCrapsPostProcessConfig PlayerViewPP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    FCrapsPostProcessConfig CinematicDollyPP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    FCrapsPostProcessConfig DiceCloseUpPP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    FCrapsPostProcessConfig SidePerspectivePP;

    // ------------------------------------------------------------------
    // Blueprint callable API
    // ------------------------------------------------------------------

    /** Immediately (no transition) snap to a camera preset. */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SnapToPreset(ECrapsCameraPreset Preset);

    /**
     * Smoothly transition to a camera preset over TransitionDuration.
     * If already transitioning, the in-flight transition is interrupted.
     */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetPreset(ECrapsCameraPreset Preset);

    /** Returns the currently active camera preset. */
    UFUNCTION(BlueprintPure, Category = "Camera")
    ECrapsCameraPreset GetCurrentPreset() const { return ActivePreset; }

    /**
     * Cut to the dice close-up view, then after HoldDuration seconds
     * automatically return to the previous view.
     */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void CutToDiceCloseUp(float HoldDuration = 3.f);

    /** Start/stop the automated cinematic dolly orbit. */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetDollying(bool bEnabled);

    /** Whether the cinematic dolly is currently active. */
    UFUNCTION(BlueprintPure, Category = "Camera")
    bool IsDollying() const { return bDollying; }

    /** Activate this camera as the view target for the local player. */
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetAsViewTarget();

    // ------------------------------------------------------------------
    // AActor overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    /** Compute the desired world transform for a given preset. */
    void ComputePresetTransform(ECrapsCameraPreset Preset,
                                FVector& OutLocation,
                                FRotator& OutRotation,
                                float& OutArmLength,
                                float& OutFOV) const;

    /** Apply a post-process config to the UPostProcessComponent. */
    void ApplyPostProcess(const FCrapsPostProcessConfig& Config);

    /** Lerp between two post-process configs. */
    static FCrapsPostProcessConfig LerpPostProcess(
        const FCrapsPostProcessConfig& A,
        const FCrapsPostProcessConfig& B,
        float Alpha);

    /** Returns the PP config for a given preset. */
    const FCrapsPostProcessConfig& GetPostProcessForPreset(ECrapsCameraPreset Preset) const;

    ECrapsCameraPreset ActivePreset    = ECrapsCameraPreset::DealerView;
    ECrapsCameraPreset PreTransitionPreset = ECrapsCameraPreset::DealerView;

    // Transition state
    bool  bTransitioning    = false;
    float TransitionTimer   = 0.f;
    FVector    TransitionFromLoc;
    FRotator   TransitionFromRot;
    float      TransitionFromArmLen = 0.f;
    float      TransitionFromFOV    = 90.f;

    // Dolly state
    bool  bDollying       = false;
    float DollyAngle      = 0.f;

    // Dice close-up hold
    bool  bHoldingCloseUp    = false;
    float CloseUpHoldTimer   = 0.f;
    float CloseUpHoldDuration = 3.f;
};
