#include "festival_tracker/core/SleepScheduler.h"

namespace festival_tracker {

SleepScheduler::SleepScheduler(SleepSchedulerConfig config)
    : config_(config) {
}

std::vector<Action> SleepScheduler::handle(const Event& event) {
    std::vector<Action> actions;

    if (event.type == EventType::Wake) {
        asleep_ = false;
        woke_at_ms_ = event.now_ms;
        last_activity_ms_ = event.now_ms;
        return actions;
    }

    if (is_activity(event)) {
        last_activity_ms_ = event.now_ms;
        if (asleep_) {
            asleep_ = false;
            woke_at_ms_ = event.now_ms;
        }
        return actions;
    }

    if (event.type == EventType::Tick && !asleep_) {
        const bool awake_long_enough = event.now_ms >= woke_at_ms_ &&
            event.now_ms - woke_at_ms_ >= config_.minimum_awake_ms;
        const bool idle_long_enough = event.now_ms >= last_activity_ms_ &&
            event.now_ms - last_activity_ms_ >= config_.idle_sleep_after_ms;

        if (awake_long_enough && idle_long_enough) {
            Action action;
            action.type = ActionType::EnterSleep;
            action.note = "idle timeout";
            actions.push_back(action);
            asleep_ = true;
        }
    }

    return actions;
}

bool SleepScheduler::asleep() const {
    return asleep_;
}

TimestampMs SleepScheduler::last_activity_ms() const {
    return last_activity_ms_;
}

bool SleepScheduler::is_activity(const Event& event) const {
    return event.type == EventType::Button ||
        event.type == EventType::LocalPosition ||
        event.type == EventType::RemotePacket ||
        (event.type == EventType::Motion && event.motion.moving);
}

}  // namespace festival_tracker
