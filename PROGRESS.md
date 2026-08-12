Progress notes and next steps

I added a large_simulator tool and a GitHub Actions workflow to build the rules library, run unit tests, and produce a simulation artifact. When the CI workflow runs it will upload simulation_results.csv and simulation_report.md as artifacts.

Next I will:
- Finish remaining bet rules edge-cases and rounding conventions
- Add an Unreal plugin wrapper and minimal UE5 project scaffold
- Implement the Rust server prototype and a simple client example

You don't need to do anything — I'll push the next commits into scaffold/prototype as they are ready.
