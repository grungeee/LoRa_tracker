#pragma once

#include <vector>

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class FakeStorageAdapter {
public:
    void save(const std::vector<NodeRecord>& records);
    std::vector<NodeRecord> load() const;
    std::size_t save_count() const;

private:
    std::vector<NodeRecord> records_;
    std::size_t save_count_ = 0;
};

}  // namespace festival_tracker
