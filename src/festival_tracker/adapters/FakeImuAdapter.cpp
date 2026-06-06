#include "festival_tracker/adapters/FakeImuAdapter.h"

namespace festival_tracker {

void FakeImuAdapter::set_sample(const MotionSample& sample) {
    latest_sample_ = sample;
}

std::optional<MotionSample> FakeImuAdapter::latest_sample() const {
    return latest_sample_;
}

}  // namespace festival_tracker
