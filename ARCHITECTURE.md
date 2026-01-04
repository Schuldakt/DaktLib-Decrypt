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
│ (magic)    │ (layout)   │ (AES/RC4*) │ (LZ/XMem*) │ Calc/Chk │
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
│   ├── Decrypt.hpp          # main include
│   ├── Inspector.hpp
│   ├── Context.hpp
│   ├── Detection.hpp
│   ├── Chunk.hpp
│   ├── Crypto.hpp
│   ├── Compression.hpp
│   ├── Hashcode.hpp
│   ├── Input.hpp
│   ├── Results.hpp
│   └── c_api.h
├── src/
│   ├── Inspector.cpp
│   ├── Context.cpp
│   ├── Detection.cpp
│   ├── Chunk.cpp
│   ├── Crypto.cpp
│   ├── Compression.cpp
│   ├── Hashcode.cpp
│   ├── Input.cpp
│   ├── Builtins.cpp
│   └── c_api.cpp
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

## Registries
- `MagicRegistry`: signature patterns (offset + bytes, mask support) mapped to format IDs.
- `ChunkRegistry`: known chunk types with parsers for headers/layout.
- `CryptoRegistry`: decryptors keyed by name/ID; pluggable.
- `CompressionRegistry`: decompressors keyed by name/ID; pluggable.
- Registries are modifiable at runtime for new formats (no recompile required if exposed through C API plus dynamic registration hooks).

## Crypto / Hash / Compression
- Interfaces are small and stateless; instances created per operation.
- Built-ins are minimal and dependency-free; stubs for heavier algorithms allow future drop-in replacements.
- Hashcode check: CRC32 baseline; room for custom hash used by specific CryEngine chunks.

## Reporting
- `ChunkReport`: chunk id/type, offsets/sizes, chosen crypto/comp, hash validity, notes.
- `FileReport`: detected format(s), chunk list, unknown chunk summaries, warnings.
- Designed for tooling consumption and logging.

## C API Sketch

```c
// dakt_decrypt.h

typedef struct DaktDecryptContext* DaktDecryptContextHandle;
typedef struct DaktDecryptReport* DaktDecryptReportHandle;

typedef enum {
    DAKT_DECRYPT_OK = 0,
    DAKT_DECRYPT_ERR_IO,
    DAKT_DECRYPT_ERR_UNRECOGNIZED,
    DAKT_DECRYPT_ERR_DECRYPT,
    DAKT_DECRYPT_ERR_DECOMPRESS
} DaktDecryptResult;

DAKT_API DaktDecryptContextHandle dakt_decrypt_create(void);
DAKT_API void dakt_decrypt_destroy(DaktDecryptContextHandle ctx);

DAKT_API DaktDecryptResult dakt_decrypt_from_file(DaktDecryptContextHandle ctx, const char* path);
DAKT_API DaktDecryptResult dakt_decrypt_from_memory(DaktDecryptContextHandle ctx, const void* data, size_t size);

DAKT_API const char* dakt_decrypt_report_format(DaktDecryptReportHandle rpt);
DAKT_API size_t dakt_decrypt_report_chunk_count(DaktDecryptReportHandle rpt);
DAKT_API /* chunk info getters */

DAKT_API void dakt_decrypt_report_destroy(DaktDecryptReportHandle rpt);
```

## Thread Safety
- Context is not thread-safe; create per-thread contexts.
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
