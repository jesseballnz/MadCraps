# MadCraps Unreal plugin integration notes

This directory contains instructions and a scaffold for integrating the rules engine as a native Unreal plugin.

Steps to integrate:
1. Create a new Unreal plugin (C++ - Runtime) inside your UE5 project: "MadCrapsRules".
2. Add the compiled static library (madcraps_rules) to the plugin's ThirdParty folder or compile the rules source directly as part of the plugin module.
3. Expose a thin UCLASS wrapper that calls into the rules engine functions and dispatches results to Blueprints.
4. Use the rules engine for authoritative resolution. For multiplayer, call server RPCs that execute rules on the server and replicate signed roll tokens to clients for visuals.

Notes:
- For deterministic replay and auditing, the server should seed the RNG with a per-round nonce and sign the resulting roll with the server's private key. Clients validate the signature before accepting the visual result.
- The visualization should animate dice but snap to the authoritative outcome at the end. This prevents desyncs while preserving nice visuals.
