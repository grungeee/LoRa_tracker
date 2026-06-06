#pragma once

#include <vector>

#include "festival_tracker/core/NodeTable.h"
#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct SyncPolicyConfig {
    Milliseconds interval_ms = 30000;
};

class SyncPolicy {
public:
    explicit SyncPolicy(SyncPolicyConfig config = {});

    std::vector<Action> handle(const Event& event, const NodeTable& table);
    TimestampMs last_sync_ms() const;

private:
    SyncPolicyConfig config_;
    TimestampMs last_sync_ms_ = 0;
};

}  // namespace festival_tracker
