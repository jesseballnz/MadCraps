# MadCraps Unreal plugin integration notes

This directory contains instructions and a scaffold for integrating the rules engine as a native Unreal plugin.

Steps to integrate:
1. Create a new Unreal plugin (C++ - Runtime) inside your UE5 project: "MadCrapsRules".
2. Add the compiled static library (madcraps_rules) to the plugin's ThirdParty folder or compile the rules source directly as part of the plugin module.
3. Expose a thin UCLASS wrapper that calls into the rules engine functions and dispatches results to Blueprints.
4. Use the rules engine for authoritative resolution. For multiplayer, call server RPCs that execute rules on the server and replicate signed roll tokens to clients for visuals.
5. Use `AMadCrapsTableActor` from this plugin as a blockout actor for a basic 3D craps table level.

Included in this scaffold:
- `UMadCrapsRulesWrapper`: Blueprint-facing wrapper around the native rules engine.
- `AMadCrapsTableActor`: procedural table geometry, felt, layout, chip stacks, and a Blueprint/C++ betting API for Pass, Place, Field, and Hardway bets.
- `AMadCrapsDieActor` + `UMadCrapsDiceRollCoordinatorComponent`: local suspense roll plus authoritative settle path for dice.
- `CRAPS_TABLE_LEVEL_GUIDE.md`: level setup notes and marketplace asset suggestions.

Dice flow:
1. Call `StartPreviewDiceRoll(...)` on `AMadCrapsTableActor` when the shooter throws.
2. Resolve the actual dice values through the rules engine or server.
3. Package the result into `FMadCrapsAuthoritativeRoll`, including optional token/signature metadata.
4. Call `ApplyAuthoritativeDiceRoll(...)` to snap both dice to the canonical outcome after the reveal delay.
5. Use `PlacePassBet`, `PlacePlaceBet`, `PlaceFieldBet`, `PlaceHardwayBet`, or generic `PlaceBet`, then call `ResolveActiveBetsForRoll` or `ResolveActiveBetsForDice` to update the table's active wager state and point.

The coordinator supports two modes:
- `Animated`: deterministic anticipation roll with no collider tuning required.
- `Physics`: impulse-driven throw using UE rigid body simulation, still snapped to the authoritative result at settle time.

Notes:
- For deterministic replay and auditing, the server should seed the RNG with a per-round nonce and sign the resulting roll with the server's private key. Clients validate the signature before accepting the visual result.
- The visualization should animate dice but snap to the authoritative outcome at the end. This prevents desyncs while preserving nice visuals.
- The plugin includes a first-pass sound foundation via `UMadCrapsSoundProfile` and `UMadCrapsSoundLibrary` so table audio can be wired before final assets are imported.
