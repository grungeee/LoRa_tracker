#pragma once

#include <optional>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeImuAdapter {
public:
    void set_sample(const MotionSample& sample);
    std::optional<MotionSample> latest_sample() const;

private:
    std::optional<MotionSample> latest_sample_;
};

}  // namespace festival_tracker
