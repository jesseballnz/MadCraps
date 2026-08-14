use axum::{routing::post, Json, Router, extract::State};
use ed25519_dalek::{Keypair, PublicKey, Signature, Signer};
use rand_chacha::ChaCha20Rng;
use rand_core::{RngCore, SeedableRng};
use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::{net::SocketAddr, sync::Arc};
use base64::{engine::general_purpose, Engine as _};
use tracing_subscriber;

#[derive(Clone)]
struct AppState {
    keypair: Arc<Keypair>,
}

#[derive(Deserialize)]
struct RollRequest {
    // Optional per-round nonce (string). If provided, server derives a deterministic seed from it.
    nonce: Option<String>,
    // Optional explicit seed. If provided, server will use this seed instead of nonce-derived.
    seed: Option<u64>,
}

#[derive(Serialize, Clone)]
struct RollResult {
    seed: u64,
    dice_a: u8,
    dice_b: u8,
    outcome: u8,
    debug_info: String,
}

#[derive(Serialize)]
struct RollResponse {
    result: RollResult,
    signature: String,      // base64(signature)
    public_key: String,     // base64(public_key) for client verification
}

async fn handle_roll(State(state): State<AppState>, Json(req): Json<RollRequest>) -> Json<RollResponse> {
    // Determine seed
    let seed_u64: u64 = if let Some(s) = req.seed {
        s
    } else if let Some(nonce) = req.nonce {
        // Derive seed from nonce + public key to bind nonce to server identity deterministically
        let mut hasher = Sha256::new();
        hasher.update(nonce.as_bytes());
        hasher.update(state.keypair.public.as_bytes());
        let hash = hasher.finalize();
        // Use first 8 bytes as u64 seed (little-endian)
        let mut b: [u8; 8] = [0; 8];
        b.copy_from_slice(&hash[0..8]);
        u64::from_le_bytes(b)
    } else {
        // No seed or nonce provided — use current timestamp
        let nanos = chrono::Utc::now().timestamp_nanos() as u64;
        nanos
    };

    // Use ChaCha RNG seeded by seed_u64 to generate dice
    let mut seed_bytes = [0u8; 32];
    // Expand u64 seed into 32 bytes using simple deterministic pattern
    for i in 0..4 {
        seed_bytes[i*8..(i+1)*8].copy_from_slice(&seed_u64.to_le_bytes());
    }
    let mut rng = ChaCha20Rng::from_seed(seed_bytes);
    let a = (rng.next_u32() % 6) as u8 + 1;
    let b = (rng.next_u32() % 6) as u8 + 1;
    let outcome = a + b;

    let result = RollResult {
        seed: seed_u64,
        dice_a: a,
        dice_b: b,
        outcome,
        debug_info: format!("Deterministic roll derived from seed={}", seed_u64),
    };

    // Serialize the result deterministically (compact) and sign it
    let serialized = serde_json::to_vec(&result).expect("serialize roll result");
    let signature: Signature = state.keypair.sign(&serialized);

    let sig_b64 = general_purpose::STANDARD.encode(signature.to_bytes());
    let pub_b64 = general_purpose::STANDARD.encode(state.keypair.public.as_bytes());

    let response = RollResponse {
        result,
        signature: sig_b64,
        public_key: pub_b64,
    };

    Json(response)
}

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();

    tracing::info!("MadCraps authoritative server starting...");

    // Keypair generation: in production, load this from secure storage. For example, set via env var or file.
    // Here we generate a new ephemeral keypair on startup and log the public key so clients can verify during testing.
    use rand_core::OsRng;
    let mut csprng = OsRng;
    let keypair = Keypair::generate(&mut csprng);
    let pub_b64 = base64::engine::general_purpose::STANDARD.encode(keypair.public.as_bytes());
    tracing::info!("Server Ed25519 public key: {}", pub_b64);

    let shared = AppState { keypair: Arc::new(keypair) };

    let app = Router::new()
        .route("/roll", post(handle_roll))
        .with_state(shared);

    // Bind address
    let addr = SocketAddr::from(([127, 0, 0, 1], 3000));
    tracing::info!("Listening on {}", addr);
    axum::Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}
