# MadCraps

This repository will contain MadCraps — a 3D, Vegas-accurate craps game built with Unreal Engine 5. You gave full permission to push into this repo and selected:

- Engine: Unreal Engine 5 (Nanite + Lumen)
- Platforms: Multiplayer and single-player (multiplayer planned with authoritative server)
- Table rules: Canonical Vegas rules (configurable per-table)
- Art: mix of custom and marketplace assets

This branch (scaffold/prototype) contains the initial scaffold for the rules engine prototype, design doc, and build scaffold. It does not include any Unreal binary project files — instead it includes an Unreal plugin scaffold and instructions to integrate the rules engine as a native plugin.

What's in this commit

- README.md (this file)
- DESIGN.md (architecture, rules, table-config example)
- rules/ : C++ rules-engine prototype (header + implementation)
- rules/tools/simulator.cpp : small statistical simulator for validating house edge
- rules/tests/ : simple test harness (CMake) verifying core payouts and behavior
- unreal/PLUGIN_README.md : instructions for adding the rules engine as an Unreal plugin
- .gitignore

Next steps

If this looks good I will:
- Add more bet types and complete unit tests for every Vegas bet (Place, Buy, Lay, Hardways, Props, Horn, etc.)
- Implement an Unreal plugin wrapper that exposes the rules engine to Blueprints and the UE gameplay framework
- Create a polished table scene in an Unreal project (using marketplace assets as needed)
- Implement server-authoritative multiplayer services (node/actix or C#/ASP.NET Core) for RNG and betting state

If you want anything adjusted in the table rules (Field 12 payout, Buy commission, etc.) tell me and I'll update the default table config and tests.
