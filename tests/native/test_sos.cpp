#include <cassert>
#include <fstream>
#include <string>

#include "festival_tracker/core/SosState.h"

using namespace festival_tracker;

int main() {
    SosState sos;

    Event local_on;
    local_on.type = EventType::Button;
    local_on.button = ButtonEvent::LongPress;
    const auto on_actions = sos.handle(local_on);
    assert(on_actions.size() == 1);
    assert(on_actions.front().type == ActionType::SosOn);
    assert(sos.status() == SosStatus::LocalActive);

    Event local_off = local_on;
    const auto off_actions = sos.handle(local_off);
    assert(off_actions.size() == 1);
    assert(off_actions.front().type == ActionType::SosOff);
    assert(!sos.active());

    Event remote;
    remote.type = EventType::RemotePacket;
    remote.packet.source = 77;
    remote.packet.sos = true;
    const auto remote_actions = sos.handle(remote);
    assert(remote_actions.size() == 1);
    assert(remote_actions.front().target == 77);
    assert(sos.status() == SosStatus::RemoteActive);

    std::ifstream fixture("tests/fixtures/sos_remote.json");
    assert(fixture.good());
    std::string contents((std::istreambuf_iterator<char>(fixture)), std::istreambuf_iterator<char>());
    assert(contents.find("sos_remote") != std::string::npos);

    return 0;
}
