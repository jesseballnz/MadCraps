use std::{
    env,
    net::SocketAddr,
    sync::Arc,
    time::{SystemTime, UNIX_EPOCH},
};

use axum::{
    extract::State,
    http::StatusCode,
    response::IntoResponse,
    routing::{get, post},
    Json, Router,
};
use base64::{engine::general_purpose::STANDARD_NO_PAD, Engine as _};
use hmac::{Hmac, Mac};
use rand::{rngs::OsRng, RngCore};
use serde::{Deserialize, Serialize};
use sha2::Sha256;
use tokio::sync::Mutex;
use uuid::Uuid;

type HmacSha256 = Hmac<Sha256>;

#[derive(Clone)]
struct AppState {
    signing_key: Arc<[u8]>,
    accepted_bets: Arc<Mutex<Vec<AcceptedBet>>>,
}

#[derive(Debug, Deserialize)]
struct BetRequest {
    player_id: String,
    amount: u64,
    wager: WagerKind,
}

#[derive(Debug, Deserialize, Serialize, Clone, Copy)]
#[serde(rename_all = "snake_case")]
enum WagerKind {
    PassLine,
    DontPass,
    Field,
}

#[derive(Debug, Serialize, Clone)]
struct AcceptedBet {
    bet_id: Uuid,
    player_id: String,
    amount: u64,
    wager: WagerKind,
    roll: RollResult,
}

#[derive(Debug, Serialize, Clone)]
struct RollResult {
    die_one: u8,
    die_two: u8,
    total: u8,
    rolled_at_unix_ms: u128,
    signature: String,
}

#[derive(Debug, Serialize)]
struct BetResponse {
    bet_id: Uuid,
    accepted: bool,
    outcome: &'static str,
    player_id: String,
    amount: u64,
    wager: WagerKind,
    roll: RollResult,
}

#[derive(Debug, Serialize)]
struct HealthResponse {
    status: &'static str,
    accepted_bets: usize,
}

#[tokio::main]
async fn main() {
    let signing_key = env::var("MADCRAPS_SIGNING_KEY")
        .map(|value| value.into_bytes())
        .unwrap_or_else(|_| {
            let mut bytes = [0_u8; 32];
            OsRng.fill_bytes(&mut bytes);
            bytes.to_vec()
        });

    let state = AppState {
        signing_key: Arc::from(signing_key),
        accepted_bets: Arc::new(Mutex::new(Vec::new())),
    };

    let app = Router::new()
        .route("/health", get(health))
        .route("/bets", post(place_bet))
        .with_state(state);

    let addr = SocketAddr::from(([127, 0, 0, 1], 8080));
    println!("MadCraps authoritative server listening on http://{addr}");

    let listener = tokio::net::TcpListener::bind(addr)
        .await
        .expect("failed to bind TCP listener");

    axum::serve(listener, app)
        .await
        .expect("server exited unexpectedly");
}

async fn health(State(state): State<AppState>) -> Json<HealthResponse> {
    let accepted_bets = state.accepted_bets.lock().await.len();
    Json(HealthResponse {
        status: "ok",
        accepted_bets,
    })
}

async fn place_bet(
    State(state): State<AppState>,
    Json(request): Json<BetRequest>,
) -> Result<impl IntoResponse, (StatusCode, Json<serde_json::Value>)> {
    if request.player_id.trim().is_empty() {
        return Err(validation_error("player_id must not be empty"));
    }

    if request.amount == 0 {
        return Err(validation_error("amount must be greater than zero"));
    }

    let bet_id = Uuid::new_v4();
    let roll = generate_signed_roll(&state.signing_key, bet_id);
    let outcome = resolve_stub_outcome(request.wager, roll.total);

    let accepted = AcceptedBet {
        bet_id,
        player_id: request.player_id.clone(),
        amount: request.amount,
        wager: request.wager,
        roll: roll.clone(),
    };

    state.accepted_bets.lock().await.push(accepted);

    Ok((
        StatusCode::CREATED,
        Json(BetResponse {
            bet_id,
            accepted: true,
            outcome,
            player_id: request.player_id,
            amount: request.amount,
            wager: request.wager,
            roll,
        }),
    ))
}

fn validation_error(message: &str) -> (StatusCode, Json<serde_json::Value>) {
    (
        StatusCode::BAD_REQUEST,
        Json(serde_json::json!({
            "error": message,
        })),
    )
}

fn generate_signed_roll(signing_key: &[u8], bet_id: Uuid) -> RollResult {
    let die_one = roll_die();
    let die_two = roll_die();
    let rolled_at_unix_ms = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("system clock before unix epoch")
        .as_millis();
    let total = die_one + die_two;

    let payload = format!("{bet_id}:{die_one}:{die_two}:{total}:{rolled_at_unix_ms}");

    let mut mac = HmacSha256::new_from_slice(signing_key)
        .expect("signing key should always be valid for HMAC");
    mac.update(payload.as_bytes());
    let signature = STANDARD_NO_PAD.encode(mac.finalize().into_bytes());

    RollResult {
        die_one,
        die_two,
        total,
        rolled_at_unix_ms,
        signature,
    }
}

fn roll_die() -> u8 {
    ((OsRng.next_u32() % 6) + 1) as u8
}

fn resolve_stub_outcome(wager: WagerKind, total: u8) -> &'static str {
    match wager {
        WagerKind::PassLine => match total {
            7 | 11 => "win",
            2 | 3 | 12 => "lose",
            _ => "point_established",
        },
        WagerKind::DontPass => match total {
            2 | 3 => "win",
            7 | 11 => "lose",
            12 => "push",
            _ => "point_established",
        },
        WagerKind::Field => match total {
            2 | 3 | 4 | 9 | 10 | 11 | 12 => "win",
            _ => "lose",
        },
    }
}

