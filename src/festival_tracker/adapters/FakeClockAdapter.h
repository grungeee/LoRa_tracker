#pragma once

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeClockAdapter {
public:
    explicit FakeClockAdapter(TimestampMs initial_ms = 0);

    TimestampMs now_ms() const;
    void set_ms(TimestampMs now_ms);
    void advance_ms(Milliseconds delta_ms);

private:
    TimestampMs now_ms_ = 0;
};

}  // namespace festival_tracker
