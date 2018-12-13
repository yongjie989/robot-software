#ifndef STRATEGY_GOALS_H
#define STRATEGY_GOALS_H

#include <goap/goap.hpp>
#include "state.h"

struct InitGoal : goap::Goal<RobotState> {
    virtual int distance_to(const RobotState& state) const
    {
        return goap::Distance().shouldBeFalse(state.arms_are_deployed);
    }
};

struct FirstPuckGoal : goap::Goal<RobotState> {
    virtual int distance_to(const RobotState& state) const
    {
        return goap::Distance().shouldBeFalse(state.arms_are_deployed);
    }
};

#endif /* STRATEGY_GOALS_H */
