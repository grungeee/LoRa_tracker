#pragma once

#include <optional>
#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeRadioAdapter {
public:
    void send(const Packet& packet);
    void inject_received(const Packet& packet);
    std::optional<Packet> receive();

    const std::vector<Packet>& sent_packets() const;
    std::size_t pending_count() const;

private:
    std::vector<Packet> sent_;
    std::vector<Packet> pending_;
};

}  // namespace festival_tracker
