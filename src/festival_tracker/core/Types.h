#pragma once

#include <cstdint>
#include <string>

namespace festival_tracker {

using NodeId = std::uint32_t;
using SequenceNumber = std::uint32_t;
using TimestampMs = std::uint64_t;
using Milliseconds = std::uint64_t;

struct Position {
    double latitude_degrees = 0.0;
    double longitude_degrees = 0.0;
    double altitude_meters = 0.0;
    bool valid = false;
};

struct MotionSample {
    bool moving = false;
    double acceleration_mps2 = 0.0;
    double heading_degrees = 0.0;
};

enum class LedMode {
    Off,
    Idle,
    Locating,
    Sos,
    Flashlight,
};

enum class ButtonEvent {
    None,
    ShortPress,
    LongPress,
    DoublePress,
};

enum class SosStatus {
    Inactive,
    LocalActive,
    RemoteActive,
};

enum class EventType {
    Tick,
    LocalPosition,
    RemotePacket,
    Button,
    Motion,
    SosReceived,
    StorageDirty,
    Wake,
    Sleep,
};

enum class ActionType {
    None,
    SendPacket,
    RequestSync,
    UpdateLed,
    EnterSleep,
    WakeGnss,
    StopGnss,
    StoreTable,
    FlashlightOn,
    FlashlightOff,
    SosOn,
    SosOff,
};

struct Packet {
    NodeId source = 0;
    SequenceNumber sequence = 0;
    TimestampMs sent_at_ms = 0;
    Position position;
    bool sos = false;
};

struct NodeRecord {
    NodeId id = 0;
    SequenceNumber last_sequence = 0;
    TimestampMs last_seen_ms = 0;
    Position position;
    bool sos = false;
};

struct Event {
    EventType type = EventType::Tick;
    TimestampMs now_ms = 0;
    Packet packet;
    Position position;
    ButtonEvent button = ButtonEvent::None;
    MotionSample motion;
};

struct Action {
    ActionType type = ActionType::None;
    NodeId target = 0;
    Packet packet;
    LedMode led_mode = LedMode::Off;
    Milliseconds duration_ms = 0;
    std::string note;

    static Action none();
};

}  // namespace festival_tracker
