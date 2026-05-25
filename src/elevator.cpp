#include "elevator.hpp"

#include "building.hpp"

#include <chrono>
#include <sstream>
#include <thread>

namespace elevator {

namespace {
constexpr auto kMoveDelay = std::chrono::milliseconds(400);
constexpr auto kDoorDelay = std::chrono::milliseconds(600);
}  // namespace

Elevator::Elevator(int id, Building& building, int min_floor, int max_floor)
    : id_(id), building_(building), min_floor_(min_floor), max_floor_(max_floor) {}

Elevator::~Elevator() { stop(); }

void Elevator::start() {
  if (running_.exchange(true)) {
    return;
  }
  worker_ = std::thread(&Elevator::run, this);
}

void Elevator::stop() {
  if (!running_.exchange(false)) {
    return;
  }
  cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }
}

void Elevator::add_destination(int floor) {
  if (floor < min_floor_ || floor > max_floor_) {
    return;
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    destinations_.insert(floor);
    update_direction();
  }
  cv_.notify_one();
}

void Elevator::assign_hall_call(const CallRequest& call) {
  if (!building_.try_claim_hall_call(call.floor, call.type, id_)) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  destinations_.insert(call.floor);
  update_direction();
  cv_.notify_one();
}

Direction Elevator::direction() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return direction_;
}

ElevatorState Elevator::state() const { return state_.load(); }

std::string Elevator::status_line() const {
  std::ostringstream oss;
  oss << "Лифт " << id_ << ": этаж " << current_floor_.load() << ", "
      << direction_name(direction()) << ", " << state_name(state());
  return oss.str();
}

void Elevator::run() {
  while (running_.load()) {
    int next_floor = -1;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [this] { return !running_.load() || !destinations_.empty(); });

      if (!running_.load()) {
        break;
      }

      next_floor = pick_next_floor();
      if (next_floor < 0) {
        direction_ = Direction::Idle;
        state_ = ElevatorState::Idle;
        continue;
      }
    }

    if (next_floor == current_floor_.load()) {
      open_doors();
      continue;
    }

    state_ = ElevatorState::Moving;
    while (running_.load() && current_floor_.load() != next_floor) {
      move_one_floor();
      if (should_stop_at(current_floor_.load())) {
        open_doors();
        break;
      }
    }
  }
}

void Elevator::move_one_floor() {
  Direction dir;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    dir = direction_;
  }

  int target = current_floor_.load();
  if (dir == Direction::Up && target < max_floor_) {
    ++target;
  } else if (dir == Direction::Down && target > min_floor_) {
    --target;
  } else {
    return;
  }

  std::this_thread::sleep_for(kMoveDelay);
  current_floor_.store(target);
}

void Elevator::open_doors() {
  state_ = ElevatorState::DoorsOpen;
  std::this_thread::sleep_for(kDoorDelay);

  const int floor = current_floor_.load();
  building_.on_elevator_arrived(id_, floor);

  std::lock_guard<std::mutex> lock(mutex_);
  destinations_.erase(floor);

  if (destinations_.empty()) {
    direction_ = Direction::Idle;
    state_ = ElevatorState::Idle;
  } else {
    update_direction();
    state_ = ElevatorState::Idle;
  }
}

int Elevator::pick_next_floor() {
  if (destinations_.empty()) {
    return -1;
  }

  const int current = current_floor_.load();

  if (direction_ == Direction::Up) {
    auto it = destinations_.lower_bound(current);
    if (it != destinations_.end()) {
      return *it;
    }
  } else if (direction_ == Direction::Down) {
    auto it = destinations_.upper_bound(current);
    if (it != destinations_.begin()) {
      --it;
      return *it;
    }
  }

  int nearest = *destinations_.begin();
  int best_distance = std::abs(nearest - current);
  for (int floor : destinations_) {
    const int distance = std::abs(floor - current);
    if (distance < best_distance) {
      best_distance = distance;
      nearest = floor;
    }
  }
  return nearest;
}

bool Elevator::should_stop_at(int floor) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return destinations_.count(floor) > 0;
}

void Elevator::update_direction() {
  if (destinations_.empty()) {
    direction_ = Direction::Idle;
    return;
  }

  const int current = current_floor_.load();
  int target = *destinations_.begin();
  for (int floor : destinations_) {
    if (std::abs(floor - current) < std::abs(target - current)) {
      target = floor;
    }
  }

  if (target > current) {
    direction_ = Direction::Up;
  } else if (target < current) {
    direction_ = Direction::Down;
  }
}

}  // namespace elevator
