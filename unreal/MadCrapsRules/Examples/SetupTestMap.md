# Test Level and Blueprint setup instructions

This document explains how to create a minimal test map and run the plugin's TestRoller flow. The repository includes a Python script to automate map creation inside the Unreal Editor, but you can also create the level manually.

Automatic creation (recommended):
1. Copy the plugin to your project: <YourProject>/Plugins/MadCrapsRules
2. Open the project in the Unreal Editor (compile the plugin if prompted).
3. Window → Developer Tools → Python Console, then run:
   exec(open(r"<PathToRepo>/unreal/MadCrapsRules/Examples/CreateTestLevel.py").read())
   (Adjust path for your project layout.)
4. Play the map. If TestLevelBootstrap.bAutoTrigger is enabled (true by default), it will auto-trigger a roll at BeginPlay.

Manual creation:
1. Create a new blank level (File → New Level → Empty).
2. Place an instance of TestLevelBootstrap (found under Place Actors → All Classes → TestLevelBootstrap) into the level.
3. Optionally set ServerBaseUrl on the actor to your server (default: http://127.0.0.1:3000).
4. Save the level under /Game/Maps/TestMadCrapsMap and press Play.

Notes:
- The TestRoller actor uses Unreal's HTTP module to contact the server, so your server must be reachable from the Editor host.
- The TestLevelBootstrap will spawn a DiceActor and a TestRoller and optionally auto-trigger a roll. The TestRoller verifies the signed_blob returned by the server and drives the DiceActor on successful verification.
