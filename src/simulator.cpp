#include "simulator.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace elevator {

namespace {
void clear_screen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}
}  // namespace

Simulator::Simulator(int floors, int elevators, int tick_ms)
    : building_(floors, elevators), dispatcher_(building_), tick_ms_(tick_ms) {}

void Simulator::start() {
  if (running_.exchange(true)) {
    return;
  }
  building_.start_elevators();
  loop_thread_ = std::thread(&Simulator::simulation_loop, this);
}

void Simulator::stop() {
  if (!running_.exchange(false)) {
    return;
  }
  if (loop_thread_.joinable()) {
    loop_thread_.join();
  }
  building_.stop_elevators();
}

void Simulator::run_interactive() {
  start();

  std::cout << "=== Симулятор лифтов ===\n";
  std::cout << "Этажей: " << building_.floor_count()
            << ", лифтов: " << building_.elevator_count() << "\n\n";
  std::cout << "Команды:\n"
            << "  u <этаж>  - вызов лифта вверх с этажа\n"
            << "  d <этаж>  - вызов лифта вниз с этажа\n"
            << "  c <лифт> <этаж> - кнопка в кабине\n"
            << "  r         - случайный вызов\n"
            << "  q         - выход\n\n";

  std::string line;
  while (running_.load()) {
    print_state();
    std::cout << "> ";
    if (!std::getline(std::cin, line)) {
      break;
    }

    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "q" || cmd == "Q") {
      break;
    }
    if (cmd == "r" || cmd == "R") {
      generate_random_call();
      continue;
    }

    if (cmd == "u" || cmd == "U") {
      int floor = 0;
      if (iss >> floor) {
        building_.submit_call({floor, CallType::HallUp});
      }
      continue;
    }

    if (cmd == "d" || cmd == "D") {
      int floor = 0;
      if (iss >> floor) {
        building_.submit_call({floor, CallType::HallDown});
      }
      continue;
    }

    if (cmd == "c" || cmd == "C") {
      int elevator_id = 0;
      int floor = 0;
      if (iss >> elevator_id >> floor) {
        CallRequest req{floor, CallType::Cabin, elevator_id};
        building_.submit_call(req);
      }
      continue;
    }
  }

  stop();
}

void Simulator::run_demo(int seconds) {
  start();

  const auto end_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
  while (running_.load() && std::chrono::steady_clock::now() < end_time) {
    generate_random_call();
    print_state();
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  stop();
}

void Simulator::simulation_loop() {
  while (running_.load()) {
    dispatcher_.dispatch_pending();
    std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms_));
  }
}

void Simulator::generate_random_call() {
  const int floors = building_.floor_count();
  const int elevators = building_.elevator_count();
  std::uniform_int_distribution<int> floor_dist(0, floors - 1);
  std::uniform_int_distribution<int> type_dist(0, 2);
  std::uniform_int_distribution<int> elevator_dist(1, elevators);

  const int floor = floor_dist(rng_);
  const int type_val = type_dist(rng_);

  CallRequest call{floor, CallType::HallUp};
  if (type_val == 0 && floor < floors - 1) {
    call.type = CallType::HallUp;
  } else if (type_val == 1 && floor > 0) {
    call.type = CallType::HallDown;
  } else {
    call.type = CallType::Cabin;
    call.assigned_elevator = elevator_dist(rng_);
  }

  building_.submit_call(call);
}

void Simulator::print_state() const {
  clear_screen();
  std::cout << "=== Симулятор лифтов ===\n\n";
  for (const auto& line : building_.elevator_status()) {
    std::cout << line << "\n";
  }
  std::cout << "\n";
}

}  // namespace elevator
