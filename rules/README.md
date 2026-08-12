# Rules Engine Prototype

This folder contains a small C++ rules engine prototype that implements core craps mechanics and a small simulator for validating house edge. The code is intentionally small and self-contained so it can be built as a library and integrated into an Unreal plugin later.

Build (Unix-like / CMake):

mkdir build && cd build
cmake ..
cmake --build .

Warning: this is a prototype. It implements Pass Line, Come-out resolution, Field, and a simple payout calculator for common bets. We'll expand it to cover every proposition and buy/lay behavior next.
