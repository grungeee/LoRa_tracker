#pragma once

#include "festival_tracker/core/Types.h"

namespace festival_tracker {

class Sequence {
public:
    explicit Sequence(SequenceNumber initial = 0);

    SequenceNumber next();
    SequenceNumber current() const;
    bool is_newer(SequenceNumber incoming, SequenceNumber last_seen) const;
    bool is_duplicate_or_stale(SequenceNumber incoming, SequenceNumber last_seen) const;

private:
    SequenceNumber current_;
};

}  // namespace festival_tracker
