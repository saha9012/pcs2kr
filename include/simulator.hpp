#pragma once

#include <atomic>
#include <random>
#include <thread>

#include "building.hpp"
#include "dispatcher.hpp"

namespace elevator {

class Simulator {
public:
    Simulator(int floors, int elevators, int tick_ms = 500);

    void start();
    void stop();

    Building& building() { return building_; }

    void run_interactive();
    void run_demo(int seconds = 30);

private:
    void simulation_loop();
    void generate_random_call();
    void print_state() const;

    Building building_;
    Dispatcher dispatcher_;
    int tick_ms_;

    std::atomic<bool> running_{false};
    std::thread loop_thread_;
    std::mt19937 rng_{std::random_device{}()};
};

}  // namespace elevator
