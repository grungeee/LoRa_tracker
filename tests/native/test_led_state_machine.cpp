#include <cassert>

#include "festival_tracker/adapters/FakeLedAdapter.h"
#include "festival_tracker/core/LedStateMachine.h"

using namespace festival_tracker;

int main() {
    LedStateMachine machine;
    FakeLedAdapter adapter;

    Event flashlight;
    flashlight.type = EventType::Button;
    flashlight.button = ButtonEvent::LongPress;
    const auto flashlight_action = machine.handle(flashlight);
    assert(flashlight_action.type == ActionType::UpdateLed);
    assert(flashlight_action.led_mode == LedMode::Flashlight);
    adapter.apply(flashlight_action);
    assert(adapter.mode() == LedMode::Flashlight);

    Event remote_sos;
    remote_sos.type = EventType::RemotePacket;
    remote_sos.packet.source = 11;
    remote_sos.packet.sos = true;
    const auto sos_action = machine.handle(remote_sos);
    assert(sos_action.type == ActionType::UpdateLed);
    assert(sos_action.led_mode == LedMode::Sos);

    return 0;
}
