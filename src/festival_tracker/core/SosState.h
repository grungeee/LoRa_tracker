#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class SosState {
public:
    std::vector<Action> handle(const Event& event);
    SosStatus status() const;
    bool active() const;

private:
    SosStatus status_ = SosStatus::Inactive;
};

}  // namespace festival_tracker
