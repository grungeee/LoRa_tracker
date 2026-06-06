#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct SleepSchedulerConfig {
    Milliseconds idle_sleep_after_ms = 60000;
    Milliseconds minimum_awake_ms = 5000;
};

class SleepScheduler {
public:
    explicit SleepScheduler(SleepSchedulerConfig config = {});

    std::vector<Action> handle(const Event& event);
    bool asleep() const;
    TimestampMs last_activity_ms() const;

private:
    bool is_activity(const Event& event) const;

    SleepSchedulerConfig config_;
    bool asleep_ = false;
    TimestampMs woke_at_ms_ = 0;
    TimestampMs last_activity_ms_ = 0;
};

}  // namespace festival_tracker
