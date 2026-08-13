# MadCraps Marketplace + Fab Asset Recommendations

This document is a practical sourcing guide for turning the current `MadCrapsTableActor` blockout into a polished UE5 casino table scene.

It is organized around the assets that matter most for the first convincing playtest:

- dice
- chips
- felt and table materials
- casino environment
- audio

The goal is not to find one perfect mega-pack. The better strategy for MadCraps is to combine:

- one strong hero environment pack
- one or two focused table prop packs
- a dedicated dice/chip audio source
- custom table layout textures and game-specific polish on top

Because Fab listing pages can change, treat prices, ratings, and availability as needing a quick re-check before purchase.

## Project Fit

MadCraps already has:

- a UE5-native table prototype
- plugin-side audio cue slots for dice, chips, ambience, and dealer VO
- a stated art direction of "mix of custom and marketplace assets"

That means marketplace assets should be used to accelerate scene quality, not to lock core gameplay or rules logic into third-party content.

## Recommended Buying Strategy

### Best value sequence

1. Buy or adopt one casino environment pack for the room shell and supporting props.
2. Buy one focused dice/chips prop source if the environment pack does not include close-up quality table pieces.
3. Buy one dedicated audio pack for casino foley and ambience.
4. Build the craps felt layout, chip denominations, puck, and table markings as MadCraps-specific content.

### What should stay custom

- craps betting layout texture/decal set
- puck / ON-OFF marker
- denomination logic and color system
- dealer callout integration
- gameplay Blueprints and dice authority flow

## Dice

### Primary Fab candidates

1. `Casino Decoration Dice | Virtual Casino Asset Pack`
   - URL: `https://www.fab.com/listings/a16c1ca7-aa4a-40b8-9b9c-638f582829b0`
   - Best use: background and medium-shot casino dressing.
   - Why it fits: casino-specific presentation, PBR materials, useful if you want scene-consistent prop styling.
   - Risk: may read more like a decorative prop than a physically tuned hero gameplay die.

2. `Dice`
   - URL: `https://www.fab.com/listings/8410e9a7-0539-418a-82dd-9c90b4a10187`
   - Best use: gameplay-facing dice if the included colliders and mesh variants are clean.
   - Why it fits: likely the best Fab candidate for rollable dice because search metadata indicates collider support and multiple poly levels.
   - Risk: may need material cleanup or custom pip spacing to feel casino-authentic in close-up shots.

3. `Dice Pack`
   - URL: `https://www.fab.com/listings/b6c8217f-a6fb-4e48-8e19-f3ca9a7186cc`
   - Best use: prototype acceleration if you want a ready-made rolling Blueprint to inspect.
   - Why it fits: useful reference implementation for visual dice behavior.
   - Risk: MadCraps already has authoritative roll logic, so third-party dice Blueprints should be treated as reference only, not as gameplay source of truth.

### Dice verdict

For MadCraps, the safest route is:

- use a plain, physically usable dice mesh pack for actual rolling
- keep decorative casino dice assets only for environment dressing

If none of the Fab dice are sharp enough in close-up, commission or build a custom casino die mesh. Dice are hero props in craps, so this is one of the few places where custom work has outsized value.

## Chips

### Primary Fab candidates

1. `Casino chip stacks 01`
   - URL: `https://www.fab.com/listings/83083d41-e72a-41e1-8195-888bfb766af6`
   - Best use: pre-stacked chip dressing around rail positions and betting examples.
   - Why it fits: likely the fastest way to upgrade the current cylinder chip stacks into something that reads as a real casino table.
   - Risk: pre-stacked meshes are great for dressing, but not enough by themselves for interactive betting.

2. `Poker_Table`
   - URL: `https://www.fab.com/listings/1f4d1eb4-5005-493c-aceb-e391958cdf32`
   - Best use: salvage chips, cards, and secondary table props if quality is high enough.
   - Why it fits: useful as a component source rather than as a direct poker-table drop-in.
   - Risk: the table itself is likely the wrong layout for craps, so buy it only if the chip assets are reusable and well-textured.

### Chip verdict

MadCraps should not depend on a poker table asset for the main table geometry, but it can absolutely cannibalize:

- chip meshes
- chip stacks
- tray props
- small felt-adjacent accessories

The ideal production setup is:

- one interactive single-chip mesh
- a few short stack variants
- a few tall stack variants
- denomination-specific material instances

If the Fab chips are only average, custom chips are still affordable to create compared with a full custom environment.

## Felt And Table Materials

There was no obvious Fab search result that looked like a dedicated craps felt package, so this should be treated as a custom-plus-marketplace hybrid.

### Recommendation

Build the craps surface as custom MadCraps content:

- one high-resolution betting layout texture or decal atlas
- one felt material with directional fiber response
- one edge wear variation
- one clean version and one slightly used version

Use marketplace assets for the surrounding table parts:

- wood base
- padded rail
- brass or chrome trim
- cup holders
- trays
- stanchions and decor

### Supporting Fab candidates

1. `Gambling Table`
   - URL: `https://www.fab.com/listings/3549d6f4-33fb-43ba-bc0e-74e33ffaeeff`
   - Best use: secondary reference for proportions, rail treatment, or re-usable trim pieces.
   - Risk: table topology may not match casino craps dimensions.

2. `Casino Decoration Dice | Virtual Casino Asset Pack`
   - Use as part of a broader casino prop series if you decide to collect matching decor items from the same seller.

### Felt verdict

Do not try to buy the identity of the MadCraps table from a generic asset pack. The felt and printed layout are where the game becomes recognizable and ownable.

## Casino Environment

### Strong Fab candidates

1. `Casino Environment`
   - Seller: `CGStuff Studios`
   - Seller URL: `https://www.fab.com/sellers/CGStuff%20Studios`
   - Best use: main hero room if the listing still contains a complete casino floor set.
   - Why it fits: strongest direct match for the actual target fantasy, which is a believable casino interior around a craps table.
   - Risk: verify visual quality, modularity, and whether the mature-content flag is a real blocker for your pipeline or simply a marketplace classification.

2. `Realistic Bar and Casino Props Pack`
   - URL: `https://www.fab.com/listings/a7d2c1ba-6124-4591-8b8e-72369316e5bc`
   - Best use: support props around the table area.
   - Why it fits: likely valuable for stools, bar props, glassware, chips-adjacent clutter, and upscale room dressing.
   - Risk: probably not enough on its own to create a full casino floor shell.

3. `Casino FREE - Low Poly 3D Models Pack`
   - URL: `https://www.fab.com/listings/af85242e-e50a-4188-bd59-e25873ff1ec1`
   - Best use: placeholder environment for rapid iteration or distant background dressing.
   - Why it fits: free and fast for layout experiments.
   - Risk: low-poly styling is likely below the bar for MadCraps' intended Vegas look.

4. `Miami Club Megapack`
   - URL: `https://www.fab.com/listings/914d3775-df6f-4e0a-ad2a-1fbabdef6f37`
   - Best use: premium lighting mood, nightclub adjacency, and luxury trim pieces.
   - Why it fits: useful if MadCraps wants a flashy modern casino vibe rather than a strict classic floor.
   - Risk: may skew too nightclub and require restraint to avoid making the space feel like a lounge instead of a gaming pit.

### Environment verdict

Best production approach:

- use `Casino Environment` or another direct casino interior as the base shell
- supplement with `Realistic Bar and Casino Props Pack`
- avoid low-poly packs except for prototype dressing

If the base environment pack is too opinionated, buy a modular upscale interior pack and add casino props on top. That usually ages better than forcing gameplay into a rigid premade casino layout.

## Audio

MadCraps already has a good sound-slot design in [SOUND_DESIGN.md](/Users/jesseball/.openclaw/workspace/MadCraps/unreal/SOUND_DESIGN.md:1), so the audio sourcing goal is to fill those slots with layered assets rather than one giant undifferentiated pack.

### Fab audio candidates

1. `Casino Sound Effects 2`
   - URL: `https://www.fab.com/listings/6b6277df-3993-4fe4-bde7-d33908bb346d`
   - Best use: general casino bed, slot spill, and utility casino UI/audio sweeteners.
   - Why it fits: the search metadata points to a dedicated casino pack with hundreds of effects.
   - Risk: likely stronger on slots and broad casino color than on tactile close-mic craps foley.

2. `Ultimate Casino, Club & Arcade Game Sound Effects Pack`
   - URL: `https://www.fab.com/listings/32c1d3a4-2bfd-4380-8433-830413ca0e0c`
   - Best use: additional atmosphere, UI accents, and room color.
   - Why it fits: broad coverage if you want one purchase to cover many secondary sounds.
   - Risk: could be too arcadey if used as the primary source for a grounded table sim.

3. `Advanced Game Sound Effects`
   - URL: `https://www.fab.com/listings/d3d34674-3227-4e17-aa85-d3d41f83bc49`
   - Best use: gap-filler library for general game sounds and supplemental casino cues.
   - Risk: breadth over specificity.

4. `Dice Roll`
   - URL: `https://www.fab.com/listings/c19156d5-b818-4328-8fb0-367ed4726a55`
   - Best use: direct support for `DiceRollStart`, `DiceBounce`, and `DiceSettle`.
   - Why it fits: a specialized dice source is more useful for MadCraps than another generic casino pack.
   - Risk: verify whether the recordings are on wood only or if they also give enough felt-and-rail texture for craps.

### Non-Fab audio marketplaces worth considering

1. `344 Audio - Complete Casino`
   - URLs:
     - `https://www.asoundeffect.com/sound-library/complete-casino/`
     - `https://www.344sfx.com/product/complete-casino`
   - Best use: premium all-around casino ambience and detailed foley.
   - Why it fits: strongest dedicated non-Fab recommendation for realistic casino layering.

2. `Sonniss - The Complete Casino Sound Effects`
   - URL: `https://sonniss.com/sound-effects/the-complete-casino-sound-effects/`
   - Best use: pro-level casino ambience, machine spill, and table-side support layers.
   - Why it fits: Sonniss is often a better source than game-asset marketplaces for high-quality raw SFX.

3. `Splice - Field & Foley: Casino Essentials`
   - URL: `https://splice.com/sounds/packs/field-foley/casino-essentials/samples`
   - Best use: close-up chip, card, and dice detail layers.
   - Why it fits: ideal companion pack when a marketplace bundle gives ambience but not enough tactile table detail.

4. `OpenGameArt - 54 Casino Sound Effects (Kenney)`
   - URL: `https://opengameart.org/content/54-casino-sound-effects-cards-dice-chips`
   - Best use: placeholder or supplement.
   - Why it fits: fast, free, good enough for early implementation and cue routing.
   - Risk: not likely to be the final-quality source for a premium-feeling shipped table.

### Audio verdict

Best overall audio stack for MadCraps:

- one dedicated casino ambience/casino utility pack
- one dedicated close-mic foley source for dice and chips
- optional custom dealer VO later

Recommended pairing:

- Fab: `Casino Sound Effects 2`
- Plus either `Dice Roll` or `Splice - Casino Essentials`
- Plus later custom or recorded dealer calls

## Final Shortlist By Category

### Best overall picks

- Dice gameplay: `Dice` (`8410e9a7-0539-418a-82dd-9c90b4a10187`)
- Dice decor: `Casino Decoration Dice | Virtual Casino Asset Pack`
- Chips: `Casino chip stacks 01`
- Environment base: `Casino Environment` by `CGStuff Studios`
- Environment support props: `Realistic Bar and Casino Props Pack`
- Audio base: `Casino Sound Effects 2`
- Audio detail layer: `Dice Roll` or `Splice - Casino Essentials`

### Best budget-first picks

- Environment placeholder: `Casino FREE - Low Poly 3D Models Pack`
- Audio placeholder: `OpenGameArt - 54 Casino Sound Effects`
- Chips/dice: use free or low-cost Fab props while keeping the felt and table layout custom

### Best quality-first picks

- Environment: direct casino interior pack plus supplemental realistic props
- Dice: custom hero dice mesh if Fab options do not hold up in close camera shots
- Audio: `344 Audio - Complete Casino` or Sonniss casino library plus a dedicated dice/chips detail source

## What To Buy First

If only buying three things right now, buy in this order:

1. `Casino Environment` or equivalent direct casino room pack
2. `Casino chip stacks 01`
3. `Casino Sound Effects 2`

Then build:

- the actual craps felt layout
- denomination materials
- puck and dealer accessories
- final dice presentation

## Integration Notes For The Current UE5 Prototype

### Immediate art swaps

- Replace primitive chip cylinders with imported chip stacks and single chips.
- Replace basic felt color with a proper felt material and custom printed layout.
- Dress the perimeter with stools, stanchions, side tables, bar props, and soft lighting.

### Immediate audio swaps

- Map imported sounds into `UMadCrapsSoundProfile`.
- Populate `DiceBounce` with multiple short variants.
- Keep ambience subtle and route it through a dedicated bus for ducking under dealer VO.

### Avoid

- Buying a full poker-table asset and forcing it to become a craps table.
- Using slot-machine-heavy audio as the main dice/chip source.
- Letting third-party Blueprints override MadCraps authoritative roll logic.

## Recommended Next Asset Pass

After these purchases, the next custom content should be:

1. hero craps felt texture set
2. casino denomination chip material set
3. puck / ON-OFF marker
4. dealer stick, rail details, chip tray, and table accessories
5. recorded or directed dealer callouts

That combination gives the fastest path from prototype to a table that feels recognizably like a polished Vegas craps experience.
