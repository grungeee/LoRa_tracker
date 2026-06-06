#include <cassert>

#include "festival_tracker/core/NodeTable.h"
#include "festival_tracker/core/TableDigest.h"

using namespace festival_tracker;

namespace {

Packet packet(NodeId source, SequenceNumber sequence) {
    Packet packet;
    packet.source = source;
    packet.sequence = sequence;
    packet.sent_at_ms = 1000 + source;
    packet.position = Position{static_cast<double>(source), 16.0, 200.0, true};
    return packet;
}

}  // namespace

int main() {
    NodeTable left;
    left.upsert(packet(2, 1));
    left.upsert(packet(1, 1));

    NodeTable right;
    right.upsert(packet(1, 1));
    right.upsert(packet(2, 1));

    TableDigest digest;
    assert(digest.compute(left) == digest.compute(right));

    right.upsert(packet(2, 2));
    assert(digest.compute(left) != digest.compute(right));

    return 0;
}
