# MadCraps — Lighting Tuning Reference

This guide covers the casino-style lighting setup for the `ACrapsTableActor`
and how to tune it for quality, mood, and performance across target platforms.

---

## Architecture Overview

The MadCraps lighting system has three layers:

| Layer                 | Component              | Purpose                                   |
|-----------------------|------------------------|-------------------------------------------|
| **Overhead pendants** | `OverheadLight1/2/3`   | Primary directional casino illumination   |
| **Rim fill**          | `RimFillLight`         | Warm bounce from table underside           |
| **Lumen GI**          | Project Settings       | Indirect bounced light, reflections        |

Plus optional global sources you should add to your level:
- **Sky Light** (capture mode = `Real Time`) for soft ambient
- **Directional Light** for a subtle top-down key (low intensity ≤ 1 lux)

---

## Overhead Pendant Lights

Three point lights spaced evenly along the table's length simulate a
typical casino chandelier array.

### Recommended values (Vegas Classic)

```
Intensity:        7500–8500 lux
Colour temp:      3000–3400 K  (warm tungsten/halogen)
Attenuation:      300–360 cm radius
Shadow softness:  Enable "Source Radius = 8 cm" for soft cast shadows
bUseInverseSquaredFalloff: true
```

To change the mood:
- **Cooler/brighter (high-roller focus):** 2700 K, 9000 lux
- **Night club feel:**  4200 K, reduce to 4000 lux + add coloured point lights
- **Outdoor/daylight:**  6500 K, 12000 lux

### Theme colour temperatures

| Theme            | Temp (K) | Colour character              |
|-----------------|----------|-------------------------------|
| Vegas Classic   | 3200     | Warm amber — classic casino   |
| Atlantic City   | 5500     | Cool neutral — modern hotel   |
| High Roller     | 2700     | Very warm, intimate           |
| Luxury Red      | 3000     | Warm red-gold                 |

---

## Rim Fill Light

A single low-intensity point light mounted slightly below the table surface
(`Z = -30 cm`) creates a warm bounce that separates chip stacks from the felt.

```
Intensity:   800–1500 lux
Colour:      (0.2, 0.15, 0.05)  — dark amber
Radius:      150–200 cm
CastShadows: false  (pure fill, no hard shadows)
```

Increase `RailLightIntensity` to `2000+` for a neon-sign glow effect (High Roller theme).

---

## Lumen Global Illumination

Lumen provides real-time bounced light so the green felt colour naturally
washes onto nearby chip stacks — no baked lightmaps required.

### Enabling Lumen (Project Settings)

```
Engine → Rendering → Global Illumination Method: Lumen
Engine → Rendering → Reflections Method:         Lumen
```

### Tuning for the craps table

```ini
# DefaultEngine.ini
[/Script/Engine.RendererSettings]
r.Lumen.GatherCvars.MaxTraceDistance=2500
r.Lumen.SceneCapture.CardUpdateFrequencyScale=2
r.Lumen.Reflections.ScreenSpaceReconstruction=1
r.Lumen.DiffuseIndirect.MaxBounces=4
```

| Setting                   | Recommended | Notes                                  |
|--------------------------|-------------|----------------------------------------|
| Max bounces              | 4           | Diminishing returns above 4 for tables |
| Trace distance           | 2500 cm     | Covers the full table + close walls    |
| Reflection quality       | High        | Needed for dice/chip specular          |
| Scene lighting update    | Every frame | For dynamic theme switching at runtime |

### Performance scaling

- **PC Ultra (RTX 3070+):** Lumen HW raytracing, reflection quality High
- **PC High / PS5:**        Lumen SW raytracing, reflection quality Medium
- **PC Medium / Xbox S:**  Lumen GI on, reflections = Screen Space
- **Mobile:**               Disable Lumen; use baked sky-light + reflection captures

---

## Dynamic Theme Lighting Changes

When `ApplyTheme()` is called at runtime, the three overhead lights and rim fill
are updated instantly.  For a smooth visual transition, wrap the call in a
Blueprint Timeline or C++ interpolation:

```cpp
// Example: lerp from current to target colour temperature over 1 second
void UMyGameMode::SmoothThemeTransition(ECrapsTableTheme NewTheme)
{
    // Read current and target FCrapsLightingConfig
    FCrapsTablePresetData NewPreset = ACrapsTableActor::GetPresetForTheme(NewTheme);
    // Lerp light parameters frame by frame via a timer/timeline...
    TableActor->ApplyPreset(BlendedPreset);
}
```

---

## Emissive Zone Highlighting

Winning bet zones and the current-point indicator use **emissive** material channels
rather than spawning new light actors — this keeps the lighting budget flat.

### How it works
1. `ACrapsTableActor::SetCurrentPoint()` calls `SetZoneState(CurrentPoint)` on
   the relevant `UBetZoneComponent`.
2. The component sets `EmissiveIntensity = 3.5` on its decal material.
3. Because Lumen processes emissive surfaces, the warm gold glow reflects subtly
   onto adjacent chips and dice without any extra light actors.

### Tuning emissive intensity

| Zone state      | `EmissiveIntensity` | Lumen visible |
|----------------|---------------------|---------------|
| Idle            | 0.0                 | No            |
| Current point   | 3.0–4.0             | Yes — subtle gold cast |
| Win flash peak  | 6.0–8.0             | Yes — noticeable |
| Win fade-out    | Interpolated → 0    | Fades naturally |

Keep emissive values below **10.0** to avoid Lumen over-brightening the scene.
Values > 10 require `r.Lumen.Scene.SurfaceCacheResolution` to be raised to
avoid blocky GI artefacts.

---

## Shadow Quality Settings

For soft, realistic shadows from casino lights:

```
Point Light Source Radius: 8 cm   (mimics a round bulb)
Shadow Bias:               0.04
Shadow Filter Sharpening:  0.5
Contact Shadows:           Enabled (length 0.05)
```

Enable **Virtual Shadow Maps** in Project Settings for highest quality:
```
Engine → Rendering → Shadow Map Method: Virtual Shadow Maps
```
VSMs give per-pixel shadow precision needed for chip stack shadows and
the thin edge of the dice landing on the felt.

---

## Post-Process Volume

Add a **Post Process Volume** to your level set to **Infinite Extent** with:

```
Tone mapper:        ACES Filmic
Film Shoulder:      0.35
Film Black Clip:    0.0
Film White Clip:    0.95
Color Grading Shadows: (1.0, 0.95, 0.85) — warm amber lift
Color Grading Midtones: (1.0, 0.98, 0.92) — slightly warm
Contrast:           1.05
Saturation:         1.08   — slightly punchy
```

These values give the characteristic "casino warm" look — not over-saturated,
but with a pleasant golden quality that flatters the green felt.

---

## Performance Budget (Lighting)

| Platform       | Target FPS | Lumen GI | Shadows          | Overhead lights |
|---------------|-----------|----------|------------------|-----------------|
| PC Ultra       | 60        | HW RT    | VSM              | 3 × dynamic     |
| PC High / PS5  | 60        | SW RT    | VSM              | 3 × dynamic     |
| PC Medium      | 60        | SW RT    | CSM (cascade 2)  | 2 × dynamic     |
| Mobile         | 30        | Off      | Static lightmap  | 0 × dynamic     |

On mobile, bake the felt/table lighting into a single **lightmap** at 256 px
and use a **Reflection Capture Actor** for dice/chip specular.
