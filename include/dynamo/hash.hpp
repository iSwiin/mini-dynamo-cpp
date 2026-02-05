#pragma once
#include <cstdint>
#include <string_view>

namespace dynamo {

// Stable 64-bit FNV-1a hash (deterministic across platforms/compilers).
uint64_t fnv1a_64(std::string_view s);

} // namespace dynamo
