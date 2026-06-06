#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct GnssPolicyConfig {
    Milliseconds sample_interval_ms = 10000;
    Milliseconds fix_timeout_ms = 30000;
};

class GnssPolicy {
public:
    explicit GnssPolicy(GnssPolicyConfig config = {});

    std::vector<Action> handle(const Event& event);
    bool active() const;

private:
    GnssPolicyConfig config_;
    bool active_ = false;
    TimestampMs started_at_ms_ = 0;
    TimestampMs last_fix_ms_ = 0;
};

}  // namespace festival_tracker
