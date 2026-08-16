// Native verification implementation: calls into the ed25519 shim in ThirdParty.
#include "CoreMinimal.h"
#include "ed25519.h"

extern "C" bool NativeVerifyEd25519(const uint8* pubkey, int pubkey_len, const uint8* sig, int sig_len, const uint8* msg, int msg_len)
{
    if (pubkey == nullptr || sig == nullptr || msg == nullptr) return false;
    if (pubkey_len != 32) return false; // Ed25519 public key length
    if (sig_len != 64) return false;    // Ed25519 signature length

    int ok = ed25519_verify(sig, msg, (size_t)msg_len, pubkey);
    return ok == 1;
}
