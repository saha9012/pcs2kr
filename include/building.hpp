#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "elevator.hpp"
#include "types.hpp"

namespace elevator {

class Building {
public:
    Building(int floors, int elevators);

    int floor_count() const { return floor_count_; }
    int elevator_count() const { return static_cast<int>(elevators_.size()); }

    void submit_call(const CallRequest& call);
    bool pop_next_call(CallRequest& out);
    void assign_call_to_elevator(const CallRequest& call, int elevator_id);
    Elevator* elevator_by_id(int id);
    const Elevator* elevator_by_id(int id) const;

    std::vector<std::string> elevator_status() const;

    void on_elevator_arrived(int elevator_id, int floor);
    bool try_claim_hall_call(int floor, CallType type, int elevator_id);

    void start_elevators();
    void stop_elevators();

private:
    int floor_count_;
    std::vector<std::unique_ptr<Elevator>> elevators_;

    mutable std::mutex calls_mutex_;
    std::queue<CallRequest> pending_calls_;
    std::vector<std::vector<bool>> hall_up_;
    std::vector<std::vector<bool>> hall_down_;
};

}  // namespace elevator
