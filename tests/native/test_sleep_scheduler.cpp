#include <cassert>

#include "festival_tracker/core/SleepScheduler.h"

using namespace festival_tracker;

int main() {
    SleepScheduler scheduler(SleepSchedulerConfig{100, 0});

    Event wake;
    wake.type = EventType::Wake;
    wake.now_ms = 0;
    assert(scheduler.handle(wake).empty());
    assert(!scheduler.asleep());

    Event before_timeout;
    before_timeout.type = EventType::Tick;
    before_timeout.now_ms = 99;
    assert(scheduler.handle(before_timeout).empty());

    Event timeout;
    timeout.type = EventType::Tick;
    timeout.now_ms = 100;
    const auto actions = scheduler.handle(timeout);
    assert(actions.size() == 1);
    assert(actions.front().type == ActionType::EnterSleep);
    assert(scheduler.asleep());

    Event motion;
    motion.type = EventType::Motion;
    motion.now_ms = 120;
    motion.motion.moving = true;
    assert(scheduler.handle(motion).empty());
    assert(!scheduler.asleep());

    return 0;
}
