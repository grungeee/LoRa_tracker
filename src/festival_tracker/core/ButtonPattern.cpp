#include "festival_tracker/core/ButtonPattern.h"

namespace festival_tracker {

ButtonPattern::ButtonPattern(ButtonPatternConfig config)
    : config_(config) {
}

ButtonEvent ButtonPattern::observe(bool pressed, TimestampMs now_ms) {
    if (pressed && !currently_pressed_) {
        currently_pressed_ = true;
        pressed_at_ms_ = now_ms;
        return ButtonEvent::None;
    }

    if (!pressed && currently_pressed_) {
        currently_pressed_ = false;
        const auto duration_ms = now_ms >= pressed_at_ms_ ? now_ms - pressed_at_ms_ : 0;

        if (duration_ms >= config_.long_press_ms) {
            last_short_release_ms_ = 0;
            return ButtonEvent::LongPress;
        }

        const bool double_press = last_short_release_ms_ != 0 &&
            now_ms >= last_short_release_ms_ &&
            now_ms - last_short_release_ms_ <= config_.double_press_window_ms;
        last_short_release_ms_ = double_press ? 0 : now_ms;

        return double_press ? ButtonEvent::DoublePress : ButtonEvent::ShortPress;
    }

    return ButtonEvent::None;
}

}  // namespace festival_tracker
