#pragma once

#include "CoreMinimal.h"
#include "CrapsTableTypes.generated.h"

// ---------------------------------------------------------------------------
// Enumerations
// ---------------------------------------------------------------------------

/** All bet zones available on a standard craps table. */
UENUM(BlueprintType)
enum class ECrapsBetZone : uint8
{
    None            UMETA(DisplayName = "None"),
    PassLine        UMETA(DisplayName = "Pass Line"),
    DontPass        UMETA(DisplayName = "Don't Pass"),
    Come            UMETA(DisplayName = "Come"),
    DontCome        UMETA(DisplayName = "Don't Come"),
    Field           UMETA(DisplayName = "Field"),
    Place4          UMETA(DisplayName = "Place 4"),
    Place5          UMETA(DisplayName = "Place 5"),
    Place6          UMETA(DisplayName = "Place 6"),
    Place8          UMETA(DisplayName = "Place 8"),
    Place9          UMETA(DisplayName = "Place 9"),
    Place10         UMETA(DisplayName = "Place 10"),
    HardWay4        UMETA(DisplayName = "Hard 4"),
    HardWay6        UMETA(DisplayName = "Hard 6"),
    HardWay8        UMETA(DisplayName = "Hard 8"),
    HardWay10       UMETA(DisplayName = "Hard 10"),
    AnyCraps        UMETA(DisplayName = "Any Craps"),
    Any7            UMETA(DisplayName = "Any 7"),
    Horn            UMETA(DisplayName = "Horn"),
    Horn2           UMETA(DisplayName = "Horn High 2"),
    Horn3           UMETA(DisplayName = "Horn High 3"),
    Horn11          UMETA(DisplayName = "Horn High 11"),
    Horn12          UMETA(DisplayName = "Horn High 12"),
    Big6            UMETA(DisplayName = "Big 6"),
    Big8            UMETA(DisplayName = "Big 8"),
    PassLineOdds    UMETA(DisplayName = "Pass Line Odds"),
    DontPassOdds    UMETA(DisplayName = "Don't Pass Odds"),
};

/** Outcome categories returned by the rules engine. */
UENUM(BlueprintType)
enum class ECrapsOutcome : uint8
{
    None            UMETA(DisplayName = "None"),
    NaturalWin      UMETA(DisplayName = "Natural (7 or 11) — Pass Line Win"),
    CrapsOut        UMETA(DisplayName = "Craps (2, 3, 12) — Pass Line Loss"),
    PointSet        UMETA(DisplayName = "Point Set"),
    PointHit        UMETA(DisplayName = "Point Hit — Win"),
    SevenOut        UMETA(DisplayName = "Seven Out — Loss"),
    FieldWin        UMETA(DisplayName = "Field Win"),
    FieldLoss       UMETA(DisplayName = "Field Loss"),
    HardWayWin      UMETA(DisplayName = "Hard Way Win"),
    HardWayLoss     UMETA(DisplayName = "Hard Way Loss"),
    PropWin         UMETA(DisplayName = "Proposition Win"),
    PropLoss        UMETA(DisplayName = "Proposition Loss"),
    PlaceWin        UMETA(DisplayName = "Place Win"),
    PlaceLoss       UMETA(DisplayName = "Place Loss"),
};

/** Predefined visual themes for the craps table. */
UENUM(BlueprintType)
enum class ECrapsTableTheme : uint8
{
    VegasClassic    UMETA(DisplayName = "Vegas Classic (Green/Gold)"),
    AtlanticCity    UMETA(DisplayName = "Atlantic City (Blue/Silver)"),
    HighRoller      UMETA(DisplayName = "High Roller (Black/Purple)"),
    LuxuryRed       UMETA(DisplayName = "Luxury Red (Burgundy/Brass)"),
    Vintage         UMETA(DisplayName = "Vintage (Tan/Bronze)"),
    Custom          UMETA(DisplayName = "Custom"),
};

/** Camera preset positions. */
UENUM(BlueprintType)
enum class ECrapsCameraPreset : uint8
{
    DealerView      UMETA(DisplayName = "Dealer / Overhead (isometric)"),
    PlayerView      UMETA(DisplayName = "Player (low angle, intimate)"),
    CinematicDolly  UMETA(DisplayName = "Cinematic Dolly (sweeping)"),
    DiceCloseUp     UMETA(DisplayName = "Dice Close-Up"),
    SidePerspective UMETA(DisplayName = "Side Perspective"),
};

/** Visual state of an individual bet zone. */
UENUM(BlueprintType)
enum class EBetZoneState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Hovered         UMETA(DisplayName = "Hovered"),
    Active          UMETA(DisplayName = "Active (Bet Placed)"),
    Winning         UMETA(DisplayName = "Winning"),
    Losing          UMETA(DisplayName = "Losing"),
    CurrentPoint    UMETA(DisplayName = "Current Point"),
};

// ---------------------------------------------------------------------------
// Structs
// ---------------------------------------------------------------------------

/** PBR material parameter set for a single table surface. */
USTRUCT(BlueprintType)
struct FTableSurfaceMaterialParams
{
    GENERATED_BODY()

    /** Base (albedo) colour tint, multiplied over the base-colour texture. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    FLinearColor BaseColorTint = FLinearColor(0.08f, 0.35f, 0.09f, 1.f); // casino green

    /** 0 = fully dielectric, 1 = fully metallic. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float Metallic = 0.f;

    /** Surface roughness — 0 = mirror, 1 = fully diffuse. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float Roughness = 0.72f;

    /** Specular occlusion multiplier. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float Specular = 0.35f;

    /** Detail normal map tile scale. Higher = finer detail. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float NormalTiling = 4.f;

    /** Emissive colour used when a zone is highlighted (winning/point). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    FLinearColor EmissiveColor = FLinearColor(1.f, 0.85f, 0.1f, 1.f); // warm gold

    /** Emissive intensity multiplier. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float EmissiveIntensity = 0.f;

    /** Opacity of the decal/text overlay layer (0–1). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    float TextOpacity = 1.f;
};

/** Full material configuration for every surface on the table. */
USTRUCT(BlueprintType)
struct FCrapsTableMaterialConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    FTableSurfaceMaterialParams Felt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    FTableSurfaceMaterialParams Wood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    FTableSurfaceMaterialParams MetalRail;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    FTableSurfaceMaterialParams Dice;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    FTableSurfaceMaterialParams Chip;
};

/** Lighting parameters for the table scene. */
USTRUCT(BlueprintType)
struct FCrapsLightingConfig
{
    GENERATED_BODY()

    /** Colour temperature (Kelvin) of the overhead casino lights. Warm ~3200K, neutral ~5500K. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (ClampMin = "1700.0", ClampMax = "10000.0"))
    float LightTemperature = 3400.f;

    /** Intensity of the directional overhead light (lux). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (ClampMin = "0.0", ClampMax = "100000.0"))
    float DirectionalLightIntensity = 8000.f;

    /** Whether to enable Lumen global illumination. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bEnableLumen = true;

    /** Number of Lumen indirect bounces (4–8 recommended). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (ClampMin = "1", ClampMax = "16"))
    int32 LumenBounces = 4;

    /** Enable dynamic point lights mounted to table rails. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    bool bEnableRailLights = true;

    /** Colour of the ambient fill light under the table rim. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
    FLinearColor RimLightColor = FLinearColor(0.2f, 0.15f, 0.05f, 1.f);

    /** Intensity of rail-mounted point lights. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting", meta = (ClampMin = "0.0", ClampMax = "10000.0"))
    float RailLightIntensity = 1200.f;
};

/** Post-process parameters for cinematic presentation. */
USTRUCT(BlueprintType)
struct FCrapsPostProcessConfig
{
    GENERATED_BODY()

    /** Target exposure (EV100). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "-8.0", ClampMax = "8.0"))
    float ExposureBias = 0.3f;

    /** Vignette intensity (0 = off, 1 = full). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float VignetteIntensity = 0.25f;

    /** Filmic tone mapper highlight desaturation (0–1). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ToneMapperShoulder = 0.35f;

    /** Colour grading shadows tint — casino-warm amber. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
    FVector4 ColorGradingShadows = FVector4(1.0f, 0.95f, 0.85f, 1.0f);

    /** Depth of field focal distance from the camera (cm). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "10.0"))
    float FocalDistance = 180.f;

    /** Depth of field aperture (f-stop). Smaller = shallower DOF. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "1.0", ClampMax = "32.0"))
    float Aperture = 4.5f;

    /** Enable chromatic aberration for cinematic lens imperfection. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
    bool bChromaticAberration = true;

    /** Chromatic aberration intensity (0–5). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess", meta = (ClampMin = "0.0", ClampMax = "5.0"))
    float ChromaticAberrationIntensity = 0.6f;
};

/** A single placed bet on the table. */
USTRUCT(BlueprintType)
struct FCrapsBet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bet")
    ECrapsBetZone Zone = ECrapsBetZone::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bet")
    float Amount = 0.f;

    /** Optional target number for Place/Come/Don't Come bets. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bet")
    int32 TargetNumber = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bet")
    bool bActive = true;
};

/** Result of resolving a single roll against the placed bets. */
USTRUCT(BlueprintType)
struct FCrapsRollResolution
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    int32 DieA = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    int32 DieB = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    int32 Total = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    int32 PointAfterRoll = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    ECrapsOutcome Outcome = ECrapsOutcome::None;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    TArray<FCrapsBet> WinningBets;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    TArray<FCrapsBet> LosingBets;

    UPROPERTY(BlueprintReadOnly, Category = "Result")
    float TotalPayout = 0.f;
};

/** Complete descriptor for a visual table theme/preset. */
USTRUCT(BlueprintType)
struct FCrapsTablePresetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FName PresetName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    ECrapsTableTheme Theme = ECrapsTableTheme::VegasClassic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FCrapsTableMaterialConfig Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FCrapsLightingConfig Lighting;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FCrapsPostProcessConfig PostProcess;
};

// ---------------------------------------------------------------------------
// Delegate declarations (used by table actors and manager)
// ---------------------------------------------------------------------------

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBetPlaced,   ECrapsBetZone, Zone, float, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBetRemoved,  ECrapsBetZone, Zone, float, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnRollResult,  FCrapsRollResolution, Resolution);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnPointChanged, int32, NewPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnZoneStateChanged, ECrapsBetZone, Zone, EBetZoneState, NewState);
