# MadCraps Server (Rust)

Minimal authoritative backend stub for MadCraps multiplayer.

## What it does
- Exposes `GET /health` for a simple readiness check
- Accepts `POST /bets` with a player id, wager type, and amount
- Rolls two server-side dice and returns the authoritative result
- Signs each roll payload with HMAC-SHA256 so clients can verify the response came from the server
- Keeps accepted bets in memory as a temporary stub audit log

## Stack
- Tokio runtime
- Axum HTTP server
- HMAC-SHA256 signatures

## Run
```bash
cargo run
```

Optionally provide a stable signing key:

```bash
MADCRAPS_SIGNING_KEY=dev-secret cargo run
```

## Example request
```bash
curl -X POST http://127.0.0.1:8080/bets \
  -H 'content-type: application/json' \
  -d '{
    "player_id": "player-123",
    "amount": 25,
    "wager": "pass_line"
  }'
```

## Notes
- The current outcome resolution is intentionally minimal and only covers a few common craps bet types.
- Bets and audit data are stored in memory only.
- This is a stub meant to be replaced with real session handling, balance checks, persistence, and full table-state management.
