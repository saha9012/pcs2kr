#pragma once

#include "building.hpp"
#include "types.hpp"

namespace elevator {

class Dispatcher {
public:
    explicit Dispatcher(Building& building);

    void dispatch_pending();

private:
    int select_elevator(const CallRequest& call) const;
    int score_elevator(const Elevator& e, const CallRequest& call) const;

    Building& building_;
};

}  // namespace elevator
