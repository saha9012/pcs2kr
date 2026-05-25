#include "dispatcher.hpp"

#include <cmath>
#include <limits>

namespace elevator {

Dispatcher::Dispatcher(Building& building) : building_(building) {}

void Dispatcher::dispatch_pending() {
  CallRequest call;
  while (building_.pop_next_call(call)) {
    int elevator_id = call.assigned_elevator;
    if (elevator_id <= 0 || call.type != CallType::Cabin) {
      elevator_id = select_elevator(call);
    }
    if (elevator_id <= 0) {
      building_.submit_call(call);
      break;
    }
    building_.assign_call_to_elevator(call, elevator_id);
  }
}

int Dispatcher::select_elevator(const CallRequest& call) const {
  int best_id = -1;
  int best_score = std::numeric_limits<int>::max();

  for (int i = 1; i <= building_.elevator_count(); ++i) {
    const Elevator* elevator = building_.elevator_by_id(i);
    if (!elevator) {
      continue;
    }
    const int score = score_elevator(*elevator, call);
    if (score < best_score) {
      best_score = score;
      best_id = i;
    }
  }
  return best_id;
}

int Dispatcher::score_elevator(const Elevator& e, const CallRequest& call) const {
  const int current = e.current_floor();
  const int distance = std::abs(call.floor - current);
  int score = distance * 10;

  const Direction dir = e.direction();
  if (dir == Direction::Idle) {
    score -= 5;
  } else if (call.type == CallType::HallUp && dir == Direction::Up && call.floor >= current) {
    score -= 20;
  } else if (call.type == CallType::HallDown && dir == Direction::Down && call.floor <= current) {
    score -= 20;
  } else if (call.type == CallType::Cabin) {
    score -= 3;
  } else {
    score += 15;
  }

  return score;
}

}  // namespace elevator
