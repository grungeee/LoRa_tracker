#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class NodeTable {
public:
    bool upsert(const Packet& packet);
    std::optional<NodeRecord> find(NodeId id) const;
    std::size_t remove_expired(TimestampMs now_ms, Milliseconds max_age_ms);
    std::size_t size() const;
    const std::vector<NodeRecord>& records() const;

private:
    std::vector<NodeRecord> records_;
};

}  // namespace festival_tracker
