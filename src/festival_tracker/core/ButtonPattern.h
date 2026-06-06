#pragma once

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct ButtonPatternConfig {
    Milliseconds long_press_ms = 800;
    Milliseconds double_press_window_ms = 350;
};

class ButtonPattern {
public:
    explicit ButtonPattern(ButtonPatternConfig config = {});

    ButtonEvent observe(bool pressed, TimestampMs now_ms);

private:
    ButtonPatternConfig config_;
    bool currently_pressed_ = false;
    TimestampMs pressed_at_ms_ = 0;
    TimestampMs last_short_release_ms_ = 0;
};

}  // namespace festival_tracker
