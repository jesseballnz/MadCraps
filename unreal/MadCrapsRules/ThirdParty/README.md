This directory is intended to hold prebuilt rules engine artifacts used by the MadCrapsRules plugin.

Expected layout:

madcraps_rules/
  include/            -> public C headers for the rules engine (e.g., madcraps.h)
  lib/
    Win64/            -> madcraps_rules.lib or libmadcraps_rules.a
    Linux/            -> libmadcraps_rules.a
    Mac/              -> libmadcraps_rules.a

Notes:
- Build the rules engine for each target platform and place the artifacts here. Update MadCrapsRules.Build.cs to point to the correct library file per platform.
- Alternatively, instead of prebuilt artifacts, add the rules engine sources to the plugin's source tree and compile them as part of the module (simpler for CI but increases compile times).
