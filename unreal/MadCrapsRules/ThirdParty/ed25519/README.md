# Ed25519 verification (ThirdParty)

This folder contains a minimal Ed25519 verification shim. It prefers to use libsodium if available on the build machine; otherwise it provides a placeholder implementation which must be replaced with a proper verifier for production builds.

Files:
- ed25519.h / ed25519.c : small C shim with `int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)`.

Build notes:
- If you have libsodium installed and available to the Unreal build, you can modify MadCrapsRules.Build.cs to link against it and remove the placeholder implementation.
- The current setup compiles the ed25519.c shipped here into the plugin. Review licensing before shipping.
