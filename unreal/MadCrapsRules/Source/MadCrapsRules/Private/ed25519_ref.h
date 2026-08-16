#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Minimal reference verifier API used by the plugin. This file includes a compact Ed25519 verification
// implementation sufficient for client-side verification of server-signed roll blobs. It is intended
// to be self-contained (no external dependencies).

// Returns 1 when signature is valid, 0 otherwise.
int ed25519_ref_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk);

#ifdef __cplusplus
}
#endif
