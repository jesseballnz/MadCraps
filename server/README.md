# MadCraps Server (Rust)

Authoritative backend for MadCraps multiplayer.

## Responsibilities
- Validate bets against current table rules
- Generate cryptographically signed roll tokens
- Resolve rounds using the authoritative rules engine
- Maintain audit logs for every roll/bet
- Simple player session & balance tracking (MVP)

## Tech
- Tokio + async runtime
- WebSocket protocol for clients
- Will eventually link or re-implement core rules logic in Rust for maximum safety

## Status
Skeleton stage. Next: basic WS server + signed roll generation.