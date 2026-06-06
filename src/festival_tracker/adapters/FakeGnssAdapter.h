#pragma once

#include <optional>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeGnssAdapter {
public:
    void set_active(bool active);
    bool active() const;

    void set_position(const Position& position);
    std::optional<Position> latest_position() const;

private:
    bool active_ = false;
    std::optional<Position> latest_position_;
};

}  // namespace festival_tracker
