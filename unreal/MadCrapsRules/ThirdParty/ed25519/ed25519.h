# Minimal Ed25519 verify shim
#ifndef ED25519_H
#define ED25519_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns 1 on success (signature valid), 0 on failure.
int ed25519_verify(const unsigned char *sig, const unsigned char *msg, size_t msglen, const unsigned char *pk);

#ifdef __cplusplus
}
#endif

#endif // ED25519_H
