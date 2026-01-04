# AGENT Brief — DaktLib-Decrypt

## Mission
- Detect and decode CryEngine model chunk types, magic numbers, hashcodes, and crypto/comp variants via modular registries.

## Constraints
- C++23, dependency-free; deterministic outputs.
- Offline, read-only analysis; no execution or injection.
- Pluggable registries for magic/chunk/crypto/comp; runtime-extendable.
- C API must stay ClangSharp-friendly.

## Scope Highlights
- Magic/chunk detection, crypto (XOR/RC4-lite/AES-stub), compression (None/LZ4-lite/XMem-stub), CRC32 hash checks.
- Reporting of chunk offsets/sizes/types and unknowns with safe previews.

## Limitations
- No third-party crypto/comp libs bundled initially.
- Not a general-purpose encrypt/decrypt toolkit beyond supported chunk flows.
