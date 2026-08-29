#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CrapsTableTypes.h"
#include "BetZoneComponent.generated.h"

class UStaticMeshComponent;
class UDecalComponent;

/**
 * UBetZoneComponent
 *
 * Represents a single interactive betting zone on the craps table surface.
 * Each zone manages its own visual state (idle, hovered, active, winning,
 * losing) by driving material parameter collections on its mesh and decal
 * components.  The owning CrapsTableActor queries and drives these states
 * in response to game events.
 *
 * Blueprint usage:
 *   - Attach to the CrapsTableActor
 *   - Set ZoneType to identify which bet this zone represents
 *   - Bind to OnZoneClicked to receive player-interaction events
 */
UCLASS(ClassGroup = "MadCraps", meta = (BlueprintSpawnableComponent))
class MADCRAPSRULES_API UBetZoneComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UBetZoneComponent();

    // ------------------------------------------------------------------
    // Properties — editable per instance in the Blueprint editor
    // ------------------------------------------------------------------

    /** Which craps bet this zone represents. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone")
    ECrapsBetZone ZoneType = ECrapsBetZone::None;

    /**
     * Human-readable label painted on the felt.
     * Defaults are pre-filled from the ZoneType but can be overridden.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone")
    FText ZoneLabel;

    /** World-space half-extents of the clickable hit box (in cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone")
    FVector HitExtent = FVector(30.f, 15.f, 2.f);

    /** Colour multiplier applied to the felt decal when in Idle state. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor IdleColor = FLinearColor(1.f, 1.f, 1.f, 0.f); // transparent overlay

    /** Colour multiplier applied when the mouse/cursor hovers this zone. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor HoveredColor = FLinearColor(1.f, 1.f, 1.f, 0.35f);

    /** Colour multiplier when a bet is placed (active). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor ActiveColor = FLinearColor(0.4f, 0.9f, 1.f, 0.45f);

    /** Colour multiplier when this zone resolves as a win (golden glow). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor WinningColor = FLinearColor(1.f, 0.85f, 0.1f, 1.f);

    /** Colour multiplier when this zone resolves as a loss (red). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor LosingColor = FLinearColor(0.9f, 0.1f, 0.1f, 0.7f);

    /** Colour for the current-point indicator. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Materials")
    FLinearColor PointColor = FLinearColor(1.f, 0.6f, 0.f, 1.f);

    /** Duration (seconds) for the win/lose flash fade-out animation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Animation", meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float FlashFadeDuration = 1.2f;

    /** Pulse frequency (Hz) while in CurrentPoint state. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BetZone|Animation", meta = (ClampMin = "0.1", ClampMax = "4.0"))
    float PointPulseRate = 1.5f;

    // ------------------------------------------------------------------
    // Blueprint delegates
    // ------------------------------------------------------------------

    /** Fires when the player clicks/taps this zone. */
    UPROPERTY(BlueprintAssignable, Category = "BetZone|Events")
    FOnBetPlaced OnZoneClicked;

    // ------------------------------------------------------------------
    // Blueprint callable API
    // ------------------------------------------------------------------

    /** Set the visual state of this zone and update its material. */
    UFUNCTION(BlueprintCallable, Category = "BetZone")
    void SetZoneState(EBetZoneState NewState);

    /** Get the current visual state. */
    UFUNCTION(BlueprintPure, Category = "BetZone")
    EBetZoneState GetZoneState() const { return CurrentState; }

    /** Flash a win or lose animation then return to Idle. */
    UFUNCTION(BlueprintCallable, Category = "BetZone")
    void FlashResult(bool bWin);

    /** Returns true if any bet amount is currently staked on this zone. */
    UFUNCTION(BlueprintPure, Category = "BetZone")
    bool HasActiveBet() const { return CurrentBetAmount > 0.f; }

    /** Amount currently bet on this zone. */
    UFUNCTION(BlueprintPure, Category = "BetZone")
    float GetBetAmount() const { return CurrentBetAmount; }

    /** Add (positive) or remove (negative) chips from this zone. */
    UFUNCTION(BlueprintCallable, Category = "BetZone")
    void AddBetAmount(float Delta);

    /** Remove all chips from this zone. */
    UFUNCTION(BlueprintCallable, Category = "BetZone")
    void ClearBet();

    // ------------------------------------------------------------------
    // UActorComponent overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    /** Apply CurrentState's colour to the decal material dynamic instance. */
    void ApplyStateMaterial();

    /** Returns the colour corresponding to the given state. */
    FLinearColor GetColorForState(EBetZoneState State) const;

    EBetZoneState CurrentState = EBetZoneState::Idle;
    float         CurrentBetAmount = 0.f;

    // Runtime animation tracking
    float FlashTimer     = 0.f;
    bool  bFlashing      = false;
    bool  bFlashIsWin    = false;
    float PulsePhase     = 0.f;

    /** Optional decal component for per-zone overlay highlight (set in BP). */
    UPROPERTY(BlueprintReadWrite, Category = "BetZone|Components")
    UDecalComponent* HighlightDecal = nullptr;
};
