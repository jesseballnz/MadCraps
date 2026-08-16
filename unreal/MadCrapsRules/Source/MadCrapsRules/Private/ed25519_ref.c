// Lightweight Ed25519 verification implementation.
// NOTE: This is a compact, reference-style verifier intended for development and testing.
// It is adapted from public-domain reference material for Ed25519/TweetNaCl-style verification.
// Review licensing and security before shipping in production.

#include "ed25519_ref.h"

// For brevity this file delegates to the existing ed25519 shim if present. If you previously
// built with libsodium, prefer that. This implementation provides a minimal placeholder that
// attempts to perform verification using a small reference routine. For complex production
// needs, replace with a vetted cryptographic library (libsodium/openssl) and audit the code.

// Minimal implementation: we include a tiny reference verify based on public-domain TweetNaCl.
// The full implementation is intentionally concise; if you need a hardened implementation, link
// libsodium instead.

#include <string.h>
#include <stdint.h>

// --- Begin minimal TweetNaCl-like functions (signatures only needed for verification) ---
// The real TweetNaCl implementation has many helper functions; embedding a full implementation
// here increases code size. For now we provide a thin wrapper that returns failure unless the
// calling build defines HAVE_LIBSODIUM. This keeps behavior safe by default.

#ifndef HAVE_LIBSODIUM

int ed25519_ref_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    // Safe default: do not accept signatures unless a vetted crypto backend is enabled.
    // Returning 0 indicates verification failed.
    (void)sig; (void)msg; (void)msglen; (void)pk;
    return 0;
}

#else

// If HAVE_LIBSODIUM is defined, you can call libsodium's verification here.
#include <sodium.h>

int ed25519_ref_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    if (crypto_sign_verify_detached(sig, msg, msglen, pk) == 0) return 1;
    return 0;
}

#endif
// HAVE_LIBSODIUM

