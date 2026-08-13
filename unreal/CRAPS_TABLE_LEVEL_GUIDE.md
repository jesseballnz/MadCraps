# UE5 Craps Table Prototype Guide

This plugin now includes `AMadCrapsTableActor`, a runtime-built table blockout that uses only Unreal Engine primitives:

- wood table base and rails
- green felt play surface
- simple betting layout boxes and labels
- chip stacks placed on both player sides

## How to use in UE5

1. Copy `MadCraps/unreal/MadCrapsRules` into your UE5 project's `Plugins/` folder.
2. Enable the plugin and restart the editor if prompted.
3. Create a blank level named `LVL_CrapsPrototype`.
4. Drag `MadCrapsTableActor` into the level.
5. Add a directional light, skylight, exponential height fog, and a rect light above the table.
6. Place a cine camera around `X=-450 Y=0 Z=220` with a slight downward tilt for preview renders.

## Recommended actor settings

- `TableSize`: `X=320 Y=180 Z=100`
- `FeltThickness`: `4`
- `RailWidth`: `18`
- `RailHeight`: `14`
- `ChipStacksPerSide`: `4`
- `ChipsPerStack`: `8`

These defaults are already set in the actor and give a quick hero-shot blockout sized for centimeters.

## Suggested marketplace art upgrades

Use these as replacement targets after the prototype reads well in-engine:

- Casino environment pack:
  Search for realistic casino interior or card room environment kits with Nanite-ready furniture, wall trims, and floor materials.
- Table accessories:
  Look for casino prop packs that include dice sticks, lammer puck, dealer apron props, chip trays, and rail cup holders.
- Character dressing:
  Use a dealer character or modular formalwear pack for staffed table presentation shots.
- Surface materials:
  Replace the dynamic basic-shape material with a proper felt shader, wood veneer, and leather or vinyl rail materials.

## Fast polish pass

- Swap text render labels for decal materials or a single betting-layout texture once the proportions are approved.
- Add point lights under warm shades above the table to get a focused casino mood.
- Replace chip cylinders with beveled chip meshes and varied denominations.
- Add dice, a stick, and mirrored chrome or brass trim for better silhouette reads.
