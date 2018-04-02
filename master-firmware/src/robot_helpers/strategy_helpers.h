#ifndef STRATEGY_HELPERS_H
#define STRATEGY_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <msgbus/messagebus.h>
#include "trajectory_manager/trajectory_manager.h"
#include "blocking_detection_manager/blocking_detection_manager.h"
#include "base/base_controller.h"
#include "math/lie_groups.h"
#include "robot_helpers/eurobot2018.h"

/** Team color
 */
enum strat_color_t {
    YELLOW=0,
    BLUE
};

/** Lever side */
enum lever_side_t {
    LEVER_SIDE_LEFT,
    LEVER_SIDE_RIGHT,
};

/** Compute the symmetrical position depending on color
 */
#define MIRROR_X(color, x) (color == YELLOW ? (x) : 3000. - (x))
#define MIRROR_A(color, a_deg) (color == YELLOW ? (a_deg) : 180. - (a_deg))
#define MIRROR(color, value) (color == YELLOW ? (value) : - (value))

/** Auto position robot at requested location, and ensure the correct
 *  position is reached by aligning against walls.
 */
void strategy_auto_position(int32_t x, int32_t y, int32_t heading, enum strat_color_t robot_color);

/** Align on y axis */
void strategy_align_y(int32_t y);

/** Compute cube position given pose of cubes block in table frame and cube color */
point_t strategy_cube_pos(se2_t cubes_pose, enum cube_color color);

/** Compute closest position to pickup cubes */
se2_t strategy_closest_pose_to_pickup_cubes(se2_t current_pose, se2_t cubes_pose);

#ifdef __cplusplus
}
#endif

#endif /* STRATEGY_HELPERS_H */
