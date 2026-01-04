# DaktLib-Decrypt Architecture

> Chunk/hash decrypt and format inspection library for CryEngine model files. Focus: identify chunk types, magic numbers, encryption/compression variants; modular for future format growth. Zero external dependencies.

## Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                      │
├─────────────────────────────────────────────────────────────┤
│                 C API (dakt_decrypt_*)                      │
├─────────────────────────────────────────────────────────────┤
│                  Decrypt/Inspect Orchestrator               │
│        (format detection, chunk catalog, strategy pick)     │
├────────────┬────────────┬────────────┬────────────┬─────────┤
│ Magic DB   │ Chunk DB   │ Crypto     │ Compression│ Hashcode│
│ (magic)    │ (layout)   │ (AES/RC4*) │ (LZ/XMem*) │ Calc/Chk│
├────────────┴────────────┴────────────┴────────────┴─────────┤
│                      IO / Buffers                           │
└─────────────────────────────────────────────────────────────┘
```

*Crypto/Compression marked with * are pluggable modules; only lightweight built-ins initially (no external libs).

## Design Goals
- Modular registries: add new formats/chunks/crypto/comp modules without touching core.
- Safe offline analysis: no execution of game code; pure data parsing.
- Deterministic outputs: stable results for the same inputs.
- Binary-friendly: works on memory buffers and file-backed streams.

## Namespace Structure

```cpp
namespace dakt::decrypt {
    // Core orchestrator
    class Inspector;        // entry point
    class Context;          // holds registries and options

    // Detection
    class MagicDetector;    // magic numbers and signatures
    class ChunkDetector;    // recognizes chunk headers/types

    // Registries
    class MagicRegistry;
    class ChunkRegistry;
    class CryptoRegistry;
    class CompressionRegistry;

    // Chunk model
    struct ChunkHeader;
    struct ChunkInfo;       // type, offset, size, flags
    struct ChunkDecodePlan; // chosen crypto/comp strategies

    // Crypto / Compression interfaces
    class ICrypto;          // decrypt
    class IHasher;          // chunk hashcode calc/check
    class ICompressor;      // decompress (if needed)

    // Built-ins (minimal, extendable)
    namespace crypto { class XorSimple; class RC4Lite; class AESStub; }
    namespace compress { class None; class Lz4Lite; class XMemStub; }
    class HashcodeCRC32;

    // IO
    class IInput;           // abstract reader
    class FileInput;
    class MemoryInput;

    // Results
    struct DetectionResult;
    struct ChunkReport;
    struct FileReport;

    // C API handles
    struct CContext;
    struct CReport;
}
```

## Directory Structure

```
DaktLib-Decrypt/
├── include/dakt/decrypt/
│   ├── Decrypt.hpp                 # umbrella include
│   ├── core/
│   │   ├── Context.hpp
│   │   └── Inspector.hpp
│   ├── detection/
│   │   ├── MagicDetector.hpp
│   │   └── ChunkDetector.hpp
│   ├── chunk/
│   │   └── Chunk.hpp               # header/info/plan types
│   ├── registry/
│   │   ├── MagicRegistry.hpp
│   │   ├── ChunkRegistry.hpp
│   │   ├── CryptoRegistry.hpp
│   │   └── CompressionRegistry.hpp
│   ├── crypto/
│   │   ├── Crypto.hpp              # interface
│   │   ├── XorSimple.hpp
│   │   ├── RC4Lite.hpp
│   │   └── AESStub.hpp
│   ├── compression/
│   │   ├── Compression.hpp         # interface
│   │   ├── None.hpp
│   │   ├── Lz4Lite.hpp
│   │   └── XMemStub.hpp
│   ├── hash/
│   │   ├── Hashcode.hpp
│   │   └── CRC32.hpp
│   ├── io/
│   │   ├── Input.hpp
│   │   ├── FileInput.hpp
│   │   └── MemoryInput.hpp
│   ├── results/
│   │   └── Results.hpp
│   ├── api/
│   │   └── c_api.h
│   └── Builtins.hpp                # registration helpers
├── src/
│   ├── core/
│   │   ├── Context.cpp
│   │   └── Inspector.cpp
│   ├── detection/
│   │   ├── MagicDetector.cpp
│   │   └── ChunkDetector.cpp
│   ├── chunk/
│   │   └── Chunk.cpp
│   ├── registry/
│   │   ├── MagicRegistry.cpp
│   │   ├── ChunkRegistry.cpp
│   │   ├── CryptoRegistry.cpp
│   │   └── CompressionRegistry.cpp
│   ├── crypto/
│   │   ├── Crypto.cpp
│   │   ├── XorSimple.cpp
│   │   ├── RC4Lite.cpp
│   │   └── AESStub.cpp
│   ├── compression/
│   │   ├── Compression.cpp
│   │   ├── None.cpp
│   │   ├── Lz4Lite.cpp
│   │   └── XMemStub.cpp
│   ├── hash/
│   │   ├── Hashcode.cpp
│   │   └── CRC32.cpp
│   ├── io/
│   │   ├── Input.cpp
│   │   ├── FileInput.cpp
│   │   └── MemoryInput.cpp
│   ├── results/
│   │   └── Results.cpp
│   ├── api/
│   │   └── c_api.cpp
│   └── Builtins.cpp
├── tests/
├── CMakeLists.txt
├── ARCHITECTURE.md
└── TODO.md
```

## Core Flow

1) **Input**: memory buffer or file stream into `Inspector`.
2) **Magic detection**: scan header/footer signatures via `MagicRegistry`.
3) **Chunk scan**: walk file to identify chunk headers (size/type/hashcode fields).
4) **Strategy pick**: for each chunk, consult registries to choose crypto/compression/hasher.
5) **Decode**: apply crypto then decompression as needed; compute/verify hashcode.
6) **Report**: emit `FileReport` containing chunk list, detected formats, mismatches, and unknowns.

## Chunk Model
- `ChunkHeader`: offset, size, type, flags, hashcode, version.
- `ChunkInfo`: header + derived info (expected type name, codec hints, endian).
- `ChunkDecodePlan`: selected crypto/compression/hasher identifiers.

## Namespace Structure (stable public surface)

```cpp
namespace dakt::decrypt {
    // Core orchestrator
    class Inspector;            // entry point
    class Context;              // owns registries/options

    // Detection
    class MagicDetector;        // magic numbers and signatures
    class ChunkDetector;        // chunk header scanning

    // Registries
    class MagicRegistry;
    class ChunkRegistry;
    class CryptoRegistry;
    class CompressionRegistry;

    // Chunk model
    struct ChunkHeader;
    struct ChunkInfo;           // type, offset, size, flags
    struct ChunkDecodePlan;     // chosen crypto/comp/hasher

    // Crypto / Compression / Hash interfaces
    class ICrypto;              // decrypt
    class IHasher;              // chunk hashcode calc/check
    class ICompressor;          // decompress (if needed)

    // Built-ins (minimal, extendable)
    namespace crypto { class XorSimple; class RC4Lite; class AESStub; }
    namespace compress { class None; class Lz4Lite; class XMemStub; }
    namespace hash { class CRC32; }

    // IO
    class IInput;               // abstract reader
    class FileInput;
    class MemoryInput;

    // Results
    struct DetectionResult;
    struct ChunkReport;
    struct FileReport;

    // C API handles
    struct CContext;
    struct CReport;
}
```

### Modularity Notes
- Each interface (crypto/compression/hash/io/registry) lives in its own subdirectory with its built-ins alongside, making it easy to add new implementations without touching core orchestrator code.
- `Builtins` module centralizes default registration so downstream users can opt-out or swap modules.
- `api/` isolates the C bindings from the C++ surface to keep ClangSharp-friendly types stable.
- Registry and detector code is split from chunk modeling, keeping pure data structures header-only where possible.
- Stateless registries can be shared read-only once built; mutation not thread-safe.
- IO and decode operate on caller thread only.

## Performance Considerations
- Single-pass chunk scan with bounded buffering.
- Lazy decode: only decode chunks when requested (optional flag).
- Avoid copies by slicing views into input buffer when safe.
- Minimal allocations; reuse scratch buffers per context.

## Compliance / Safety
- No code injection or execution; read-only parsing/decoding.
- Dependency-free; all algorithms implemented in-house.
- Deterministic outputs for reproducibility.
