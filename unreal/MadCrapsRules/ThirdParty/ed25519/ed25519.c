#include "ed25519.h"

// Simple implementation that attempts to use libsodium if available. If not, this provides a placeholder
// that always returns 0 (verification failed). Replace with a real implementation for production.

#ifdef HAVE_LIBSODIUM
#include <sodium.h>

int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    // libsodium uses crypto_sign_verify_detached
    if (crypto_sign_verify_detached(sig, msg, msglen, pk) == 0) return 1;
    return 0;
}

#else

// Fallback stub (safe default: reject signatures). This prevents clients from accidentally trusting invalid tokens.
int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    (void)sig; (void)msg; (void)msglen; (void)pk;
    return 0;
}

#endif
