# DaktLib-Decrypt TODO

## Legend
- **Priority**: P0 (Critical) → P3 (Nice-to-have)
- **Complexity**: S (Small) | M (Medium) | L (Large) | XL (Extra Large)

---

## Phase 1: Foundation
- [x] **[S]** CMake scaffolding (C++23, compile_commands, platform flags)
- [x] **[S]** Docs scaffolding (ARCHITECTURE, README)
- [ ] **[S]** Export macros for library consumers
- [x] **[S]** Modular include/src directory scaffolding
- [ ] **[S]** Core value/types (`ChunkHeader`, `ChunkInfo`, `ChunkDecodePlan`)
- [ ] **[M]** Registries (`MagicRegistry`, `ChunkRegistry`, `CryptoRegistry`, `CompressionRegistry`)
- [ ] **[S]** IO abstractions (`IInput`, `FileInput`, `MemoryInput`)

## Phase 2: Detection
- [ ] **[M]** Magic detection (offset/masked signatures)
- [ ] **[M]** Chunk header scanner (offset, size, type, hash fields)
- [ ] **[S]** Format ID + confidence scoring

## Phase 3: Crypto / Hash / Compression Interfaces
- [ ] **[S]** `ICrypto` decrypt interface
- [ ] **[S]** `IHasher` for chunk hashcode check (CRC32 baseline)
- [ ] **[S]** `ICompressor` decompress interface
- [ ] **[M]** Built-ins: XOR simple, RC4-lite; None/Lz4-lite/XMem-stub; CRC32

## Phase 4: Inspect / Orchestrator
- [ ] **[M]** `Inspector` pipeline (magic -> chunk scan -> strategy pick)
- [ ] **[M]** Plan resolution from registries
- [ ] **[S]** Error/report propagation

## Phase 5: Decode & Reporting
- [ ] **[M]** Per-chunk decode (crypto → decompress)
- [ ] **[S]** Lazy decode option (metadata-only mode)
- [ ] **[S]** `ChunkReport` and `FileReport` structures
- [ ] **[S]** Unknown-chunk capture with hexdump preview (bounded)

## Phase 6: Extensibility Hooks
- [ ] **[M]** Runtime registration for new magic/chunk/crypto/comp modules
- [ ] **[S]** Versioned registry entries for compatibility
- [ ] **[S]** Deterministic ID naming guidelines

## Phase 7: C API
- [ ] **[M]** Context/report handles
- [ ] **[M]** File and memory entry points
- [ ] **[S]** Chunk getter functions (type, offset, size, codec names)
- [ ] **[S]** Error codes + string messages

## Phase 8: Validation
- [ ] **[M]** Test fixtures for known CryEngine model files (different versions)
- [ ] **[M]** Golden reports for detection and chunk lists
- [ ] **[S]** Fuzz/bounds tests on header parsing

## Phase 9: Performance & UX
- [ ] **[S]** Scratch buffer reuse
- [ ] **[S]** Streaming read path for large files
- [ ] **[S]** Optional CLI sample (inspect + dump)

---

## Milestones

| Milestone | Target | Phases |
|-----------|--------|--------|
| **v0.1.0** | Week 1 | 1 (foundation) |
| **v0.2.0** | Week 2 | 2-3 (detection + interfaces) |
| **v0.3.0** | Week 3 | 4-5 (orchestrator + reporting) |
| **v0.4.0** | Week 4 | 6 (extensibility) |
| **v0.5.0** | Week 5 | 7 (C API) |
| **v0.6.0** | Week 6 | 8-9 (validation + perf/CLI) |

---

## Estimated Effort
- Foundation + detection: ~1.5 weeks
- Crypto/comp interfaces + built-ins: ~1 week
- Orchestrator + reporting: ~1 week
- Extensibility hooks + C API: ~1 week
- Validation + perf polish: ~1 week
- **Total: ~5-5.5 weeks**

---

## Acceptance Criteria
- [ ] Detects known CryEngine model chunk layouts and reports types/offsets/sizes
- [ ] Magic/signature detection yields correct format ID with confidence score
- [ ] Hashcode check (CRC32 baseline) matches expected values where known
- [ ] Supports XOR/RC4-lite crypto and None/LZ4-lite/XMem-stub compression paths
- [ ] Unknown chunks are surfaced with safe previews (bounded length)
- [ ] C API usable from C/C#, returns structured reports and errors
- [ ] Registries accept runtime additions without recompiling core
