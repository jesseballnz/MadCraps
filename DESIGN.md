MadCraps DESIGN

Goal
- Build a visually high-fidelity, rules-accurate craps game inspired by Las Vegas using Unreal Engine 5.
- Separate an authoritative, fully-tested rules engine (native C++) from the engine presentation layer.

High-level architecture

1) Rules Engine (native C++ library)
   - Deterministic PRNG with seeded option for reproducible simulations and server-signed randomness for multiplayer.
   - Full bet model (data-driven): Pass/Don't Pass, Come/Don't Come, Odds, Place/Buy/Lay, Field, Big 6/8, Hardways, Proposition bets, Horn, etc.
   - TableConfig JSON defines payouts, max/min bets, odds limits, commission settings.
   - Exposes a small set of functions:
      - void setTableConfig(TableConfig)
      - GameState startRound()
      - RollResult resolveRoll(GameState, DiceRoll)
      - Payouts calculatePayouts(GameState)
   - Unit tests for every bet type and statistical simulator harness.

2) Engine integration (Unreal plugin)
   - Native plugin exposing rules API to Blueprints and C++ gameplay code.
   - Handles UI, table layout, chips, dealer/NPC, and dice visualization.
   - Visual dice: optionally use Unreal physics to animate dice while the authoritative roll is determined by the rules engine (visualization follows authoritative result).

3) Multiplayer backend (optional)
   - Authoritative server for bets and RNG. Server returns signed roll tokens for auditability.
   - Persistence for audit logs, player balances, anti-cheat.

Table rules and canonical Vegas defaults (configurable)

- Pass Line / Come: 1:1
- Odds:
  - Point 4/10: 2:1
  - Point 5/9: 3:2
  - Point 6/8: 6:5
- Place bets to win:
  - 6/8: pays 7:6
  - 5/9: pays 7:5
  - 4/10: pays 9:5
- Field: 2 pays 2:1, 12 pays 3:1 by default (configurable to 2:1)
- Big 6/8: pays 1:1
- Hardways: typical payouts (configurable per-table)
- Commission on Buy bets: default 5% on wins (configurable)

Data-driven example (table_config.json)
{
  "name": "Vegas Classic",
  "field": { "two": 2.0, "twelve": 3.0 },
  "odds": { "4": 2.0, "5": 1.5, "6": 1.2, "8": 1.2, "9": 1.5, "10": 2.0 },
  "place_payouts": { "4": 1.8, "5": 1.4, "6": 1.1666667, "8": 1.1666667, "9": 1.4, "10": 1.8 },
  "buy_commission_pct": 0.05
}

Development priorities for MVP
- Canonical rules engine with exhaustive unit tests
- Working prototype table in UE5 with physics-based dice visuals mapped to authoritative rolls
- Statistical validation harness to confirm house-edge matches expected numbers


