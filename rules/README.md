# Rules Engine Prototype

This folder contains a small C++ rules engine prototype that implements core craps mechanics and a small simulator for validating house edge. The code is intentionally small and self-contained so it can be built as a library and integrated into an Unreal plugin later.

Build (Unix-like / CMake):

mkdir build && cd build
cmake ..
cmake --build .

Production build + package (all platforms via CPack):

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
cd build && cpack -C Release

Generated artifacts are written to `rules/build/` with deterministic names:

- Windows: `madcraps-rules-0.1.0-Windows-<arch>.exe` (NSIS installer)
- macOS: `madcraps-rules-0.1.0-Darwin-<arch>.dmg` (installable DMG)
- Linux: `madcraps-rules-0.1.0-Linux-<arch>.deb`, `.rpm`, `.tar.gz`

Unsigned artifacts are produced by default. To distribute signed installers in CI, add platform signing credentials/secrets and corresponding signing steps:

- Windows: code-signing certificate + private key/password
- macOS: Apple Developer ID certificate + notarization credentials
- Linux: repository/distribution-specific signing keys (if publishing package repos)

Warning: this is a prototype. It implements Pass Line, Come-out resolution, Field, and a simple payout calculator for common bets. We'll expand it to cover every proposition and buy/lay behavior next.
