---
title: Writing a proper init system for our firmwares.
author: "@antoinealb"
---

# Background

Over the years, we increased the number of subsystems running on the robot.
We now have a large number of threads which must be running in order to have a working robot.
Bringing up those poses its own set of challenges, as the dependencies between systems are not tracked.
This lead to a `main()` filled with delays and one off solutions based on semaphores.
The result is complex to follow, takes long to boot and is also complicated to extend: changing dependencies between services requires rework of the main, possibly introducing race conditions.

Another issue with that situation is that it makes it hard to write a partially working robot.
For example, if there is no connected touchscreen, the current implementation will panic.
It would be better if the GUI system reported its state as failing, and functionality not relying on it would keep working.

In the outside world, we can find a similar situation in the world of operating systems.
Those usually have an *init system*, such as systemd, which tracks the dependencies between services.
It can then start those in the correct order by sorting the service dependency graph using *topological sorting*.

Our subsystems usually have a shared structure, with every step being optional:

1. Create shared ressources such as msgbus topics and parameters.
2. Init some hardware.
3. Wait for parameters to be filled.
4. Perform their function .
5. Cleanup and exit.

Those steps naturally define several possible state for a system, with one or several additional states for error conditions.

# Proposed solution

* systems identified by a name
* each system have a state (enum)
* each system can change its own state
* each system can declare some dependencies

a complication comes from the fact that we have several stages and each state has some dependencies
this define a graph, where each node is a (service, state) pair.

How do you enforce dependencies ?

Best if dependency rules can be stored in flash with minimal runtime state.

example service set:

```
Service {
  name: "strategy",
  states: {
    "ressources_created": {
      depends: {
        "parameter_tree": "ressources_created",
      }
    }
    "running": {
      depends: {
       "parameter_tree": "running",
      }
    }
  }
}
```

api:

```
// tells that we have done everything needed and we are now in state RUNNING
init_transition("strategy", RUNNING);

// wait for all dependencies required to go into state RUNNING
wait_conditions_for_runstate(RUNNING);
```

idea: check consistency at compile time ?
idea: wait on global state, condvar to detect changes ?
    or even just looping at first.

# Proposed rollout plan

The UWB board will be the first converted to this system, because it has enough complexity to highlight corner cases, without representing something big like master-firmware.

# Alternatives considered


