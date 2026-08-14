MadCrapsRules Unreal plugin skeleton

This directory contains a minimal Unreal Engine 5 plugin module that provides a Blueprint-callable bridge to the authoritative MadCraps rules engine.

Design choices (default)
- Consumption method: prebuilt static library (ThirdParty) — place compiled artifacts in unreal/MadCrapsRules/ThirdParty/madcraps_rules/{include,lib}
- Blueprint access: UBlueprintFunctionLibrary provides ExecuteRoll and VerifySignedRoll stubs for prototyping.

What the skeleton includes
- MadCrapsRules.Build.cs: Module build file with notes for linking a ThirdParty static library.
- Module entry files (Public/Private) with Startup/Shutdown hooks.
- RulesBridge (Public/Private): Blueprint-callable API and prototype implementations.

How to use
1. Copy or build the rules engine static library for your platform into:
   unreal/MadCrapsRules/ThirdParty/madcraps_rules/lib/<Platform>/
   and headers into:
   unreal/MadCrapsRules/ThirdParty/madcraps_rules/include/

2. Edit MadCrapsRules.Build.cs to add PublicIncludePaths and PublicAdditionalLibraries entries for each platform you support. Example (Win64):
   PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "madcraps_rules", "include"));
   PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "madcraps_rules", "lib", "Win64", "madcraps_rules.lib"));

3. In your UE5 project, copy the MadCrapsRules plugin folder into <YourProject>/Plugins/ and open the project in the Editor. Build the project (or the plugin) using your IDE/UnrealBuildTool.

4. Use the Blueprint node "ExecuteRoll" in Blueprints (or call from C++) to test. For production, ensure server executes rolls and signs the results; clients should call VerifySignedRoll before accepting outcomes.

Next steps we can take
- Implement native bindings (C API) and call into the real rules engine from RulesBridge.
- Add platform-specific linking logic to Build.cs and provide prebuilt binaries in ThirdParty.
- Add example level and Blueprint test assets (dice actor, simple UI).
