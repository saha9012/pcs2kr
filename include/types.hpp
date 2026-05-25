#pragma once

namespace elevator {

enum class Direction { Idle, Up, Down };

enum class ElevatorState { Idle, Moving, DoorsOpen };

enum class CallType { HallUp, HallDown, Cabin };

struct CallRequest {
    int floor;
    CallType type;
    int assigned_elevator{-1};
};

inline const char* direction_name(Direction d) {
    switch (d) {
        case Direction::Up: return "вверх";
        case Direction::Down: return "вниз";
        default: return "стоит";
    }
}

inline const char* state_name(ElevatorState s) {
    switch (s) {
        case ElevatorState::Moving: return "движение";
        case ElevatorState::DoorsOpen: return "двери открыты";
        default: return "ожидание";
    }
}

}  // namespace elevator
