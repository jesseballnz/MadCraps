# MadCraps — Camera Setup Guide

This guide explains how to configure `ACrapsCamera` for each preset and how
to create cinematic shots that showcase the photorealistic craps table.

---

## Quick Start

1. **Place `ACrapsCamera`** in your level.
2. **Assign `TableActor`** to the `ACrapsTableActor` instance in the same level.
3. **Call `SetAsViewTarget()`** (or set `Auto Activate for Player 0` in the Blueprint).
4. **Press Play** — the camera starts in `DealerView` by default.
5. **Call `SetPreset(ECrapsCameraPreset)`** from your UI/input Blueprint to switch views.

---

## Preset Reference

### `DealerView` — Overhead isometric
```
Location offset:   (0, 0, +420 cm above table centre)
Rotation:          Pitch -90°, Yaw 0°
FOV:               78°
Spring arm length: 420 cm
DOF:               f/11, focus 300 cm (very deep field — whole table sharp)
Vignette:          0.15
Exposure bias:     +0.2 EV
```
**Best for:** UI layouts, bet placement overview, tutorial screenshots.

---

### `PlayerView` — Low-angle intimate
```
Location offset:   (0, -162 cm, +60 cm)
Rotation:          Pitch -18°, Yaw 90°
FOV:               58°
Spring arm length: 180 cm
DOF:               f/2.4, focus 120 cm (shallow — dice sharp, table edges soft)
Vignette:          0.35
Exposure bias:     +0.4 EV
Chromatic aberration: 0.8
```
**Best for:** Main gameplay view. The shallow DOF focuses attention on the
dice and active bet zone while the rim of the table blurs beautifully.

---

### `CinematicDolly` — Sweeping orbit
```
Orbit radius:   340 cm
Orbit speed:    12°/s (full revolution ≈ 30 s)
Height offset:  +80 cm
FOV:            65°
DOF:            f/3.5, focus 220 cm
```
**Best for:** Attract mode / idle screen, replays, menu backgrounds.
Call `SetDollying(true)` to start the automated orbit.
The camera maintains a look-at toward `TableActor->GetActorLocation()`.

---

### `DiceCloseUp` — Macro dice focus
```
Location offset:   (80 cm along table, 0, +50 cm)
Rotation:          Pitch -55°, Yaw 180°
FOV:               40°
Spring arm length: 55 cm
DOF:               f/1.4, focus 40 cm (razor-thin: only dice faces sharp)
Vignette:          0.45
Chromatic aberration: 1.2
```
**Best for:** Roll resolution reveal. Call `CutToDiceCloseUp(HoldDuration)` from the
`CrapsTableManager.OnRollResolved` event to cut to this view for the dramatic result,
then automatically return to the previous camera after `HoldDuration` seconds.

**Blueprint wiring example:**
```
[OnRollResolved] → CrapsCamera.CutToDiceCloseUp(3.0)
                 → (3 s later) returns to PlayerView automatically
```

---

### `SidePerspective` — 45° side view
```
Location offset:   (-180 cm, +240 cm, +70 cm)
Rotation:          Pitch -20°, Yaw 140°
FOV:               62°
Spring arm length: 300 cm
DOF:               f/5.6, focus 180 cm
```
**Best for:** Multiplayer split-screen reference, second player's perspective.

---

## Smooth Transitions

All preset switches via `SetPreset()` interpolate over `TransitionDuration` (default 1.0 s)
using smoothstep easing (`TransitionEasingExp = 2.0`).

To make a transition feel more dramatic (fast cut → slow settle):
- Set `TransitionEasingExp = 3.0` (cubic ease-out)
- Reduce `TransitionDuration = 0.6 s`

For slow, cinematic dissolves:
- Set `TransitionDuration = 2.5 s`
- Set `TransitionEasingExp = 1.5`

---

## Depth of Field Tuning

UE5 uses a **Circle of Confusion** model for Depth of Field.  The key parameters:

| Parameter         | C++ name       | Typical range |
|------------------|----------------|---------------|
| Focal distance    | `FocalDistance` | 40–400 cm     |
| Aperture (f-stop) | `Aperture`     | 1.4–16        |
| Sensor width      | Camera FOV     | affects scale |

**Rule of thumb:** For a 58° FOV camera at ~180 cm from the table, use `f/2.4`
for a pleasing bokeh background with the near dice plane in focus.

If the dice look blurry at `DiceCloseUp`, reduce `FocalDistance` or check the
world-space transform of the dice — they should be ~40 cm from the camera origin.

---

## Post-Process Quick Reference

All post-process settings are managed via `FCrapsPostProcessConfig` per camera preset.

| Setting                  | Effect                                            |
|--------------------------|--------------------------------------------------|
| `ExposureBias`           | +EV = brighter (simulate underexposed sensor)    |
| `VignetteIntensity`      | Dark corners; `0.25–0.45` for cinematic          |
| `ToneMapperShoulder`     | Compress highlights; `0.35` prevents clipping    |
| `ColorGradingShadows`    | Warm amber `(1, 0.95, 0.85)` for Vegas mood      |
| `bChromaticAberration`   | Lens colour fringing on edges — subtle (<1.2)   |

---

## Advanced: Multiple Simultaneous Cameras

For split-screen or Picture-in-Picture:
1. Spawn two `ACrapsCamera` instances.
2. Set different `DefaultPreset` values on each.
3. Use `UGameplayStatics::GetPlayerController(0/1)->SetViewTarget()`.
4. Each camera maintains its own post-process blend.

---

## Tips for Photorealistic Composition

- **Rule of thirds:** Position the current-point number zone at a thirds intersection.
- **Leading lines:** The long axis of the table naturally leads the eye; tilt the
  camera 2–3° for dynamism.
- **Rim lighting:** Ensure the `RimFillLight` is visible in `PlayerView` — it
  separates the chip stacks from the felt.
- **Bokeh quality:** In Project Settings → Rendering → Depth of Field, enable
  **Circular DOF** (not Gaussian) for production quality.
- **Motion blur:** For dice rolls, enable per-object motion blur on `SM_Dice`.
  Set `r.MotionBlur.Scale 0.5` for subtle dice motion without smearing.
