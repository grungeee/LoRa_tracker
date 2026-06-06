#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct FlashPolicyConfig {
    Milliseconds flush_interval_ms = 5000;
};

class FlashPolicy {
public:
    explicit FlashPolicy(FlashPolicyConfig config = {});

    std::vector<Action> handle(const Event& event);
    void mark_clean();
    bool dirty() const;

private:
    FlashPolicyConfig config_;
    bool dirty_ = false;
    TimestampMs last_flush_ms_ = 0;
};

}  // namespace festival_tracker
