#ifndef STRATEGY_ACTIONS_H
#define STRATEGY_ACTIONS_H

#include <goap/goap.hpp>
#include "state.h"

/** Number of states goap can visit before giving up. Increasing it means a
 * solution is found on more complex problems at the expense of RAM use. */
#define GOAP_SPACE_SIZE 40

namespace actions {

struct IndexArms : public goap::Action<RobotState> {
    bool can_run(const RobotState& state)
    {
        (void)state;
        return true;
    }

    void plan_effects(RobotState& state)
    {
        state.arms_are_indexed = true;
    }
};

struct RetractArms : public goap::Action<RobotState> {
    bool can_run(const RobotState& state)
    {
        return state.arms_are_indexed;
    }

    void plan_effects(RobotState& state)
    {
        state.arms_are_deployed = false;
    }
};
} // namespace actions

#endif /* STRATEGY_ACTIONS_H */
