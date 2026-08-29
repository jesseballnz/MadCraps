#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrapsTableTypes.h"
#include "DiceActor.h"
#include "ChipActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * AChipActor
 *
 * A stackable, physically simulated casino chip.
 *
 * Each chip is a thin cylinder with beveled edges, rendered using PBR
 * materials that support:
 *   - Base colour (tinted per denomination)
 *   - Metallic edge insert (low metallic, high specular)
 *   - Roughness variation (worn clay feel)
 *   - Normal map for edge details and embossed denomination markings
 *   - Optional emissive pulse when selected or winning
 *
 * Chips are spawned and managed by ACrapsTableManager.  They can be
 * stacked by positioning them along their Z axis, and can be animated
 * to move between bet zones and the player tray.
 *
 * -------------------------------------------------------------------------
 * Mesh slot:
 *   SM_Chip — assign a chip cylinder in the Blueprint defaults.
 *             Recommended: ~3.5 cm diameter, ~0.32 cm thick, 32-sided
 *             polygon with chamfered top/bottom edges (Nanite-enabled).
 *
 * Material slot:
 *   MI_Chip — assign M_ChipMaterial instance.
 * -------------------------------------------------------------------------
 */
UCLASS(BlueprintType, Blueprintable)
class MADCRAPSRULES_API AChipActor : public AActor
{
    GENERATED_BODY()

public:
    AChipActor();

    // ------------------------------------------------------------------
    // Components
    // ------------------------------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chip|Mesh")
    UStaticMeshComponent* ChipMesh;

    // ------------------------------------------------------------------
    // Visual configuration
    // ------------------------------------------------------------------

    /**
     * Face value of the chip.  Drives the base-colour tint automatically:
     *   $1   = White/grey
     *   $5   = Red
     *   $25  = Green
     *   $100 = Black
     *   $500 = Purple
     *   $1000= Orange/yellow
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Config")
    float Denomination = 5.f;

    /**
     * Override the automatic tint with a custom colour.
     * Only used when bOverrideColor == true.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Config")
    bool bOverrideColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Config",
              meta = (EditCondition = "bOverrideColor"))
    FLinearColor CustomColor = FLinearColor::White;

    /** Edge metallic insert colour (usually gold/silver). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Config")
    FLinearColor EdgeColor = FLinearColor(0.8f, 0.75f, 0.2f, 1.f);

    /** Roughness value (clay-like — recommended 0.65–0.80). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Config",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ChipRoughness = 0.72f;

    // ------------------------------------------------------------------
    // Animation / interaction
    // ------------------------------------------------------------------

    /** Whether this chip is currently being dragged by the player. */
    UPROPERTY(BlueprintReadOnly, Category = "Chip|State")
    bool bIsHeld = false;

    /** Emissive pulse rate when winning resolution animation plays (Hz). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Animation",
              meta = (ClampMin = "0.1", ClampMax = "10.0"))
    float WinPulseRate = 2.5f;

    /** Duration (s) of the win-pulse animation. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chip|Animation",
              meta = (ClampMin = "0.1", ClampMax = "5.0"))
    float WinPulseDuration = 1.8f;

    // ------------------------------------------------------------------
    // Blueprint callable API
    // ------------------------------------------------------------------

    /** Play the win animation (pulsing emissive + scale bounce). */
    UFUNCTION(BlueprintCallable, Category = "Chip|Animation")
    void PlayWinAnimation();

    /** Play the lose animation (brief red flash + sink). */
    UFUNCTION(BlueprintCallable, Category = "Chip|Animation")
    void PlayLoseAnimation();

    /**
     * Smoothly move to a world-space target location.
     * Used when paying out chips from the tray to a bet zone.
     */
    UFUNCTION(BlueprintCallable, Category = "Chip|Animation")
    void MoveTo(FVector TargetLocation, float Duration = 0.5f);

    /**
     * Returns the colour that corresponds to the chip's denomination
     * (follows standard US casino colour conventions).
     */
    UFUNCTION(BlueprintPure, Category = "Chip|Config")
    FLinearColor GetDenominationColor() const;

    // ------------------------------------------------------------------
    // AActor overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    /** Create and configure the dynamic material instance. */
    void InitMaterial();

    UPROPERTY()
    UMaterialInstanceDynamic* ChipMID;

    // Animation state
    bool  bPlayingWinAnim = false;
    bool  bPlayingLoseAnim = false;
    float AnimTimer = 0.f;
    bool  bMoving = false;
    FVector MoveStartLocation;
    FVector MoveTargetLocation;
    float MoveDuration = 0.5f;
    float MoveTimer = 0.f;
};
