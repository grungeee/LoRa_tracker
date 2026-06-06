#include "festival_tracker/adapters/FakeRadioAdapter.h"

namespace festival_tracker {

void FakeRadioAdapter::send(const Packet& packet) {
    sent_.push_back(packet);
}

void FakeRadioAdapter::inject_received(const Packet& packet) {
    pending_.push_back(packet);
}

std::optional<Packet> FakeRadioAdapter::receive() {
    if (pending_.empty()) {
        return std::nullopt;
    }

    Packet packet = pending_.front();
    pending_.erase(pending_.begin());
    return packet;
}

const std::vector<Packet>& FakeRadioAdapter::sent_packets() const {
    return sent_;
}

std::size_t FakeRadioAdapter::pending_count() const {
    return pending_.size();
}

}  // namespace festival_tracker
