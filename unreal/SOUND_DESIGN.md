# MadCraps 3D Table Sound Design Foundation

This document defines the first-pass sound layer for the UE5 table so the project can wire audio early without blocking on final asset purchases.

## Goals

- Make the dice feel heavy and readable: launch, bounce cluster, settle.
- Make chip interactions tactile without becoming noisy during rapid bet placement.
- Keep a low casino-bed in the room so the table never feels dead.
- Reserve dealer calls as explicit slots so VO can be replaced later without Blueprint rewiring.

## Sound Categories

### 1. Dice

- `DiceRollStart`: stick push or hand toss transient at throw start.
- `DiceBounce`: 3 to 6 short table/felt/rail impact variants triggered from physics contacts, rate-limited.
- `DiceSettle`: softer end-state tick when both dice sleep.

Recommended implementation:
- Trigger `DiceRollStart` once when the server-authoritative roll begins.
- Trigger `DiceBounce` from physical collision events, but gate to one event every `0.04` to `0.06` seconds per die.
- Trigger `DiceSettle` once after both dice have come to rest and before revealing payouts.

### 2. Chips

- `ChipSingle`: one chip placed onto the felt.
- `ChipStack`: 2 to 6 chip clacks for larger bet placement or odds behind line.
- `ChipSweep`: dealer clears losing bets.

Recommended implementation:
- Use `ChipSingle` for drag/drop and tap-to-place.
- Use `ChipStack` when a single action spawns multiple chip meshes.
- Duck repeated chip events slightly during spam placement by reducing gain `-1` to `-3 dB`.

### 3. Ambient Casino Bed

- `AmbientLoop`: low crowd murmur, distant slots, occasional glass/clink energy.

Recommended implementation:
- Loop in 2D at low volume or from room speakers around the table.
- Keep it subtle enough that dealer VO stays intelligible.
- Add light volume automation: lift slightly during idle, dip during major callouts.

### 4. Dealer Calls

- `DealerComingOut`
- `DealerSevenOut`
- `DealerPointMade`
- `DealerNoMoreBets`
- `DealerPlaceYourBets`

Recommended implementation:
- Start with placeholders using clean VO temp assets or synthesized speech.
- Route all dealer calls through the same Blueprint/audio event path that final recorded VO will use.
- Keep callouts non-overlapping; queue or suppress lower-priority lines if another line is already playing.

## UE5 Foundation Added In Plugin

The plugin now includes:

- `UMadCrapsSoundProfile` data asset for binding cue types to imported `USoundBase` assets.
- `EMadCrapsSoundCueType` enum covering dice, chips, ambient, and dealer placeholders.
- `UMadCrapsSoundLibrary::ResolveSoundCue(...)` Blueprint helper that picks a weighted random variant and returns randomized pitch/volume.

Suggested content structure once inside a UE project:

```text
Content/Audio/
  Dice/
  Chips/
  Ambience/
  Dealer/
  Mixes/
  Data/DA_MadCraps_SoundProfile.uasset
```

## Blueprint Wiring Plan

1. Create `DA_MadCraps_SoundProfile` from `UMadCrapsSoundProfile`.
2. Populate each slot with 3 to 8 variants where possible.
3. On dice throw start, call `ResolveSoundCue(DiceRollStart)` and `SpawnSoundAtLocation`.
4. On dice collision, call `ResolveSoundCue(DiceBounce)` with cooldown logic in Blueprint or component code.
5. On chip placement and sweeps, call the matching chip cues from interaction handlers.
6. Start `AmbientLoop` on table load and store the returned audio component for mix control.
7. Trigger dealer calls from game-state transitions, not raw animation timing.

## Marketplace Asset Recommendations

Use these as acquisition targets; exact pack choice depends on budget and style fit.

- **Dice / board impacts**
  - Unreal Marketplace / Fab: search `dice sound effects`, `board game foley`, `casino foley`.
  - Priority traits: close-mic impacts, wood edge hits, felt detail, multiple short variants.
- **Chip handling**
  - Unreal Marketplace / Fab: search `poker chips`, `casino chips`, `chip foley`.
  - Priority traits: clean single-chip drops, stack grabs, dealer sweeps, minimal reverb.
- **Ambient casino**
  - Unreal Marketplace / Fab: search `casino ambience`, `crowd ambience`, `slots ambience`.
  - Priority traits: loopable stereo beds, no foreground announcements, light slot presence.
- **Dealer placeholder VO**
  - Unreal Marketplace / Fab: search `announcer voice pack`, `casino voice`, `dealer voice`.
  - Priority traits: dry recordings, short phrases, commercial-safe placeholder usage.

Also consider external libraries if Marketplace selection is thin:

- Sonniss GDC bundles for general foley.
- BOOM Library casual/crowd ambience packs.
- Soundly or similar searchable libraries if the project needs rapid temping.

## Mixing Notes

- Keep dice transient peaks above chips so throws always read as the hero action.
- Roll off excessive low end on ambience; leave space for table impacts.
- Put dealer VO on a dedicated bus so it can sidechain ambience down by `2` to `4 dB`.
- Prefer short, dry source audio; add room treatment in-engine for consistency.
