#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeLedAdapter {
public:
    void apply(const Action& action);
    void set_mode(LedMode mode);

    LedMode mode() const;
    const std::vector<Action>& actions() const;

private:
    LedMode mode_ = LedMode::Off;
    std::vector<Action> actions_;
};

}  // namespace festival_tracker
