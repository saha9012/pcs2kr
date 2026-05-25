#include "building.hpp"

#include <algorithm>
#include <stdexcept>

namespace elevator {

Building::Building(int floors, int elevator_count)
    : floor_count_(floors),
      hall_up_(elevator_count, std::vector<bool>(floors, false)),
      hall_down_(elevator_count, std::vector<bool>(floors, false)) {
    if (floors < 2) {
        throw std::invalid_argument("Здание должно иметь минимум 2 этажа");
    }
    if (elevator_count < 1) {
        throw std::invalid_argument("Должен быть минимум один лифт");
    }

    for (int i = 0; i < elevator_count; ++i) {
        elevators_.push_back(std::make_unique<Elevator>(i + 1, *this, 0, floors - 1));
    }
}

void Building::submit_call(const CallRequest& call) {
  if (call.floor < 0 || call.floor >= floor_count_) {
    return;
  }
  if (call.type == CallType::HallUp && call.floor == floor_count_ - 1) {
    return;
  }
  if (call.type == CallType::HallDown && call.floor == 0) {
    return;
  }

  std::lock_guard<std::mutex> lock(calls_mutex_);
  pending_calls_.push(call);
}

bool Building::pop_next_call(CallRequest& out) {
  std::lock_guard<std::mutex> lock(calls_mutex_);
  if (pending_calls_.empty()) {
    return false;
  }
  out = pending_calls_.front();
  pending_calls_.pop();
  return true;
}

void Building::assign_call_to_elevator(const CallRequest& call, int elevator_id) {
  Elevator* elevator = elevator_by_id(elevator_id);
  if (!elevator) {
    return;
  }

  if (call.type == CallType::Cabin) {
    elevator->add_destination(call.floor);
  } else {
    elevator->assign_hall_call(call);
  }
}

Elevator* Building::elevator_by_id(int id) {
  const int idx = id - 1;
  if (idx < 0 || idx >= static_cast<int>(elevators_.size())) {
    return nullptr;
  }
  return elevators_[idx].get();
}

const Elevator* Building::elevator_by_id(int id) const {
  const int idx = id - 1;
  if (idx < 0 || idx >= static_cast<int>(elevators_.size())) {
    return nullptr;
  }
  return elevators_[idx].get();
}

std::vector<std::string> Building::elevator_status() const {
  std::vector<std::string> lines;
  lines.reserve(elevators_.size());
  for (const auto& e : elevators_) {
    lines.push_back(e->status_line());
  }
  return lines;
}

void Building::on_elevator_arrived(int elevator_id, int floor) {
  const int idx = elevator_id - 1;
  if (idx < 0 || idx >= static_cast<int>(elevators_.size())) {
    return;
  }

  std::lock_guard<std::mutex> lock(calls_mutex_);
  hall_up_[idx][floor] = false;
  hall_down_[idx][floor] = false;
}

bool Building::try_claim_hall_call(int floor, CallType type, int elevator_id) {
  const int idx = elevator_id - 1;
  if (idx < 0 || idx >= static_cast<int>(elevators_.size())) {
    return false;
  }

  std::lock_guard<std::mutex> lock(calls_mutex_);
  if (type == CallType::HallUp) {
    if (hall_up_[idx][floor]) {
      return false;
    }
    hall_up_[idx][floor] = true;
    return true;
  }
  if (type == CallType::HallDown) {
    if (hall_down_[idx][floor]) {
      return false;
    }
    hall_down_[idx][floor] = true;
    return true;
  }
  return true;
}

void Building::start_elevators() {
  for (auto& e : elevators_) {
    e->start();
  }
}

void Building::stop_elevators() {
  for (auto& e : elevators_) {
    e->stop();
  }
}

}  // namespace elevator
