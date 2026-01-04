#pragma once

#include <cstddef>
#include <cstdint>

namespace dakt::decrypt {
struct ChunkHeader {
  // Placeholder fields for chunk header metadata.
  std::uint32_t type{};
  std::uint32_t size{};
};

struct ChunkInfo {
  ChunkHeader header{};
};

struct ChunkDecodePlan {
  // Placeholder for chosen crypto/compression/hash identifiers.
};
} // namespace dakt::decrypt
