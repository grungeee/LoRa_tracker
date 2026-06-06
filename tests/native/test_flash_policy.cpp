#include <cassert>

#include "festival_tracker/adapters/FakeClockAdapter.h"
#include "festival_tracker/adapters/FakeStorageAdapter.h"
#include "festival_tracker/core/FlashPolicy.h"
#include "festival_tracker/core/NodeTable.h"

using namespace festival_tracker;

int main() {
    FlashPolicy policy(FlashPolicyConfig{100});
    FakeClockAdapter clock;
    FakeStorageAdapter storage;

    Event dirty;
    dirty.type = EventType::StorageDirty;
    dirty.now_ms = clock.now_ms();
    assert(policy.handle(dirty).empty());
    assert(policy.dirty());

    clock.advance_ms(99);
    Event early;
    early.type = EventType::Tick;
    early.now_ms = clock.now_ms();
    assert(policy.handle(early).empty());

    clock.advance_ms(1);
    Event flush;
    flush.type = EventType::Tick;
    flush.now_ms = clock.now_ms();
    const auto actions = policy.handle(flush);
    assert(actions.size() == 1);
    assert(actions.front().type == ActionType::StoreTable);

    NodeTable table;
    Packet packet;
    packet.source = 4;
    packet.sequence = 1;
    assert(table.upsert(packet));
    storage.save(table.records());
    assert(storage.save_count() == 1);
    assert(storage.load().size() == 1);

    policy.mark_clean();
    assert(!policy.dirty());

    return 0;
}
