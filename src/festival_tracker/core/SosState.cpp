#include "festival_tracker/core/SosState.h"

namespace festival_tracker {

std::vector<Action> SosState::handle(const Event& event) {
    std::vector<Action> actions;

    if (event.type == EventType::Button && event.button == ButtonEvent::LongPress) {
        Action action;
        if (status_ == SosStatus::LocalActive) {
            status_ = SosStatus::Inactive;
            action.type = ActionType::SosOff;
            action.led_mode = LedMode::Idle;
            action.note = "local sos cleared";
        } else {
            status_ = SosStatus::LocalActive;
            action.type = ActionType::SosOn;
            action.led_mode = LedMode::Sos;
            action.note = "local sos active";
        }
        actions.push_back(action);
    }

    if (event.type == EventType::RemotePacket && event.packet.sos) {
        status_ = SosStatus::RemoteActive;
        Action action;
        action.type = ActionType::SosOn;
        action.target = event.packet.source;
        action.led_mode = LedMode::Sos;
        action.note = "remote sos active";
        actions.push_back(action);
    }

    return actions;
}

SosStatus SosState::status() const {
    return status_;
}

bool SosState::active() const {
    return status_ != SosStatus::Inactive;
}

}  // namespace festival_tracker
