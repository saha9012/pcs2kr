#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "types.hpp"

namespace elevator {

class Building;

class Elevator {
public:
    Elevator(int id, Building& building, int min_floor, int max_floor);

    ~Elevator();

    Elevator(const Elevator&) = delete;
    Elevator& operator=(const Elevator&) = delete;

    void start();
    void stop();

    void add_destination(int floor);
    void assign_hall_call(const CallRequest& call);

    int id() const { return id_; }
    int current_floor() const { return current_floor_.load(); }
    Direction direction() const;
    ElevatorState state() const;
    std::string status_line() const;

private:
    void run();
    void move_one_floor();
    void open_doors();
    int pick_next_floor();
    bool should_stop_at(int floor) const;
    void update_direction();

    int id_;
    Building& building_;
    int min_floor_;
    int max_floor_;

    std::atomic<int> current_floor_{0};
    std::atomic<bool> running_{false};
    std::atomic<ElevatorState> state_{ElevatorState::Idle};

    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::set<int> destinations_;
    Direction direction_{Direction::Idle};

    std::thread worker_;
};

}  // namespace elevator
