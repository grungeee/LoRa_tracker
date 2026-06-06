#include "festival_tracker/core/TableDigest.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace festival_tracker {

namespace {

void mix_byte(std::uint32_t& hash, std::uint8_t value) {
    hash ^= value;
    hash *= 16777619u;
}

template <typename T>
void mix_value(std::uint32_t& hash, const T& value) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&value);
    for (std::size_t index = 0; index < sizeof(T); ++index) {
        mix_byte(hash, bytes[index]);
    }
}

}  // namespace

std::uint32_t TableDigest::compute(const NodeTable& table) const {
    auto records = table.records();
    std::sort(records.begin(), records.end(), [](const NodeRecord& left, const NodeRecord& right) {
        return left.id < right.id;
    });

    std::uint32_t hash = 2166136261u;
    for (const auto& record : records) {
        mix_value(hash, record.id);
        mix_value(hash, record.last_sequence);
        mix_value(hash, record.last_seen_ms);
        mix_value(hash, record.position.latitude_degrees);
        mix_value(hash, record.position.longitude_degrees);
        mix_value(hash, record.position.altitude_meters);
        mix_value(hash, record.position.valid);
        mix_value(hash, record.sos);
    }
    return hash;
}

}  // namespace festival_tracker
