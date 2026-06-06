#include <cassert>

#include "festival_tracker/adapters/FakeImuAdapter.h"
#include "festival_tracker/core/ImuPolicy.h"

using namespace festival_tracker;

int main() {
    ImuPolicy policy(ImuPolicyConfig{0.5});
    FakeImuAdapter adapter;

    Event moving;
    moving.type = EventType::Motion;
    moving.motion.acceleration_mps2 = 0.8;
    adapter.set_sample(moving.motion);

    const auto moving_actions = policy.handle(moving);
    assert(moving_actions.size() == 1);
    assert(moving_actions.front().type == ActionType::UpdateLed);
    assert(moving_actions.front().led_mode == LedMode::Locating);
    assert(policy.moving());
    assert(adapter.latest_sample().has_value());

    Event still;
    still.type = EventType::Motion;
    still.motion.acceleration_mps2 = 0.0;
    const auto still_actions = policy.handle(still);
    assert(still_actions.size() == 1);
    assert(still_actions.front().led_mode == LedMode::Idle);
    assert(!policy.moving());

    return 0;
}
