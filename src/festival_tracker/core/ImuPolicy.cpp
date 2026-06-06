#include "festival_tracker/core/ImuPolicy.h"

#include <cmath>

namespace festival_tracker {

ImuPolicy::ImuPolicy(ImuPolicyConfig config)
    : config_(config) {
}

std::vector<Action> ImuPolicy::handle(const Event& event) {
    std::vector<Action> actions;

    if (event.type != EventType::Motion) {
        return actions;
    }

    const bool now_moving = event.motion.moving ||
        std::abs(event.motion.acceleration_mps2) >= config_.moving_acceleration_threshold_mps2;

    if (now_moving != moving_) {
        moving_ = now_moving;
        Action action;
        action.type = ActionType::UpdateLed;
        action.led_mode = moving_ ? LedMode::Locating : LedMode::Idle;
        action.note = moving_ ? "motion detected" : "motion stopped";
        actions.push_back(action);
    }

    return actions;
}

bool ImuPolicy::moving() const {
    return moving_;
}

}  // namespace festival_tracker
