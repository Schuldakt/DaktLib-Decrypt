# DaktLib-Decrypt

CryEngine model decrypt/inspect library focused on identifying chunk layouts, magic numbers, crypto/compression variants, and CRC/hash validation. Modular registries let you plug in new formats and codecs without touching the core orchestrator. Dependency-free C++23, designed for deterministic, offline analysis.

## Status
- Pre-alpha scaffolding: directory/module layout, CMake setup, and docs skeleton are in place.
- Interfaces, registries, detection logic, and built-in codecs are stubbed; implementation work is tracked in [TODO.md](TODO.md).

## Goals
- Safe, read-only inspection of CryEngine model files (no code execution/injection).
- Pluggable registries for magic, chunk layouts, crypto, compression, and hashers.
- Deterministic outputs for the same inputs; binary-friendly (file or memory buffers).
- ClangSharp-friendly C API for downstream consumers.

See [AGENT.md](AGENT.md) for the concise brief and [ARCHITECTURE.md](ARCHITECTURE.md) for the module breakdown.

## Project Layout
```
include/dakt/decrypt/
  Decrypt.hpp              # umbrella include
  core/                    # Context, Inspector
  detection/               # MagicDetector, ChunkDetector
  chunk/                   # ChunkHeader/Info/Plan types
  registry/                # Magic/Chunk/Crypto/Compression registries
  crypto/                  # ICrypto + built-ins (XorSimple, RC4Lite, AESStub)
  compression/             # ICompressor + built-ins (None, Lz4Lite, XMemStub)
  hash/                    # IHasher + CRC32
  io/                      # IInput + File/Memory inputs
  results/                 # DetectionResult, ChunkReport, FileReport
  api/                     # C API header
  Builtins.hpp             # default registration helpers
src/
  mirrors include/         # per-module implementation stubs
CMakeLists.txt             # C++23, cross-platform build
```

## Building
Requirements: CMake >= 4.2.1 and a C++23 compiler (MSVC 19.3x, Clang 16+, or GCC 13+). No external dependencies.

Example (Release):
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Notes:
- Windows: use the Visual Studio or Ninja generator; `/permissive-` and `/W4` are enabled by default.
- Linux/macOS: `-Wall -Wextra -Wpedantic` are enabled by default; PIC is on.
- Outputs: static library `daktdecrypt` plus `compile_commands.json` in the build directory.

## Roadmap
Active tasks and milestones live in [TODO.md](TODO.md). Near-term focus:
- Implement core data types and registries.
- Add magic/chunk detection and scoring.
- Finish built-in crypto/compression/hash implementations and wire the inspector pipeline.

## Extending
- Add new modules by dropping headers/sources beside the existing ones (e.g., `include/dakt/decrypt/crypto/MyAlgo.hpp` + `src/crypto/MyAlgo.cpp`).
- Register them via `register_builtins` or a caller-provided registration hook in `Context`.

## License
TBD.
