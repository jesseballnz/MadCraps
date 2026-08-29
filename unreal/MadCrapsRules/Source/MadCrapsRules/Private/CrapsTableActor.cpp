#include "CrapsTableActor.h"
#include "BetZoneComponent.h"
#include "DiceActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

// ---------------------------------------------------------------------------
// Default material parameter sets per theme
// ---------------------------------------------------------------------------

namespace MadCrapsThemeDefaults
{
    // ---------- Vegas Classic (green felt, walnut wood, brass rails) ----------
    static FCrapsTablePresetData Vegas()
    {
        FCrapsTablePresetData P;
        P.PresetName = FName("VegasClassic");
        P.Theme      = ECrapsTableTheme::VegasClassic;

        // Felt
        P.Materials.Felt.BaseColorTint    = FLinearColor(0.04f, 0.28f, 0.06f, 1.f);
        P.Materials.Felt.Roughness        = 0.75f;
        P.Materials.Felt.Specular         = 0.3f;
        P.Materials.Felt.NormalTiling     = 5.f;
        P.Materials.Felt.EmissiveColor    = FLinearColor(1.f, 0.85f, 0.1f, 1.f);
        // Wood
        P.Materials.Wood.BaseColorTint    = FLinearColor(0.18f, 0.09f, 0.04f, 1.f);
        P.Materials.Wood.Roughness        = 0.55f;
        P.Materials.Wood.Specular         = 0.45f;
        P.Materials.Wood.NormalTiling     = 3.f;
        // Metal
        P.Materials.MetalRail.BaseColorTint  = FLinearColor(0.72f, 0.58f, 0.22f, 1.f); // brass
        P.Materials.MetalRail.Metallic       = 0.95f;
        P.Materials.MetalRail.Roughness      = 0.25f;
        P.Materials.MetalRail.Specular       = 0.7f;
        // Dice
        P.Materials.Dice.BaseColorTint    = FLinearColor(0.95f, 0.95f, 0.95f, 1.f);
        P.Materials.Dice.Roughness        = 0.08f;
        P.Materials.Dice.Metallic         = 0.f;
        P.Materials.Dice.Specular         = 0.8f;
        // Chip
        P.Materials.Chip.Roughness        = 0.72f;

        P.Lighting.LightTemperature       = 3200.f;
        P.Lighting.DirectionalLightIntensity = 7500.f;

        P.PostProcess.ExposureBias        = 0.3f;
        P.PostProcess.VignetteIntensity   = 0.25f;
        P.PostProcess.Aperture            = 4.5f;
        P.PostProcess.FocalDistance       = 180.f;
        return P;
    }

    // ---------- Atlantic City (blue felt, oak wood, chrome rails) ----------
    static FCrapsTablePresetData AtlanticCity()
    {
        FCrapsTablePresetData P;
        P.PresetName = FName("AtlanticCity");
        P.Theme      = ECrapsTableTheme::AtlanticCity;

        P.Materials.Felt.BaseColorTint    = FLinearColor(0.04f, 0.10f, 0.38f, 1.f); // deep blue
        P.Materials.Felt.Roughness        = 0.72f;
        P.Materials.Felt.EmissiveColor    = FLinearColor(0.4f, 0.8f, 1.f, 1.f);     // cyan glow
        P.Materials.Wood.BaseColorTint    = FLinearColor(0.24f, 0.16f, 0.08f, 1.f); // lighter oak
        P.Materials.MetalRail.BaseColorTint = FLinearColor(0.75f, 0.75f, 0.78f, 1.f); // chrome
        P.Materials.MetalRail.Metallic    = 1.f;
        P.Materials.MetalRail.Roughness   = 0.18f;

        P.Lighting.LightTemperature       = 5500.f;
        P.Lighting.DirectionalLightIntensity = 9000.f;
        P.Lighting.RimLightColor          = FLinearColor(0.1f, 0.2f, 0.4f, 1.f);

        P.PostProcess.ExposureBias        = 0.1f;
        P.PostProcess.VignetteIntensity   = 0.2f;
        return P;
    }

    // ---------- High Roller (black felt, ebony wood, gold rails) ----------
    static FCrapsTablePresetData HighRoller()
    {
        FCrapsTablePresetData P;
        P.PresetName = FName("HighRoller");
        P.Theme      = ECrapsTableTheme::HighRoller;

        P.Materials.Felt.BaseColorTint    = FLinearColor(0.02f, 0.01f, 0.04f, 1.f); // near black
        P.Materials.Felt.Roughness        = 0.68f;
        P.Materials.Felt.EmissiveColor    = FLinearColor(0.7f, 0.2f, 1.f, 1.f);     // violet
        P.Materials.Wood.BaseColorTint    = FLinearColor(0.04f, 0.03f, 0.03f, 1.f); // ebony
        P.Materials.Wood.Roughness        = 0.35f;
        P.Materials.MetalRail.BaseColorTint = FLinearColor(0.85f, 0.72f, 0.28f, 1.f); // 24k gold
        P.Materials.MetalRail.Metallic    = 1.f;
        P.Materials.MetalRail.Roughness   = 0.12f;

        P.Lighting.LightTemperature       = 2700.f;
        P.Lighting.DirectionalLightIntensity = 5000.f;
        P.Lighting.RimLightColor          = FLinearColor(0.3f, 0.1f, 0.4f, 1.f);
        P.Lighting.RailLightIntensity     = 2000.f;

        P.PostProcess.ExposureBias        = -0.3f;
        P.PostProcess.VignetteIntensity   = 0.45f;
        P.PostProcess.Aperture            = 2.8f;
        return P;
    }

    // ---------- Luxury Red (burgundy felt, mahogany, brass) ----------
    static FCrapsTablePresetData LuxuryRed()
    {
        FCrapsTablePresetData P;
        P.PresetName = FName("LuxuryRed");
        P.Theme      = ECrapsTableTheme::LuxuryRed;

        P.Materials.Felt.BaseColorTint    = FLinearColor(0.28f, 0.02f, 0.04f, 1.f); // burgundy
        P.Materials.Felt.Roughness        = 0.70f;
        P.Materials.Felt.EmissiveColor    = FLinearColor(1.f, 0.4f, 0.1f, 1.f);     // orange-gold
        P.Materials.Wood.BaseColorTint    = FLinearColor(0.20f, 0.05f, 0.03f, 1.f); // mahogany
        P.Materials.MetalRail.BaseColorTint = FLinearColor(0.75f, 0.60f, 0.25f, 1.f);
        P.Materials.MetalRail.Metallic    = 0.9f;

        P.Lighting.LightTemperature       = 3000.f;
        P.Lighting.RimLightColor          = FLinearColor(0.4f, 0.15f, 0.05f, 1.f);
        return P;
    }
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ACrapsTableActor::ACrapsTableActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Table body — assign SM_CrapsTable in the Blueprint defaults
    TableBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableBodyMesh"));
    TableBodyMesh->SetupAttachment(Root);

    // Felt surface — enable Nanite on the assigned mesh asset
    FeltSurfaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FeltSurfaceMesh"));
    FeltSurfaceMesh->SetupAttachment(TableBodyMesh);
    FeltSurfaceMesh->SetRelativeLocation(FVector(0.f, 0.f, 2.f)); // slightly above table body

    // Rails
    RailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RailMesh"));
    RailMesh->SetupAttachment(TableBodyMesh);

    // Chip trays (dealer and player sides)
    ChipTrayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChipTrayMesh"));
    ChipTrayMesh->SetupAttachment(TableBodyMesh);
    ChipTrayMesh->SetRelativeLocation(FVector(0.f, -70.f, 4.f));

    ChipTrayMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChipTrayMesh2"));
    ChipTrayMesh2->SetupAttachment(TableBodyMesh);
    ChipTrayMesh2->SetRelativeLocation(FVector(0.f, 70.f, 4.f));

    // Puck
    PuckMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuckMesh"));
    PuckMesh->SetupAttachment(FeltSurfaceMesh);
    PuckMesh->SetRelativeLocation(FVector(-80.f, -55.f, 1.f)); // Don't Pass bar area by default

    // Dice stick
    DiceStickMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceStickMesh"));
    DiceStickMesh->SetupAttachment(FeltSurfaceMesh);
    DiceStickMesh->SetRelativeLocation(FVector(60.f, 0.f, 1.f));

    // --------------- Lighting ---------------
    // Three overhead pendants along the length of the table
    OverheadLight1 = CreateDefaultSubobject<UPointLightComponent>(TEXT("OverheadLight1"));
    OverheadLight1->SetupAttachment(Root);
    OverheadLight1->SetRelativeLocation(FVector(-90.f, 0.f, 200.f));
    OverheadLight1->SetIntensity(8000.f);
    OverheadLight1->SetLightColor(FLinearColor(1.f, 0.92f, 0.78f));
    OverheadLight1->SetAttenuationRadius(350.f);
    OverheadLight1->bUseInverseSquaredFalloff = true;
    OverheadLight1->CastShadows = true;

    OverheadLight2 = CreateDefaultSubobject<UPointLightComponent>(TEXT("OverheadLight2"));
    OverheadLight2->SetupAttachment(Root);
    OverheadLight2->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
    OverheadLight2->SetIntensity(7500.f);
    OverheadLight2->SetLightColor(FLinearColor(1.f, 0.90f, 0.75f));
    OverheadLight2->SetAttenuationRadius(300.f);
    OverheadLight2->bUseInverseSquaredFalloff = true;

    OverheadLight3 = CreateDefaultSubobject<UPointLightComponent>(TEXT("OverheadLight3"));
    OverheadLight3->SetupAttachment(Root);
    OverheadLight3->SetRelativeLocation(FVector(90.f, 0.f, 200.f));
    OverheadLight3->SetIntensity(8000.f);
    OverheadLight3->SetLightColor(FLinearColor(1.f, 0.92f, 0.78f));
    OverheadLight3->SetAttenuationRadius(350.f);
    OverheadLight3->bUseInverseSquaredFalloff = true;

    // Warm rim fill
    RimFillLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RimFillLight"));
    RimFillLight->SetupAttachment(Root);
    RimFillLight->SetRelativeLocation(FVector(0.f, 0.f, -30.f));
    RimFillLight->SetIntensity(1200.f);
    RimFillLight->SetLightColor(FLinearColor(0.2f, 0.15f, 0.05f));
    RimFillLight->SetAttenuationRadius(200.f);
    RimFillLight->bUseInverseSquaredFalloff = true;
    RimFillLight->CastShadows = false;
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------

void ACrapsTableActor::BeginPlay()
{
    Super::BeginPlay();
    InitMaterialInstances();
    InitBetZones();
    InitLighting();
    ApplyTheme(DefaultTheme);
    SpawnDice();
}

void ACrapsTableActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ---------------------------------------------------------------------------
// Theme API
// ---------------------------------------------------------------------------

void ACrapsTableActor::ApplyTheme(ECrapsTableTheme Theme)
{
    FCrapsTablePresetData Preset;
    switch (Theme)
    {
    case ECrapsTableTheme::VegasClassic: Preset = MadCrapsThemeDefaults::Vegas();        break;
    case ECrapsTableTheme::AtlanticCity: Preset = MadCrapsThemeDefaults::AtlanticCity(); break;
    case ECrapsTableTheme::HighRoller:   Preset = MadCrapsThemeDefaults::HighRoller();   break;
    case ECrapsTableTheme::LuxuryRed:    Preset = MadCrapsThemeDefaults::LuxuryRed();    break;
    case ECrapsTableTheme::Custom:       Preset = CustomPreset;                           break;
    default:                             Preset = MadCrapsThemeDefaults::Vegas();         break;
    }
    ApplyPreset(Preset);
}

void ACrapsTableActor::ApplyPreset(const FCrapsTablePresetData& Preset)
{
    MaterialConfig = Preset.Materials;
    LightingConfig = Preset.Lighting;
    PostProcessConfig = Preset.PostProcess;

    UpdateFeltMaterial(Preset.Materials.Felt);
    UpdateWoodMaterial(Preset.Materials.Wood);
    UpdateMetalMaterial(Preset.Materials.MetalRail);

    // Update overhead light colour temperature and intensity
    const FLinearColor LightColor = FLinearColor::MakeFromColorTemperature(
        Preset.Lighting.LightTemperature);
    for (UPointLightComponent* Light : { OverheadLight1, OverheadLight2, OverheadLight3 })
    {
        if (Light)
        {
            Light->SetLightColor(LightColor);
            Light->SetIntensity(Preset.Lighting.DirectionalLightIntensity);
        }
    }

    if (RimFillLight)
    {
        RimFillLight->SetLightColor(Preset.Lighting.RimLightColor);
        RimFillLight->SetIntensity(
            Preset.Lighting.bEnableRailLights ? Preset.Lighting.RailLightIntensity : 0.f);
    }
}

FCrapsTablePresetData ACrapsTableActor::GetPresetForTheme(ECrapsTableTheme Theme)
{
    switch (Theme)
    {
    case ECrapsTableTheme::AtlanticCity: return MadCrapsThemeDefaults::AtlanticCity();
    case ECrapsTableTheme::HighRoller:   return MadCrapsThemeDefaults::HighRoller();
    case ECrapsTableTheme::LuxuryRed:    return MadCrapsThemeDefaults::LuxuryRed();
    default:                             return MadCrapsThemeDefaults::Vegas();
    }
}

// ---------------------------------------------------------------------------
// Game state
// ---------------------------------------------------------------------------

void ACrapsTableActor::SetCurrentPoint(int32 PointNumber)
{
    // Clear previous point highlight
    if (CurrentPoint != 0)
    {
        static const TMap<int32, ECrapsBetZone> PointToZone =
        {
            {4,  ECrapsBetZone::Place4},
            {5,  ECrapsBetZone::Place5},
            {6,  ECrapsBetZone::Place6},
            {8,  ECrapsBetZone::Place8},
            {9,  ECrapsBetZone::Place9},
            {10, ECrapsBetZone::Place10},
        };
        const ECrapsBetZone* PrevZone = PointToZone.Find(CurrentPoint);
        if (PrevZone)
        {
            UBetZoneComponent* ZoneComp = GetBetZone(*PrevZone);
            if (ZoneComp && ZoneComp->GetZoneState() == EBetZoneState::CurrentPoint)
            {
                ZoneComp->SetZoneState(EBetZoneState::Idle);
            }
        }
    }

    CurrentPoint = PointNumber;
    SetPuckState(PointNumber);

    if (PointNumber != 0)
    {
        static const TMap<int32, ECrapsBetZone> PointToZone =
        {
            {4,  ECrapsBetZone::Place4},
            {5,  ECrapsBetZone::Place5},
            {6,  ECrapsBetZone::Place6},
            {8,  ECrapsBetZone::Place8},
            {9,  ECrapsBetZone::Place9},
            {10, ECrapsBetZone::Place10},
        };
        const ECrapsBetZone* NewZone = PointToZone.Find(PointNumber);
        if (NewZone)
        {
            UBetZoneComponent* ZoneComp = GetBetZone(*NewZone);
            if (ZoneComp)
            {
                ZoneComp->SetZoneState(EBetZoneState::CurrentPoint);
            }
        }
    }

    OnPointChanged.Broadcast(PointNumber);
}

void ACrapsTableActor::OnRollResult(const FCrapsRollResolution& Resolution)
{
    // Flash winning zones
    for (const FCrapsBet& Bet : Resolution.WinningBets)
    {
        UBetZoneComponent* ZoneComp = GetBetZone(Bet.Zone);
        if (ZoneComp)
        {
            ZoneComp->FlashResult(true);
        }
    }

    // Flash losing zones
    for (const FCrapsBet& Bet : Resolution.LosingBets)
    {
        UBetZoneComponent* ZoneComp = GetBetZone(Bet.Zone);
        if (ZoneComp)
        {
            ZoneComp->FlashResult(false);
        }
    }

    // Update point if changed
    if (Resolution.PointAfterRoll != CurrentPoint)
    {
        SetCurrentPoint(Resolution.PointAfterRoll);
    }

    OnRollResolved.Broadcast(Resolution);
}

void ACrapsTableActor::SetPuckState(int32 PointNumber)
{
    if (!PuckMesh)
    {
        return;
    }

    if (PointNumber == 0)
    {
        // OFF: move to Don't Pass area, use material slot 1 (black)
        PuckMesh->SetRelativeLocation(FVector(-80.f, -55.f, 1.f));
        PuckMesh->SetMaterial(0, nullptr); // caller assigns black puck material in BP
    }
    else
    {
        // ON: move above the point number zone
        // Approximate relative positions (cm) of each number on the felt
        static const TMap<int32, FVector> PointPositions =
        {
            {4,  FVector(-55.f,  38.f, 3.f)},
            {5,  FVector(-32.f,  38.f, 3.f)},
            {6,  FVector(-10.f,  38.f, 3.f)},
            {8,  FVector( 10.f,  38.f, 3.f)},
            {9,  FVector( 32.f,  38.f, 3.f)},
            {10, FVector( 55.f,  38.f, 3.f)},
        };
        const FVector* Pos = PointPositions.Find(PointNumber);
        if (Pos)
        {
            PuckMesh->SetRelativeLocation(*Pos);
        }
    }
}

bool ACrapsTableActor::PlaceBet(ECrapsBetZone Zone, float Amount)
{
    UBetZoneComponent* ZoneComp = GetBetZone(Zone);
    if (!ZoneComp)
    {
        return false;
    }

    ZoneComp->AddBetAmount(Amount);
    OnBetPlaced.Broadcast(Zone, Amount);
    return true;
}

float ACrapsTableActor::RemoveBet(ECrapsBetZone Zone)
{
    UBetZoneComponent* ZoneComp = GetBetZone(Zone);
    if (!ZoneComp || !ZoneComp->HasActiveBet())
    {
        return 0.f;
    }

    const float Amount = ZoneComp->GetBetAmount();
    ZoneComp->ClearBet();
    OnBetRemoved.Broadcast(Zone, Amount);
    return Amount;
}

void ACrapsTableActor::ClearAllBets()
{
    for (auto& Pair : BetZones)
    {
        if (Pair.Value)
        {
            const float Amount = Pair.Value->GetBetAmount();
            if (Amount > 0.f)
            {
                Pair.Value->ClearBet();
                OnBetRemoved.Broadcast(Pair.Key, Amount);
            }
        }
    }
}

UBetZoneComponent* ACrapsTableActor::GetBetZone(ECrapsBetZone Zone) const
{
    UBetZoneComponent* const* Found = BetZones.Find(Zone);
    return Found ? *Found : nullptr;
}

// ---------------------------------------------------------------------------
// Protected helpers
// ---------------------------------------------------------------------------

void ACrapsTableActor::InitBetZones()
{
    // Zone label, position relative to felt surface, hit extent
    struct FZoneData
    {
        ECrapsBetZone Zone;
        FText         Label;
        FVector       RelativePosition;
        FVector       HalfExtent;
    };

    // Approximate positions on a 244 cm × 122 cm craps table felt (scaled down by ~2.5)
    // All positions are relative to the centre of FeltSurfaceMesh
    const TArray<FZoneData> ZoneDefinitions =
    {
        // Pass Line — long bar near the players
        { ECrapsBetZone::PassLine,     FText::FromString("PASS LINE"),       FVector(0.f,   55.f, 0.f), FVector(95.f,  5.f, 1.f) },
        { ECrapsBetZone::DontPass,     FText::FromString("DON'T PASS BAR"),  FVector(0.f,   46.f, 0.f), FVector(95.f,  4.f, 1.f) },
        // Come / Don't Come
        { ECrapsBetZone::Come,         FText::FromString("COME"),            FVector(0.f,   30.f, 0.f), FVector(60.f,  7.f, 1.f) },
        { ECrapsBetZone::DontCome,     FText::FromString("DON'T COME"),      FVector(0.f,   20.f, 0.f), FVector(60.f,  5.f, 1.f) },
        // Field
        { ECrapsBetZone::Field,        FText::FromString("FIELD"),           FVector(0.f,    8.f, 0.f), FVector(75.f,  6.f, 1.f) },
        // Place numbers
        { ECrapsBetZone::Place4,       FText::FromString("4"),               FVector(-55.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        { ECrapsBetZone::Place5,       FText::FromString("5"),               FVector(-33.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        { ECrapsBetZone::Place6,       FText::FromString("6"),               FVector(-11.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        { ECrapsBetZone::Place8,       FText::FromString("8"),               FVector( 11.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        { ECrapsBetZone::Place9,       FText::FromString("9"),               FVector( 33.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        { ECrapsBetZone::Place10,      FText::FromString("10"),              FVector( 55.f, 38.f, 0.f), FVector(8.f,  8.f, 1.f) },
        // Hardways
        { ECrapsBetZone::HardWay4,     FText::FromString("HARD 4"),          FVector(-20.f,-10.f, 0.f), FVector(8.f,  5.f, 1.f) },
        { ECrapsBetZone::HardWay6,     FText::FromString("HARD 6"),          FVector( -7.f,-10.f, 0.f), FVector(8.f,  5.f, 1.f) },
        { ECrapsBetZone::HardWay8,     FText::FromString("HARD 8"),          FVector(  7.f,-10.f, 0.f), FVector(8.f,  5.f, 1.f) },
        { ECrapsBetZone::HardWay10,    FText::FromString("HARD 10"),         FVector( 20.f,-10.f, 0.f), FVector(8.f,  5.f, 1.f) },
        // Proposition (centre of table)
        { ECrapsBetZone::AnyCraps,     FText::FromString("ANY CRAPS"),       FVector(-35.f,-20.f, 0.f), FVector(12.f, 5.f, 1.f) },
        { ECrapsBetZone::Any7,         FText::FromString("ANY SEVEN"),       FVector(  0.f,-20.f, 0.f), FVector(12.f, 5.f, 1.f) },
        { ECrapsBetZone::Horn,         FText::FromString("HORN"),            FVector( 35.f,-20.f, 0.f), FVector(12.f, 5.f, 1.f) },
        // Big 6/8
        { ECrapsBetZone::Big6,         FText::FromString("BIG 6"),           FVector(-85.f, 10.f, 0.f), FVector(7.f,  6.f, 1.f) },
        { ECrapsBetZone::Big8,         FText::FromString("BIG 8"),           FVector( 85.f, 10.f, 0.f), FVector(7.f,  6.f, 1.f) },
    };

    for (const FZoneData& ZD : ZoneDefinitions)
    {
        FName CompName = *FString::Printf(TEXT("BetZone_%s"),
            *StaticEnum<ECrapsBetZone>()->GetNameStringByValue(static_cast<int64>(ZD.Zone)));

        UBetZoneComponent* Comp = NewObject<UBetZoneComponent>(this, CompName);
        Comp->ZoneType   = ZD.Zone;
        Comp->ZoneLabel  = ZD.Label;
        Comp->HitExtent  = ZD.HalfExtent;
        Comp->SetupAttachment(FeltSurfaceMesh);
        Comp->SetRelativeLocation(ZD.RelativePosition);
        Comp->RegisterComponent();

        BetZones.Add(ZD.Zone, Comp);
    }
}

void ACrapsTableActor::InitLighting()
{
    // Lighting is already configured in the constructor.
    // This function applies the runtime LightingConfig values.
    const FLinearColor LightColor =
        FLinearColor::MakeFromColorTemperature(LightingConfig.LightTemperature);

    for (UPointLightComponent* Light : { OverheadLight1, OverheadLight2, OverheadLight3 })
    {
        if (Light)
        {
            Light->SetLightColor(LightColor);
            Light->SetIntensity(LightingConfig.DirectionalLightIntensity);
        }
    }

    if (RimFillLight)
    {
        RimFillLight->SetLightColor(LightingConfig.RimLightColor);
        RimFillLight->SetIntensity(
            LightingConfig.bEnableRailLights ? LightingConfig.RailLightIntensity : 0.f);
    }
}

void ACrapsTableActor::InitMaterialInstances()
{
    // Create dynamic material instances from each mesh's material slot 0.
    // The Blueprint should assign the correct base materials to each mesh.


    if (FeltSurfaceMesh && FeltSurfaceMesh->GetMaterial(0))
    {
        FeltMID = UMaterialInstanceDynamic::Create(FeltSurfaceMesh->GetMaterial(0), FeltSurfaceMesh);
        FeltSurfaceMesh->SetMaterial(0, FeltMID);
    }
    if (TableBodyMesh && TableBodyMesh->GetMaterial(0))
    {
        WoodMID = UMaterialInstanceDynamic::Create(TableBodyMesh->GetMaterial(0), TableBodyMesh);
        TableBodyMesh->SetMaterial(0, WoodMID);
    }
    if (RailMesh && RailMesh->GetMaterial(0))
    {
        MetalMID = UMaterialInstanceDynamic::Create(RailMesh->GetMaterial(0), RailMesh);
        RailMesh->SetMaterial(0, MetalMID);
    }
}

void ACrapsTableActor::UpdateFeltMaterial(const FTableSurfaceMaterialParams& Params)
{
    if (!FeltMID) { return; }
    FeltMID->SetVectorParameterValue(TEXT("BaseColorTint"),  Params.BaseColorTint);
    FeltMID->SetScalarParameterValue(TEXT("Roughness"),      Params.Roughness);
    FeltMID->SetScalarParameterValue(TEXT("Specular"),       Params.Specular);
    FeltMID->SetScalarParameterValue(TEXT("NormalTiling"),   Params.NormalTiling);
    FeltMID->SetVectorParameterValue(TEXT("EmissiveColor"),  Params.EmissiveColor);
    FeltMID->SetScalarParameterValue(TEXT("EmissiveIntensity"), Params.EmissiveIntensity);
    FeltMID->SetScalarParameterValue(TEXT("TextOpacity"),    Params.TextOpacity);
}

void ACrapsTableActor::UpdateWoodMaterial(const FTableSurfaceMaterialParams& Params)
{
    if (!WoodMID) { return; }
    WoodMID->SetVectorParameterValue(TEXT("BaseColorTint"), Params.BaseColorTint);
    WoodMID->SetScalarParameterValue(TEXT("Roughness"),     Params.Roughness);
    WoodMID->SetScalarParameterValue(TEXT("Specular"),      Params.Specular);
    WoodMID->SetScalarParameterValue(TEXT("NormalTiling"),  Params.NormalTiling);
}

void ACrapsTableActor::UpdateMetalMaterial(const FTableSurfaceMaterialParams& Params)
{
    if (!MetalMID) { return; }
    MetalMID->SetVectorParameterValue(TEXT("BaseColorTint"), Params.BaseColorTint);
    MetalMID->SetScalarParameterValue(TEXT("Metallic"),      Params.Metallic);
    MetalMID->SetScalarParameterValue(TEXT("Roughness"),     Params.Roughness);
    MetalMID->SetScalarParameterValue(TEXT("Specular"),      Params.Specular);
}

void ACrapsTableActor::SpawnDice()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    const FVector TableOrigin = GetActorLocation();
    FActorSpawnParameters Params;
    Params.Owner = this;

    // Spawn near the shooter end of the table
    DiceA = World->SpawnActor<ADiceActor>(ADiceActor::StaticClass(),
        TableOrigin + FVector(70.f, -5.f, 10.f), FRotator::ZeroRotator, Params);

    DiceB = World->SpawnActor<ADiceActor>(ADiceActor::StaticClass(),
        TableOrigin + FVector(70.f,  8.f, 10.f), FRotator::ZeroRotator, Params);
}

#if WITH_EDITOR
void ACrapsTableActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.Property
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    // Live-preview theme changes in the editor
    if (PropertyName == GET_MEMBER_NAME_CHECKED(ACrapsTableActor, DefaultTheme) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(ACrapsTableActor, LightingConfig) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(ACrapsTableActor, MaterialConfig))
    {
        ApplyTheme(DefaultTheme);
    }
}
#endif
