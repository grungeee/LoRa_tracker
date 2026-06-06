#include "festival_tracker/adapters/FakeGnssAdapter.h"

namespace festival_tracker {

void FakeGnssAdapter::set_active(bool active) {
    active_ = active;
}

bool FakeGnssAdapter::active() const {
    return active_;
}

void FakeGnssAdapter::set_position(const Position& position) {
    latest_position_ = position;
}

std::optional<Position> FakeGnssAdapter::latest_position() const {
    return latest_position_;
}

}  // namespace festival_tracker
