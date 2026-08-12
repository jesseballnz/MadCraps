# MadCraps Progress

**Current Phase:** B (Rules Engine Completion) → A (UE5 3D Table)

## Active Work (Sub-Agents)
Three sub-agents are running in parallel:

- **rules-come-dontcome**: Fixing Come/Don't Come independent point handling
- **rules-proposition-bets**: Implementing full Horn (30:1/15:1), Yo, and Ace-Deuce with correct Vegas payouts
- **rules-validation**: Expanding large_simulator and generating comprehensive house-edge validation report

## Goals
- Full set of common Vegas bets with accurate payouts
- Statistical validation (millions of trials)
- Then move to UE5 plugin + playable 3D table with dice rolling, sound, and atmosphere

## Quick Playable Stopgap
`madcraps_cli.py` exists as a working command-line version while the full engine is completed.

User wants the real experience: Vegas rules + 3D table + dice + sound. No half measures.