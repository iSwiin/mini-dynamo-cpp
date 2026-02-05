#include "dynamo/hash.hpp"

namespace dynamo {

uint64_t fnv1a_64(std::string_view s) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t h = FNV_OFFSET;
  for (unsigned char c : s) {
    h ^= static_cast<uint64_t>(c);
    h *= FNV_PRIME;
  }
  return h;
}

} // namespace dynamo
