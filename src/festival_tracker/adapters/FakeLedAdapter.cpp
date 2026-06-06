#include "festival_tracker/adapters/FakeLedAdapter.h"

namespace festival_tracker {

void FakeLedAdapter::apply(const Action& action) {
    actions_.push_back(action);
    if (action.type == ActionType::UpdateLed ||
        action.type == ActionType::SosOn ||
        action.type == ActionType::SosOff) {
        mode_ = action.led_mode;
    }
}

void FakeLedAdapter::set_mode(LedMode mode) {
    mode_ = mode;
}

LedMode FakeLedAdapter::mode() const {
    return mode_;
}

const std::vector<Action>& FakeLedAdapter::actions() const {
    return actions_;
}

}  // namespace festival_tracker
