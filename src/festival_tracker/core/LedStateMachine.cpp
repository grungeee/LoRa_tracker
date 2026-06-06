#include "festival_tracker/core/LedStateMachine.h"

namespace festival_tracker {

Action LedStateMachine::handle(const Event& event) {
    if (event.type == EventType::RemotePacket && event.packet.sos) {
        apply(LedMode::Sos);
    } else if (event.type == EventType::Motion && event.motion.moving) {
        apply(LedMode::Locating);
    } else if (event.type == EventType::Button && event.button == ButtonEvent::LongPress) {
        apply(mode_ == LedMode::Flashlight ? LedMode::Idle : LedMode::Flashlight);
    } else {
        return Action::none();
    }

    Action action;
    action.type = ActionType::UpdateLed;
    action.led_mode = mode_;
    action.note = "led state changed";
    return action;
}

void LedStateMachine::apply(LedMode mode) {
    mode_ = mode;
}

LedMode LedStateMachine::mode() const {
    return mode_;
}

}  // namespace festival_tracker
