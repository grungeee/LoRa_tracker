#include <cassert>

#include "festival_tracker/adapters/FakeGnssAdapter.h"
#include "festival_tracker/core/GnssPolicy.h"

using namespace festival_tracker;

int main() {
    GnssPolicy policy(GnssPolicyConfig{100, 1000});
    FakeGnssAdapter adapter;

    Event tick;
    tick.type = EventType::Tick;
    tick.now_ms = 0;
    const auto wake_actions = policy.handle(tick);
    assert(wake_actions.size() == 1);
    assert(wake_actions.front().type == ActionType::WakeGnss);
    assert(policy.active());

    adapter.set_active(true);
    assert(adapter.active());

    Event fix;
    fix.type = EventType::LocalPosition;
    fix.now_ms = 10;
    fix.position = Position{48.2082, 16.3738, 180.0, true};
    adapter.set_position(fix.position);

    const auto stop_actions = policy.handle(fix);
    assert(stop_actions.size() == 1);
    assert(stop_actions.front().type == ActionType::StopGnss);
    assert(!policy.active());
    assert(adapter.latest_position().has_value());

    tick.now_ms = 50;
    assert(policy.handle(tick).empty());

    return 0;
}
