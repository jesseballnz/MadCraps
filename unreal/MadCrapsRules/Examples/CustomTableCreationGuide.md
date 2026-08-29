# MadCraps — Custom Table Creation Guide

This guide explains how to create a completely custom-themed craps table
for your project — custom felt colour, wood type, rail material, logo, and lighting.

---

## Step 1: Create a Table Preset Data Asset (Recommended)

The cleanest approach is to define your theme as a `FCrapsTablePresetData` struct
directly in a Blueprint Data Asset so designers can iterate without recompiling C++.

### Creating a Blueprint Data Asset

1. In the **Content Browser**, right-click → **Miscellaneous → Data Asset**.
2. Choose `PrimaryDataAsset` as the parent class (or a custom C++ class if you prefer).
3. Add a property of type `FCrapsTablePresetData` named `PresetData`.
4. Fill in all the nested structs (`Materials`, `Lighting`, `PostProcess`).
5. Save the asset.

In the level Blueprint or `ACrapsTableManager`, load the asset and call:

```cpp
TableActor->ApplyPreset(MyPreset->PresetData);
```

---

## Step 2: Define Material Parameters

For each surface (Felt, Wood, Metal, Dice, Chip), fill the matching
`FTableSurfaceMaterialParams` struct:

```cpp
FCrapsTablePresetData MyNightClub;

// Deep violet felt
MyNightClub.Materials.Felt.BaseColorTint   = FLinearColor(0.08f, 0.01f, 0.20f, 1.f);
MyNightClub.Materials.Felt.Roughness       = 0.70f;
MyNightClub.Materials.Felt.EmissiveColor   = FLinearColor(0.5f, 0.0f, 1.0f, 1.f); // neon purple
MyNightClub.Materials.Felt.EmissiveIntensity = 0.f; // idle (zones light up on wins)

// Black lacquered wood
MyNightClub.Materials.Wood.BaseColorTint   = FLinearColor(0.03f, 0.03f, 0.03f, 1.f);
MyNightClub.Materials.Wood.Roughness       = 0.28f;
MyNightClub.Materials.Wood.Specular        = 0.60f;

// Rose gold rails
MyNightClub.Materials.MetalRail.BaseColorTint = FLinearColor(0.80f, 0.55f, 0.50f, 1.f);
MyNightClub.Materials.MetalRail.Metallic      = 1.0f;
MyNightClub.Materials.MetalRail.Roughness     = 0.20f;
```

See `MaterialParameterGuide.md` for the full parameter reference.

---

## Step 3: Configure Lighting

```cpp
MyNightClub.Lighting.LightTemperature       = 2700.f;  // ultra-warm
MyNightClub.Lighting.DirectionalLightIntensity = 4500.f;
MyNightClub.Lighting.bEnableRailLights      = true;
MyNightClub.Lighting.RailLightIntensity     = 3000.f;
MyNightClub.Lighting.RimLightColor          = FLinearColor(0.3f, 0.0f, 0.4f, 1.f); // purple rim
```

---

## Step 4: Configure Post-Process

```cpp
MyNightClub.PostProcess.ExposureBias        = -0.5f;    // slightly under-exposed for mystery
MyNightClub.PostProcess.VignetteIntensity   = 0.55f;    // heavy vignette
MyNightClub.PostProcess.Aperture            = 2.0f;     // very shallow DOF
MyNightClub.PostProcess.FocalDistance       = 140.f;
MyNightClub.PostProcess.bChromaticAberration = true;
MyNightClub.PostProcess.ChromaticAberrationIntensity = 1.5f;
```

---

## Step 5: Apply at Runtime

In Blueprint:

```
[BeginPlay] → TableActor.ApplyPreset(MyNightClubPreset)
```

Or respond to a player selection:

```
[OnThemeSelected(ThemeName)] → switch
    "VegasClassic"   → TableActor.ApplyTheme(VegasClassic)
    "AtlanticCity"   → TableActor.ApplyTheme(AtlanticCity)
    "MyNightClub"    → TableActor.ApplyPreset(MyNightClubPreset)
```

---

## Step 6: Custom Mesh Assignments

Replace the default static meshes in the **CrapsTableActor Blueprint**:

| Slot              | Details panel property | Recommended source                  |
|-------------------|------------------------|-------------------------------------|
| `TableBodyMesh`   | SM_CrapsTable          | Model in Blender/Maya, export as FBX |
| `FeltSurfaceMesh` | SM_Felt                | Flat plane with UV layout for decals |
| `RailMesh`        | SM_Rails               | Four-piece corner rail set           |
| `ChipTrayMesh`    | SM_ChipTray            | Brushed-metal chip rack              |
| `PuckMesh`        | SM_Puck                | 5 cm disk, two-sided material        |
| `DiceStickMesh`   | SM_DiceStick           | Thin cane/stick prop                 |

### Enabling Nanite

For each high-detail mesh:
1. Open the asset in the Static Mesh Editor.
2. Check **Enable Nanite Support** under the LOD settings.
3. Set **Fallback Triangle Percent** to `30%` for non-Nanite fallback.

### UV Layout for Felt Decals

The `T_TextAtlas_BC` texture contains all bet zone labels arranged in a
single 4K texture atlas.  The UV layout matches the default felt mesh:

```
UV quadrant (0,0)–(0.5, 0.5)  → Pass line & Don't Pass area
UV quadrant (0.5,0)–(1, 0.5)  → Come / Don't Come + Field
UV quadrant (0,0.5)–(0.5, 1)  → Point numbers 4–10
UV quadrant (0.5,0.5)–(1,1)   → Centre props (Hardways, Horn, Any 7)
```

If you use a custom felt mesh with different UVs, re-render the text atlas
in Substance Painter or Photoshop matching your UV layout.

---

## Adjusting Bet Zone Positions

If your table mesh uses different proportions than the default (244 cm × 122 cm),
you can override each zone's relative position in C++ or Blueprint:

**C++:** Modify `ACrapsTableActor::InitBetZones()` — edit the `ZoneDefinitions` array.

**Blueprint:** After `BeginPlay`, call `GetBetZone(Zone)->SetRelativeLocation(NewPos)`
on each `UBetZoneComponent` you want to reposition.

---

## Preset Gallery (Built-In)

| Theme              | Felt            | Wood       | Rails    | Key mood            |
|--------------------|-----------------|------------|----------|---------------------|
| Vegas Classic      | Casino green    | Walnut     | Brass    | Warm, classic       |
| Atlantic City      | Deep blue       | Oak        | Chrome   | Cool, modern hotel  |
| High Roller        | Near black      | Ebony      | 24k gold | Dark, exclusive     |
| Luxury Red         | Burgundy        | Mahogany   | Brass    | Rich, opulent       |
| Custom             | Your choice     | Your choice| Your choice | Fully configurable |

Switch between them with a single call:

```blueprint
TableActor.ApplyTheme(HighRoller)
```

---

## Texture Requirements (Custom Assets)

For custom PBR textures, follow this pipeline:

| Map          | Format  | Size  | Colour space   |
|-------------|---------|-------|----------------|
| Base Colour | PNG/EXR | 4096² | sRGB           |
| Normal Map  | PNG/EXR | 4096² | Linear (non-sRGB) |
| Roughness   | PNG/EXR | 2048² | Linear         |
| Metallic    | PNG/EXR | 2048² | Linear         |
| Emissive    | EXR     | 2048² | HDR Linear     |
| AO          | PNG     | 2048² | Linear         |

Pack **Roughness** into the G channel and **Metallic** into the B channel of a
single ORM texture (`T_Surface_ORM`) to halve texture memory usage.

In Unreal:
- Right-click the texture → **Texture Settings → Compression → Masks** for ORM textures.
- Use **BC5 compression** for normal maps.
- Enable **Mip Gen Settings = Blur3** for normal maps to reduce aliasing at distance.

---

## Testing Your Theme

1. Place `ACrapsTableActor` in a test level.
2. Add a simple spotlight above it.
3. In the Details panel, set `DefaultTheme = Custom` and fill `CustomPreset`.
4. Press **Play In Editor** — the theme applies at `BeginPlay`.
5. To preview changes without entering PIE: in `PostEditChangeProperty`,
   the actor auto-applies the theme when you modify `DefaultTheme`.
   This requires WITH_EDITOR defined (already included in `CrapsTableActor.cpp`).
