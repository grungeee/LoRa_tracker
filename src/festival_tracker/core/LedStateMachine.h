#pragma once

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class LedStateMachine {
public:
    Action handle(const Event& event);
    void apply(LedMode mode);
    LedMode mode() const;

private:
    LedMode mode_ = LedMode::Off;
};

}  // namespace festival_tracker
