#pragma once

#include <cstdint>

#include "festival_tracker/core/NodeTable.h"

namespace festival_tracker {

class TableDigest {
public:
    std::uint32_t compute(const NodeTable& table) const;
};

}  // namespace festival_tracker
