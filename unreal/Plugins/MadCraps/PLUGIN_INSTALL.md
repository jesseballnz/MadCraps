# MadCraps Plugin Installation Guide

A professional craps game engine for Unreal Engine 5 with Vegas-accurate rules, configurable payouts, and multiplayer support.

## Requirements

- **Unreal Engine 5.4+**
- **C++ Project** (not Blueprint-only)
- CMake 3.10+ (for building the native rules engine)
- Visual Studio 2022 or Xcode (for C++ compilation)

## Installation Steps

### 1. Build the Native Rules Engine

First, build the C++ rules engine library:

```bash
cd rules
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

This generates `madcraps_rules.lib` (Windows) or `libmadcraps_rules.a` (Mac/Linux).

### 2. Copy Plugin to Your Project

Copy the entire `unreal/Plugins/MadCraps` folder into your UE5 project:

```bash
# From the MadCraps repository root:
cp -r unreal/Plugins/MadCraps <YourProject>/Plugins/
```

### 3. Regenerate Visual Studio Project

Close Unreal Editor, then regenerate your project files:

```bash
# Windows
<YourProject>.uproject → right-click → "Generate Visual Studio project files"

# Mac/Linux
cd <YourProject>
rm -rf Intermediate Binaries
./GenerateProjectFiles.sh
```

### 4. Open and Compile

Open your project in Unreal Editor. It will automatically compile the plugin modules on first load.

## Quick Start: Creating a Craps Table

### In Blueprint:

1. **Create Game State**
   - Call `Create Game State` (MadCraps Rules Library)
   - Stores all active game data

2. **Configure Table Rules**
   - Call `Set Table Config` with a custom `MadCrapsTableConfig`
   - Adjust field payouts, odds, buy commissions, etc.

3. **Roll Dice**
   - Call `Roll Dice` to generate a result
   - Returns `Die1`, `Die2`, and `Total`

4. **Resolve Bets**
   - Create an array of `MadCrapsBet` structs
   - Call `Resolve Bets On Roll` with the array
   - Returns array of `MadCrapsPayout` structs (net winnings per bet)

### Example Blueprint Graph:

```
Create Game State
  → Set Table Config (Vegas Classic)
  → Roll Dice (returns result)
  → Resolve Bets On Roll (with active bets)
  → Apply Payouts (to player wallets)
```

## Table Configuration Reference

### FMadCrapsTableConfig Properties

**Field Bets:**
- `Field2Payout` — payout multiplier for rolling 2 (default 2.0x = 2:1)
- `Field12Payout` — payout multiplier for rolling 12 (default 3.0x = 3:1)

**Odds:**
- `OddsPointX` — true odds multiplier for each point (4, 5, 6, 8, 9, 10)

**Buy Bets:**
- `BuyCommissionPercent` — commission taken on buy wins (default 0.05 = 5%)

**Hardways:**
- `HardwayX` — payout for hard X (4, 6, 8, 10)

**Proposition Bets:**
- `Any7Payout` — any 7 bet payout (default 4.0x = 4:1)
- `AnyCrapsPayout` — any craps bet payout (default 7.0x = 7:1)
- `HornPayout` — horn bet payout (default 30:1 on 2/12, 15:1 on 3/11)

## Calculating House Edge

To validate rule accuracy, use:

```cpp
float HouseEdge = UMadCrapsRulesLibrary::CalculateHouseEdge(
    GameState,
    EMadCrapsBetType::PassLine,
    100000  // trials
);
// Expected result: ~-0.0141 (1.41% house edge on Pass Line)
```

## Supported Platforms

- **Windows (64-bit)**
- **Mac (Intel & Apple Silicon)**
- **Linux**
- **iOS** (with Blueprints only)
- **Android** (with Blueprints only)

## Multiplayer Setup (Optional)

The plugin includes hooks for a Rust-based authoritative server. See `server/README.md` for details on setting up the multiplayer backend.

## Troubleshooting

### Plugin fails to load in Unreal Editor

**Issue:** "Missing module" or compilation errors

**Solution:**
1. Ensure the native rules engine is built (`rules/build/` exists)
2. Verify library path in `MadCrapsRules.Build.cs` matches your build output
3. Delete `Intermediate/` and `Binaries/` folders, regenerate project

### House edge values don't match expected results

**Issue:** Simulation results differ from known craps statistics

**Solution:**
1. Verify `FMadCrapsTableConfig` matches the intended ruleset
2. Run the standalone C++ simulator: `rules/build/large_simulator 10000000`
3. Compare results and adjust payout values in config

### Blueprint references show as "unknown" after plugin update

**Issue:** Stale Blueprint cache

**Solution:**
1. Close Unreal Editor
2. Delete `Intermediate/` folder
3. Reopen project and recompile

## Documentation

- **C++ Rules Engine:** `rules/README.md`
- **Architecture & Design:** `DESIGN.md`
- **API Reference:** See inline Doxygen comments in plugin headers

## Support & Contributions

- **Issues:** https://github.com/jesseballnz/MadCraps/issues
- **Discussions:** https://github.com/jesseballnz/MadCraps/discussions
- **Pull Requests:** Always welcome!

## License

See LICENSE file in repository root.
