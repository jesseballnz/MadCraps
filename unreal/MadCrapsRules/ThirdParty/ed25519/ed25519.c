#include "ed25519.h"

// Implementation order of preference:
// 1) OpenSSL (if HAVE_OPENSSL)
// 2) libsodium (if HAVE_LIBSODIUM)
// 3) Fallback: reject signatures (safe default)

#ifdef HAVE_OPENSSL

#include <openssl/evp.h>
#include <string.h>

int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    if (!sig || !msg || !pk) return 0;

    // Create an EVP_PKEY from the raw public key bytes
    EVP_PKEY *pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, NULL, pk, 32);
    if (!pkey) return 0;

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx)
    {
        EVP_PKEY_free(pkey);
        return 0;
    }

    int ok = 0;
    if (EVP_PKEY_verify_init(ctx) <= 0)
    {
        ok = 0;
        goto done;
    }

    // EVP_PKEY_verify returns 1 on success
    if (EVP_PKEY_verify(ctx, sig, 64, msg, msglen) == 1)
        ok = 1;

done:
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return ok;
}

#elif defined(HAVE_LIBSODIUM)

#include <sodium.h>

int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk)
{
    if (!sig || !msg || !pk) return 0;
    if (sodium_init() < 0) return 0;
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
