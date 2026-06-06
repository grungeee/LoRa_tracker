#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

struct ImuPolicyConfig {
    double moving_acceleration_threshold_mps2 = 0.5;
};

class ImuPolicy {
public:
    explicit ImuPolicy(ImuPolicyConfig config = {});

    std::vector<Action> handle(const Event& event);
    bool moving() const;

private:
    ImuPolicyConfig config_;
    bool moving_ = false;
};

}  // namespace festival_tracker
