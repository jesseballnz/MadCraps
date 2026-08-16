# Ed25519 verification (ThirdParty)

This folder contains a small Ed25519 verification shim. It prefers OpenSSL (if available at build time), then libsodium. If neither is available, the shim rejects signatures by default to avoid accepting unauthenticated data.

Files:
- ed25519.h / ed25519.c : C shim with `int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)`.

Build options
- Option 1: Link OpenSSL (recommended if you have a modern OpenSSL >=1.1.1 available). Define HAVE_OPENSSL in your Build.cs and link the OpenSSL crypto library.
  - Example Build.cs snippets (Windows):
      PublicDefinitions.Add("HAVE_OPENSSL=1");
      PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "openssl", "lib", "Win64", "libcrypto.lib"));
      PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "openssl", "include"));
  - Example Build.cs snippets (Linux/Mac):
      PublicDefinitions.Add("HAVE_OPENSSL=1");
      PublicAdditionalLibraries.Add("crypto"); // or path to libcrypto
      PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "openssl", "include"));

- Option 2: Link libsodium (alternative). Define HAVE_LIBSODIUM and link libsodium.
  - Example Build.cs snippets:
      PublicDefinitions.Add("HAVE_LIBSODIUM=1");
      PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "libsodium", "lib", "Win64", "libsodium.lib"));
      PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "libsodium", "include"));

- Option 3: No crypto library available. The shim will reject signatures by default. This is a safe fallback for development if you don't want to add native deps, but you won't be able to verify server tokens until you enable OpenSSL or libsodium.

Notes
- For production builds you should link a vetted crypto library (OpenSSL or libsodium) rather than relying on an embedded reference. The shim is intentionally conservative when no crypto backend is present.
- Ensure your platform's OpenSSL version supports EVP_PKEY_new_raw_public_key (OpenSSL 1.1.1+).
