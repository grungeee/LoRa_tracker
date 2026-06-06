#include "festival_tracker/adapters/FakeStorageAdapter.h"

namespace festival_tracker {

void FakeStorageAdapter::save(const std::vector<NodeRecord>& records) {
    records_ = records;
    ++save_count_;
}

std::vector<NodeRecord> FakeStorageAdapter::load() const {
    return records_;
}

std::size_t FakeStorageAdapter::save_count() const {
    return save_count_;
}

}  // namespace festival_tracker
