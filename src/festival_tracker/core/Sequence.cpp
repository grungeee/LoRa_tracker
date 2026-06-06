#include "festival_tracker/core/Sequence.h"

namespace festival_tracker {

Sequence::Sequence(SequenceNumber initial)
    : current_(initial) {
}

SequenceNumber Sequence::next() {
    ++current_;
    return current_;
}

SequenceNumber Sequence::current() const {
    return current_;
}

bool Sequence::is_newer(SequenceNumber incoming, SequenceNumber last_seen) const {
    return incoming > last_seen;
}

bool Sequence::is_duplicate_or_stale(SequenceNumber incoming, SequenceNumber last_seen) const {
    return !is_newer(incoming, last_seen);
}

}  // namespace festival_tracker
