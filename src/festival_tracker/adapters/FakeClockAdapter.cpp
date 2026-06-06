#include "festival_tracker/adapters/FakeClockAdapter.h"

namespace festival_tracker {

FakeClockAdapter::FakeClockAdapter(TimestampMs initial_ms)
    : now_ms_(initial_ms) {
}

TimestampMs FakeClockAdapter::now_ms() const {
    return now_ms_;
}

void FakeClockAdapter::set_ms(TimestampMs now_ms) {
    now_ms_ = now_ms;
}

void FakeClockAdapter::advance_ms(Milliseconds delta_ms) {
    now_ms_ += delta_ms;
}

}  // namespace festival_tracker
