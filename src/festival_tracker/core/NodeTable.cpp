#include "festival_tracker/core/NodeTable.h"

#include <algorithm>

namespace festival_tracker {

bool NodeTable::upsert(const Packet& packet) {
    auto it = std::find_if(records_.begin(), records_.end(), [&](const NodeRecord& record) {
        return record.id == packet.source;
    });

    if (it != records_.end()) {
        if (packet.sequence < it->last_sequence) {
            return false;
        }

        it->last_sequence = packet.sequence;
        it->last_seen_ms = packet.sent_at_ms;
        it->position = packet.position;
        it->sos = packet.sos;
        return true;
    }

    records_.push_back(NodeRecord{
        packet.source,
        packet.sequence,
        packet.sent_at_ms,
        packet.position,
        packet.sos,
    });
    return true;
}

std::optional<NodeRecord> NodeTable::find(NodeId id) const {
    auto it = std::find_if(records_.begin(), records_.end(), [&](const NodeRecord& record) {
        return record.id == id;
    });

    if (it == records_.end()) {
        return std::nullopt;
    }

    return *it;
}

std::size_t NodeTable::remove_expired(TimestampMs now_ms, Milliseconds max_age_ms) {
    const auto before = records_.size();
    records_.erase(
        std::remove_if(records_.begin(), records_.end(), [&](const NodeRecord& record) {
            return now_ms >= record.last_seen_ms && now_ms - record.last_seen_ms > max_age_ms;
        }),
        records_.end());
    return before - records_.size();
}

std::size_t NodeTable::size() const {
    return records_.size();
}

const std::vector<NodeRecord>& NodeTable::records() const {
    return records_;
}

}  // namespace festival_tracker
