OpenSSL and libsodium Build.cs examples

Below are snippets you can copy into MadCrapsRules.Build.cs to enable the embedded Ed25519 shim to use OpenSSL or libsodium as the crypto backend. Place platform-specific libs in ThirdParty/<libname>/lib/<Platform>/ and headers in ThirdParty/<libname>/include/.

Windows (MSVC) — OpenSSL (recommended):

    // OpenSSL (Windows MSVC)
    PublicDefinitions.Add("HAVE_OPENSSL=1");
    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "openssl", "include"));
    // Add the path to libcrypto.lib for Win64
    PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "openssl", "lib", "Win64", "libcrypto.lib"));

Linux — OpenSSL (system):

    // OpenSSL (Linux)
    PublicDefinitions.Add("HAVE_OPENSSL=1");
    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "openssl", "include"));
    // Link against system libcrypto — the linker on your machine should find it
    PublicAdditionalLibraries.Add("crypto");

macOS — OpenSSL (Homebrew):

    // OpenSSL (macOS)
    PublicDefinitions.Add("HAVE_OPENSSL=1");
    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "openssl", "include"));
    PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "openssl", "lib", "libcrypto.a"));

Alternative: libsodium (example Windows snippet):

    // libsodium (Windows)
    PublicDefinitions.Add("HAVE_LIBSODIUM=1");
    PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "libsodium", "include"));
    PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "libsodium", "lib", "Win64", "libsodium.lib"));

Notes
- Make sure the include/lib paths match where you place the native artifacts under unreal/MadCrapsRules/ThirdParty/.
- For Linux and macOS you may prefer to install system packages (e.g., libsodium-dev or openssl) and reference system libraries instead of bundling static libs.
- After updating Build.cs, regenerate project files and rebuild the plugin.
