This directory contains helper keys for the server.

To generate a key (Ed25519) and place it here for the server to use:

1. Use the server binary to auto-generate a key on first run (it will create server/keys/priv.key automatically).
2. Or generate externally and place the base64-encoded 64-byte keypair into server/keys/priv.key.

Note: The key file should contain the base64 encoding of the 64-byte ed25519 keypair produced by ed25519-dalek's `Keypair::to_bytes()`.
Keep this file secure in production (use vault/HSM). This is only for local testing.
