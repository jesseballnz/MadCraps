// MadCraps Authoritative Server (Rust)
// Handles bet validation, signed RNG, round resolution using the rules engine logic,
// and audit logging for multiplayer.

use std::collections::HashMap;

#[tokio::main]
async fn main() {
    println!("MadCraps Authoritative Server starting...");

    // TODO:
    // - WebSocket server (tokio-tungstenite or axum)
    // - Per-round signed roll tokens (ed25519 or HMAC)
    // - Call into rules engine (or re-implement critical path in Rust for safety)
    // - Player session + balance management
    // - Audit log persistence

    println!("Server ready (skeleton).");
}