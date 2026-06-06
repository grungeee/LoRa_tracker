#include <cassert>

#include "festival_tracker/core/ButtonPattern.h"
#include "festival_tracker/core/Sequence.h"

using namespace festival_tracker;

int main() {
    Sequence sequence;
    assert(sequence.current() == 0);
    assert(sequence.next() == 1);
    assert(sequence.next() == 2);
    assert(sequence.is_newer(3, 2));
    assert(sequence.is_duplicate_or_stale(2, 2));
    assert(sequence.is_duplicate_or_stale(1, 2));

    ButtonPattern button;
    assert(button.observe(true, 1000) == ButtonEvent::None);
    assert(button.observe(false, 1050) == ButtonEvent::ShortPress);
    assert(button.observe(true, 1100) == ButtonEvent::None);
    assert(button.observe(false, 1150) == ButtonEvent::DoublePress);
    assert(button.observe(true, 2000) == ButtonEvent::None);
    assert(button.observe(false, 3000) == ButtonEvent::LongPress);

    return 0;
}
