#include "festival_tracker/core/GnssPolicy.h"

namespace festival_tracker {

GnssPolicy::GnssPolicy(GnssPolicyConfig config)
    : config_(config) {
}

std::vector<Action> GnssPolicy::handle(const Event& event) {
    std::vector<Action> actions;

    if (event.type == EventType::Tick && !active_) {
        const bool due = last_fix_ms_ == 0 ||
            (event.now_ms >= last_fix_ms_ && event.now_ms - last_fix_ms_ >= config_.sample_interval_ms);
        if (due) {
            Action action;
            action.type = ActionType::WakeGnss;
            action.note = "gnss sample due";
            actions.push_back(action);
            active_ = true;
            started_at_ms_ = event.now_ms;
        }
    }

    if (event.type == EventType::LocalPosition && event.position.valid) {
        last_fix_ms_ = event.now_ms;
        if (active_) {
            Action action;
            action.type = ActionType::StopGnss;
            action.note = "valid fix";
            actions.push_back(action);
            active_ = false;
        }
    }

    if (event.type == EventType::Tick && active_ &&
        event.now_ms >= started_at_ms_ &&
        event.now_ms - started_at_ms_ >= config_.fix_timeout_ms) {
        Action action;
        action.type = ActionType::StopGnss;
        action.note = "fix timeout";
        actions.push_back(action);
        active_ = false;
    }

    return actions;
}

bool GnssPolicy::active() const {
    return active_;
}

}  // namespace festival_tracker
