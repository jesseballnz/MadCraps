## Client verification notes (Ed25519)

The server endpoint `/roll` returns JSON with these fields:

- `result`: the RollResult JSON (seed, dice_a, dice_b, outcome, debug_info)
- `signature`: base64-encoded Ed25519 signature over the serialized `result` bytes
- `public_key`: base64-encoded Ed25519 public key (for testing). In production, clients should obtain the server public key via a trusted channel or configuration.

To verify on the client (pseudo):

1. Decode the base64 `public_key` into bytes and construct an Ed25519 public key object.
2. Decode the base64 `signature` into bytes.
3. Serialize the `result` JSON exactly as the server did (the server uses compact JSON). For interoperability, it's recommended to verify using the serialized bytes returned by the server, not a re-serialized object on the client — i.e., the server may also return a `signed_blob` of bytes.
4. Use the public key to verify the signature against the serialized `result`.

In the Unreal plugin, implement verification using a small Ed25519 library or include a tiny verification function that accepts the server's public key and signature.
