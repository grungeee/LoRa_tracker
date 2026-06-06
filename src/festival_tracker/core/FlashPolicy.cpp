#include "festival_tracker/core/FlashPolicy.h"

namespace festival_tracker {

FlashPolicy::FlashPolicy(FlashPolicyConfig config)
    : config_(config) {
}

std::vector<Action> FlashPolicy::handle(const Event& event) {
    std::vector<Action> actions;

    if (event.type == EventType::StorageDirty ||
        event.type == EventType::RemotePacket ||
        event.type == EventType::LocalPosition) {
        dirty_ = true;
    }

    if (event.type == EventType::Tick && dirty_ &&
        event.now_ms >= last_flush_ms_ &&
        event.now_ms - last_flush_ms_ >= config_.flush_interval_ms) {
        Action action;
        action.type = ActionType::StoreTable;
        action.note = "flush dirty table";
        actions.push_back(action);
        last_flush_ms_ = event.now_ms;
    }

    return actions;
}

void FlashPolicy::mark_clean() {
    dirty_ = false;
}

bool FlashPolicy::dirty() const {
    return dirty_;
}

}  // namespace festival_tracker
