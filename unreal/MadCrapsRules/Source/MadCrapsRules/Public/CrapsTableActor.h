#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrapsTableTypes.h"
#include "CrapsTableActor.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UDirectionalLightComponent;
class UMaterialInstanceDynamic;
class UBetZoneComponent;
class ADiceActor;
class AChipActor;

/**
 * ACrapsTableActor
 *
 * The central Blueprint-spawnable actor representing a fully rendered
 * craps table.  It manages:
 *
 *   - All static mesh components (table body, rails, felt surface, legs)
 *   - Dynamic material instances for every surface (felt, wood, metal,
 *     text/number overlays)
 *   - Bet zone child components for every standard wager area
 *   - Casino-style lighting (overhead directional + rail-mounted point lights)
 *   - Theme/preset switching at runtime
 *   - Delegation hooks that the CrapsTableManager drives in response to
 *     game events (point changes, roll outcomes, payout visualization)
 *
 * -------------------------------------------------------------------------
 * Mesh slots (assign Static Meshes in the Blueprint defaults):
 *   SM_TableBody   — Main table box with pocket corners
 *   SM_Felt        — Flush felt playing surface (Nanite recommended)
 *   SM_Rails       — Four corner/edge rail pieces
 *   SM_LegSet      — Pedestal or leg assembly
 *
 * Material slots (assign Material Instances in the Blueprint defaults):
 *   MI_Felt        — M_CrapsTableFelt instance
 *   MI_Wood        — M_CrapsTableWood instance
 *   MI_Metal       — M_MetalRail instance
 *   MI_TextNumbers — M_TextNumber decal instance
 * -------------------------------------------------------------------------
 */
UCLASS(BlueprintType, Blueprintable)
class MADCRAPSRULES_API ACrapsTableActor : public AActor
{
    GENERATED_BODY()

public:
    ACrapsTableActor();

    // ------------------------------------------------------------------
    // Static mesh components
    // ------------------------------------------------------------------

    /** Primary table body (wood box, padded edges, pockets). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* TableBodyMesh;

    /** Green felt playing surface — set Nanite=true on the mesh asset. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* FeltSurfaceMesh;

    /** Four wooden/padded rails around the perimeter. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* RailMesh;

    /** Brushed-metal chip tray (dealer side). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* ChipTrayMesh;

    /** Mirrored second chip tray (opposite side). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* ChipTrayMesh2;

    /** Puck / ON-OFF button mesh (white = ON, black = OFF). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* PuckMesh;

    /** Dice stick visual. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Mesh")
    UStaticMeshComponent* DiceStickMesh;

    // ------------------------------------------------------------------
    // Lighting components
    // ------------------------------------------------------------------

    /**
     * Primary overhead casino light.
     * Simulates the warm tungsten spotlight array found in real casinos.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Lighting")
    UPointLightComponent* OverheadLight1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Lighting")
    UPointLightComponent* OverheadLight2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Lighting")
    UPointLightComponent* OverheadLight3;

    /** Ambient fill light beneath the table rim (warm amber bounce). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table|Lighting")
    UPointLightComponent* RimFillLight;

    // ------------------------------------------------------------------
    // Configurable properties
    // ------------------------------------------------------------------

    /** Which built-in theme to apply at BeginPlay. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Theme")
    ECrapsTableTheme DefaultTheme = ECrapsTableTheme::VegasClassic;

    /** If Theme == Custom, these parameters are used directly. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Theme",
              meta = (EditCondition = "DefaultTheme == ECrapsTableTheme::Custom"))
    FCrapsTablePresetData CustomPreset;

    /** Lighting configuration — controls light colours, intensities, Lumen settings. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Lighting")
    FCrapsLightingConfig LightingConfig;

    /** Post-process configuration — tone mapping, DOF, vignette, etc. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|PostProcess")
    FCrapsPostProcessConfig PostProcessConfig;

    /**
     * Material parameter config used when DefaultTheme == Custom.
     * For built-in themes, call GetPresetForTheme() to retrieve defaults.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Materials")
    FCrapsTableMaterialConfig MaterialConfig;

    /** Highlight emissive intensity for the current-point number zones. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Game",
              meta = (ClampMin = "0.0", ClampMax = "10.0"))
    float PointHighlightIntensity = 3.5f;

    /** Duration (s) to keep winning zones highlighted before returning to idle. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table|Game",
              meta = (ClampMin = "0.5", ClampMax = "10.0"))
    float WinHighlightDuration = 2.f;

    // ------------------------------------------------------------------
    // Runtime-spawned dice and chip references
    // ------------------------------------------------------------------

    /** Die A (right die) — spawned by this actor at BeginPlay. */
    UPROPERTY(BlueprintReadOnly, Category = "Table|Dice")
    ADiceActor* DiceA;

    /** Die B (left die) — spawned by this actor at BeginPlay. */
    UPROPERTY(BlueprintReadOnly, Category = "Table|Dice")
    ADiceActor* DiceB;

    // ------------------------------------------------------------------
    // Delegates (bind from Blueprint or CrapsTableManager)
    // ------------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Table|Events")
    FOnBetPlaced OnBetPlaced;

    UPROPERTY(BlueprintAssignable, Category = "Table|Events")
    FOnBetRemoved OnBetRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Table|Events")
    FOnRollResult OnRollResolved;

    UPROPERTY(BlueprintAssignable, Category = "Table|Events")
    FOnPointChanged OnPointChanged;

    // ------------------------------------------------------------------
    // Blueprint callable API
    // ------------------------------------------------------------------

    /**
     * Apply a built-in theme preset to the whole table.
     * Updates material parameters, lighting, and post-process in one call.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Theme")
    void ApplyTheme(ECrapsTableTheme Theme);

    /**
     * Apply a fully-customised preset (materials + lighting + post-process).
     * Useful for runtime theme switching or editor tooling.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Theme")
    void ApplyPreset(const FCrapsTablePresetData& Preset);

    /**
     * Returns the default preset data for a given theme.
     * Blueprints can call this, modify individual fields, then call ApplyPreset.
     */
    UFUNCTION(BlueprintPure, Category = "Table|Theme")
    static FCrapsTablePresetData GetPresetForTheme(ECrapsTableTheme Theme);

    /**
     * Highlight the zone corresponding to the current-point number.
     * Clears any previous point highlight first.
     * @param PointNumber   4, 5, 6, 8, 9, 10 — or 0 to clear.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    void SetCurrentPoint(int32 PointNumber);

    /** Returns the currently set point number (0 = come-out phase). */
    UFUNCTION(BlueprintPure, Category = "Table|Game")
    int32 GetCurrentPoint() const { return CurrentPoint; }

    /**
     * Drive the table's visual response to a resolved roll.
     * Highlights winning zones, dims losing zones, updates the puck.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    void OnRollResult(const FCrapsRollResolution& Resolution);

    /**
     * Show the ON puck over a given point number zone.
     * @param PointNumber  4,5,6,8,9,10 or 0 to show OFF.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    void SetPuckState(int32 PointNumber);

    /**
     * Place a bet token on the specified zone (adds chip stack actor).
     * Returns false if the zone is not found.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    bool PlaceBet(ECrapsBetZone Zone, float Amount);

    /**
     * Remove a bet from the specified zone.
     * Returns the amount removed.
     */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    float RemoveBet(ECrapsBetZone Zone);

    /** Remove all active bets from the table. */
    UFUNCTION(BlueprintCallable, Category = "Table|Game")
    void ClearAllBets();

    /** Returns the bet zone component for a given zone type (or nullptr). */
    UFUNCTION(BlueprintPure, Category = "Table|Game")
    UBetZoneComponent* GetBetZone(ECrapsBetZone Zone) const;

    // ------------------------------------------------------------------
    // AActor overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    /** Build all BetZoneComponents and position them relative to the felt surface. */
    void InitBetZones();

    /** Position the three overhead point lights symmetrically above the table. */
    void InitLighting();

    /** Construct dynamic material instances from their base material slots. */
    void InitMaterialInstances();

    /** Push material parameter changes through to the DMIs. */
    void UpdateFeltMaterial(const FTableSurfaceMaterialParams& Params);
    void UpdateWoodMaterial(const FTableSurfaceMaterialParams& Params);
    void UpdateMetalMaterial(const FTableSurfaceMaterialParams& Params);

    /** Spawn the two dice actors near the shooter end. */
    void SpawnDice();

    // Runtime state
    int32 CurrentPoint = 0;

    // Dynamic material instances (created at BeginPlay from assigned MIs)
    UPROPERTY()
    UMaterialInstanceDynamic* FeltMID;

    UPROPERTY()
    UMaterialInstanceDynamic* WoodMID;

    UPROPERTY()
    UMaterialInstanceDynamic* MetalMID;

    /** All registered bet zone components keyed by ECrapsBetZone. */
    UPROPERTY()
    TMap<ECrapsBetZone, UBetZoneComponent*> BetZones;
};
