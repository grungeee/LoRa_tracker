#include <cassert>

#include "festival_tracker/core/NodeTable.h"
#include "festival_tracker/core/SyncPolicy.h"

using namespace festival_tracker;

int main() {
    SyncPolicy policy(SyncPolicyConfig{100});
    NodeTable table;

    Event early;
    early.type = EventType::Tick;
    early.now_ms = 99;
    assert(policy.handle(early, table).empty());

    Event due;
    due.type = EventType::Tick;
    due.now_ms = 100;
    const auto sync_actions = policy.handle(due, table);
    assert(sync_actions.size() == 1);
    assert(sync_actions.front().type == ActionType::RequestSync);

    Packet packet;
    packet.source = 9;
    packet.sequence = 1;
    packet.sent_at_ms = 150;
    assert(table.upsert(packet));

    Event remote;
    remote.type = EventType::RemotePacket;
    remote.now_ms = 151;
    remote.packet = packet;
    const auto store_actions = policy.handle(remote, table);
    assert(store_actions.size() == 1);
    assert(store_actions.front().type == ActionType::StoreTable);

    return 0;
}
