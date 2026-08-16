use axum::{routing::{get, post}, Json, Router, extract::State};
use ed25519_dalek::{Keypair, PublicKey, Signature, Signer, Verifier};
use rand_chacha::ChaCha20Rng;
use rand_core::{RngCore, SeedableRng};
use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::{net::SocketAddr, sync::Arc};
use base64::{engine::general_purpose, Engine as _};
use tracing_subscriber;
use std::fs;
use std::path::Path;

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
    signed_blob: String,    // compact JSON string that was signed (use this for verification)
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
    let serialized_vec = serde_json::to_vec(&result).expect("serialize roll result");
    let serialized = String::from_utf8(serialized_vec.clone()).expect("utf8");
    let signature: Signature = state.keypair.sign(&serialized_vec);

    let sig_b64 = general_purpose::STANDARD.encode(signature.to_bytes());

    let response = RollResponse {
        result,
        signature: sig_b64,
        signed_blob: serialized,
    };

    Json(response)
}

async fn handle_public_key(State(state): State<AppState>) -> String {
    // Return base64 public key for clients to use (for testing). In production, use secure distribution.
    base64::engine::general_purpose::STANDARD.encode(state.keypair.public.as_bytes())
}

fn load_or_generate_keypair(path: &Path) -> Keypair {
    if path.exists() {
        let b64 = fs::read_to_string(path).expect("read key file");
        let bytes = general_purpose::STANDARD.decode(b64.trim()).expect("decode base64 key");
        // Expect 64 bytes private key (expanded): ed25519_dalek::Keypair::from_bytes
        if bytes.len() == 64 {
            return Keypair::from_bytes(&bytes).expect("valid keypair bytes");
        }
        panic!("Invalid key file length");
    } else {
        // Generate and save
        use rand_core::OsRng;
        let mut csprng = OsRng;
        let keypair = Keypair::generate(&mut csprng);
        let bytes = keypair.to_bytes();
        let b64 = general_purpose::STANDARD.encode(bytes);
        // Ensure parent dir exists
        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent).ok();
        }
        fs::write(path, b64).expect("write key file");
        keypair
    }
}

#[tokio::main]
async fn main() {
    tracing_subscriber::fmt::init();

    tracing::info!("MadCraps authoritative server starting...");

    // Keypair: load from server/keys/priv.key (base64 of 64 byte keypair) or generate and store for testing.
    let key_path = Path::new("server/keys/priv.key");
    let keypair = load_or_generate_keypair(key_path);
    let pub_b64 = base64::engine::general_purpose::STANDARD.encode(keypair.public.as_bytes());
    tracing::info!("Server Ed25519 public key: {}", pub_b64);

    let shared = AppState { keypair: Arc::new(keypair) };

    let app = Router::new()
        .route("/roll", post(handle_roll))
        .route("/public_key", get(handle_public_key))
        .with_state(shared);

    // Bind address
    let addr = SocketAddr::from(([127, 0, 0, 1], 3000));
    tracing::info!("Listening on {}", addr);
    axum::Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}
