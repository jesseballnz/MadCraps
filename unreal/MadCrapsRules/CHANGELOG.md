Add TestRoller actor (HTTP client) and update Build.cs to include HTTP module. TestRoller requests /roll, fetches /public_key, verifies signature, then drives DiceActor.
