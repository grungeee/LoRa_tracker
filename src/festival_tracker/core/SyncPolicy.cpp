#include "festival_tracker/core/SyncPolicy.h"

namespace festival_tracker {

SyncPolicy::SyncPolicy(SyncPolicyConfig config)
    : config_(config) {
}

std::vector<Action> SyncPolicy::handle(const Event& event, const NodeTable& table) {
    std::vector<Action> actions;

    if (event.type == EventType::Tick &&
        event.now_ms >= last_sync_ms_ &&
        event.now_ms - last_sync_ms_ >= config_.interval_ms) {
        Action action;
        action.type = ActionType::RequestSync;
        action.duration_ms = config_.interval_ms;
        action.note = "periodic table sync";
        actions.push_back(action);
        last_sync_ms_ = event.now_ms;
    }

    if (event.type == EventType::RemotePacket && table.find(event.packet.source).has_value()) {
        Action action;
        action.type = ActionType::StoreTable;
        action.note = "remote packet known by node table";
        actions.push_back(action);
    }

    return actions;
}

TimestampMs SyncPolicy::last_sync_ms() const {
    return last_sync_ms_;
}

}  // namespace festival_tracker
