# MadCraps — 3D Craps Table: Material Parameter Guide

This document describes every material parameter exposed in the MadCraps plugin's
`FTableSurfaceMaterialParams` struct and how to set them up in Unreal Engine 5's
Material Editor to achieve photorealistic results.

---

## Overview of Materials

| Material Asset (assign in BP) | Surface            | Key files                  |
|-------------------------------|--------------------|-----------------------------|
| `M_CrapsTableFelt`            | Felt playing surface | `T_Felt_BC`, `T_Felt_N`, `T_Felt_R` |
| `M_CrapsTableWood`            | Table box & rails  | `T_Wood_BC`, `T_Wood_N`, `T_Wood_R` |
| `M_MetalRail`                 | Brass/chrome rail  | `T_Metal_BC`, `T_Metal_N`, `T_Metal_R`, `T_Metal_M` |
| `M_DiceMaterial`              | Casino dice        | `T_Dice_BC`, `T_Dice_N`, `T_Dice_R` |
| `M_ChipMaterial`              | Casino chips       | `T_Chip_BC`, `T_Chip_N`, `T_Chip_R` |
| `M_TextNumber`                | Bet zone text/numbers | `T_TextAtlas_BC` (decal) |

---

## Shared Parameters (all surface materials)

### `BaseColorTint` (Linear Color)
- **Type:** `VectorParameter`
- **Description:** Multiplied against the base-colour texture to tint the surface.
  Use the theme presets in `ACrapsTableActor::GetPresetForTheme()` as starting points.
- **Vegas Classic:** `(0.04, 0.28, 0.06, 1.0)` — rich casino green
- **High Roller:** `(0.02, 0.01, 0.04, 1.0)` — near black
- **Tip:** Keep the value below `0.5` for saturation; the texture handles most albedo detail.

### `Roughness` (Scalar)
- **Type:** `ScalarParameter`
- **Range:** `0.0` (mirror) – `1.0` (chalk)
- **Recommended values:**
  - Felt: `0.70–0.78` (fine textile catch-light)
  - Wood: `0.50–0.60` (polished wood, slight sheen)
  - Metal rail: `0.15–0.30` (brushed brass or chrome)
  - Dice: `0.05–0.12` (high-gloss casino-regulation acrylic)
  - Chip: `0.65–0.75` (matte clay composite)

### `Metallic` (Scalar)
- **Type:** `ScalarParameter`
- **Range:** `0.0–1.0`
- **Only non-zero on:** Metal rail (`0.85–1.0`), optional chrome chip edges.
- **Felt, wood, dice, chips:** Keep at `0.0`.

### `Specular` (Scalar)
- **Type:** `ScalarParameter`
- **Range:** `0.0–1.0` (maps to IOR 1.0–2.0 approximately)
- **Felt:** `0.25–0.35`
- **Wood (lacquered):** `0.40–0.55`
- **Dice:** `0.75–0.85`

### `NormalTiling` (Scalar)
- **Type:** `ScalarParameter`
- **Description:** UV tiling multiplier for the detail normal map layer.
- **Felt:** `4.0–6.0` (fine weave detail)
- **Wood:** `2.5–3.5` (grain runs along table length)
- **Metal:** `3.0–4.0` (brushing direction)

---

## Felt-Specific Parameters

### `EmissiveColor` (Linear Color)
- **Type:** `VectorParameter`
- **Description:** Colour of the emissive glow when a bet zone is highlighted.
  This is blended via `EmissiveIntensity` in the material graph.
- **Vegas:** warm gold `(1.0, 0.85, 0.1, 1.0)`
- **High Roller:** purple `(0.7, 0.2, 1.0, 1.0)`
- The `CrapsTableActor` sets `EmissiveIntensity = 0` on idle zones and ramps it
  up for winning zones / current-point indicator.

### `EmissiveIntensity` (Scalar)
- **Type:** `ScalarParameter`
- **Range:** `0.0–10.0`
- Driven at runtime by `ACrapsTableActor::UpdateFeltMaterial()`.
- **Idle:** `0.0`
- **Point highlight:** `3.0–4.0`
- **Win flash peak:** `6.0–8.0`

### `TextOpacity` (Scalar)
- **Type:** `ScalarParameter`
- **Range:** `0.0–1.0`
- Controls the opacity of the painted text/number layer.
  Multiply this with a high-contrast `T_TextAtlas_BC` texture for legible zone labels.
- Keep at `1.0` for normal gameplay; can fade to `0.0` for pure cinematic shots.

---

## Material Graph Setup (Felt — M_CrapsTableFelt)

```
[T_Felt_BC] ──┐
              ├─ Multiply ──► BaseColor
[BaseColorTint]┘

[T_Felt_N] ─────────────────► Normal
[NormalTiling] ──► TexCoord scale (detail layer)

[T_Felt_R] ─────────────────► Roughness override ─► Lerp(R_tex, Roughness, 0.5)
[Roughness] (scalar)

[Specular] ─────────────────► Specular

[EmissiveColor] ──┐
                  ├─ Multiply ──► Emissive Color
[EmissiveIntensity]┘

[T_TextAtlas] ──┐
                ├─ Lerp over BaseColor (Alpha = TextOpacity)
[TextOpacity] ──┘
```

> **Lumen note:** Mark the felt `M_CrapsTableFelt` as "Fully Rough" disabled.
> Enable "Two-Sided Lighting" only if the underside of the felt is visible.
> The emissive channel participates in Lumen GI automatically — keep peak
> `EmissiveIntensity` ≤ 8 to avoid over-brightening nearby surfaces.

---

## Dice Material (M_DiceMaterial)

Casino dice are transparent red acrylic or opaque white acrylic with
machine-drilled pip cavities filled with paint.  To approximate this:

- **BaseColor:** near white `(0.92, 0.92, 0.88)` tinted by `DiceBodyColor`
- **Roughness:** `0.07` (essentially a mirror at grazing angles)
- **Specular:** `0.78`
- **Metallic:** `0.0`
- **Normal:** sharp-edge bevel map + pip cavity normal
- **FaceEmissive** (Scalar): drives the winning-face gold glow:
  ```
  [FaceEmissiveIntensity] ──► Multiply ──► Emissive Color
  [FaceEmissiveColor (gold)] ┘
  ```
  The `DiceActor` drives `FaceEmissive` to a sine-wave pulse after snapping.

---

## Chip Material (M_ChipMaterial)

Casino chips are injection-moulded clay composite with:
- Slightly matte surface (roughness `0.70–0.75`)
- Coloured edge inserts (metallic band, `Metallic = 0.85`, `Roughness = 0.20`)
- Embossed denomination value (normal map)

**Parameters driven by `ChipActor`:**

| Parameter        | Type   | Notes                                              |
|-----------------|--------|----------------------------------------------------|
| `BaseColorTint`  | Color  | Set per denomination (red, green, black, purple…)  |
| `EdgeColor`      | Color  | Metallic edge insert colour                        |
| `Roughness`      | Scalar | Clay body roughness                                |
| `EmissiveIntensity` | Scalar | Win pulse brightness                            |

---

## Metal Rail (M_MetalRail)

Brushed brass (Vegas) or chrome (Atlantic City):

- **Metallic:** `0.92–1.0`
- **Roughness:** `0.18–0.28` (brushed = higher roughness in brush direction)
- **BaseColor:** The `BaseColorTint` parameter replaces albedo for pure metals.
- **Anisotropy:** For brushed look, enable the "Anisotropy" material property in UE5
  and connect a `T_Metal_A` (anisotropy direction) texture.

---

## Performance Notes

- Assign **4K base-colour**, **4K normal**, **2K roughness/metallic** textures for primary surfaces.
- Use **texture streaming** (`r.Streaming.PoolSize 2048`) to manage VRAM on consoles.
- Enable **Nanite** on `SM_CrapsTable`, `SM_Rails`, `SM_Felt`, and `SM_Dice` meshes.
- Do **not** enable Nanite on `SM_Chip` if stacking >20 chips (use LOD mesh instead).
- Target **32 materials draws** or fewer per frame for the whole table scene.
