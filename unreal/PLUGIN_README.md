# MadCraps Unreal Engine 5 Plugin

A professional, production-ready craps game engine with Vegas-accurate rules, configurable payouts, and multiplayer support.

## Installation

See `Plugins/MadCraps/PLUGIN_INSTALL.md` for detailed setup instructions.

## Quick Start

1. Build the native rules engine:
   ```bash
   cd rules && mkdir build && cd build
   cmake .. && cmake --build . --config Release
   ```

2. Copy plugin to your UE5 project:
   ```bash
   cp -r unreal/Plugins/MadCraps <YourProject>/Plugins/
   ```

3. Regenerate project files and open in Unreal Editor

4. Create a Blueprint and use the MadCraps Rules Library functions

## Blueprint API

- **Create Game State** → Initialize a new game session
- **Set Table Config** → Configure Vegas rules, payouts, commissions
- **Roll Dice** → Generate a roll (1-6 per die)
- **Resolve Bets On Roll** → Calculate payouts for all active bets
- **Calculate House Edge** → Simulate house edge for validation

## Features

✓ Complete Vegas craps rules  
✓ Configurable table settings (payouts, commissions, odds)  
✓ Pass/Don't Pass, Come/Don't Come, Field, Place, Buy, Lay, Hardways, Props  
✓ Blueprint & C++ integration  
✓ Multi-platform (Windows, Mac, Linux, iOS, Android)  
✓ Multiplayer-ready with authoritative server backend  

## Platforms

- Windows 64-bit
- Mac (Intel & Apple Silicon)
- Linux
- iOS
- Android
