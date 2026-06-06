#include <cassert>
#include <fstream>
#include <string>

#include "festival_tracker/adapters/FakeRadioAdapter.h"
#include "festival_tracker/core/NodeTable.h"

using namespace festival_tracker;

namespace {

Packet packet(NodeId source, SequenceNumber sequence, TimestampMs sent_at_ms) {
    Packet packet;
    packet.source = source;
    packet.sequence = sequence;
    packet.sent_at_ms = sent_at_ms;
    packet.position = Position{48.2082, 16.3738, 180.0, true};
    return packet;
}

}  // namespace

int main() {
    NodeTable table;
    assert(table.size() == 0);

    assert(table.upsert(packet(42, 1, 1000)));
    assert(table.size() == 1);

    const auto found = table.find(42);
    assert(found.has_value());
    assert(found->last_sequence == 1);

    assert(!table.upsert(packet(42, 0, 900)));
    assert(table.find(42)->last_sequence == 1);

    assert(table.upsert(packet(42, 2, 2000)));
    assert(table.find(42)->last_sequence == 2);

    assert(table.remove_expired(10000, 5000) == 1);
    assert(table.size() == 0);

    FakeRadioAdapter radio;
    radio.send(packet(7, 1, 3000));
    assert(radio.sent_packets().size() == 1);

    std::ifstream fixture("tests/fixtures/node_reappears.json");
    assert(fixture.good());
    std::string contents((std::istreambuf_iterator<char>(fixture)), std::istreambuf_iterator<char>());
    assert(contents.find("node_reappears") != std::string::npos);

    return 0;
}
