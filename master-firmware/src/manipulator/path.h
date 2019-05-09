#ifndef MANIPULATOR_PATH_H
#define MANIPULATOR_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MANIPULATOR_INIT = 0,
    MANIPULATOR_RETRACT,
    MANIPULATOR_DEPLOY,
    MANIPULATOR_DEPLOY_FULLY,
    MANIPULATOR_LIFT_HORZ,
    MANIPULATOR_PICK_HORZ,
    MANIPULATOR_PICK_VERT,
    MANIPULATOR_LIFT_VERT,
    MANIPULATOR_PICK_GOLDONIUM,
    MANIPULATOR_LIFT_GOLDONIUM,
    MANIPULATOR_SCALE,
    MANIPULATOR_PUT_ACCELERATOR,
    MANIPULATOR_PUT_ACCELERATOR_DOWN,
    MANIPULATOR_STORE_FRONT_SAFE,
    MANIPULATOR_STORE_FRONT_0,
    MANIPULATOR_STORE_FRONT_1,
    MANIPULATOR_STORE_FRONT_2,
    MANIPULATOR_STORE_FRONT_STORE,
    MANIPULATOR_STORE_FRONT_HIGH,
    MANIPULATOR_STORE_FRONT_HIGH_1,
    MANIPULATOR_STORE_FRONT_LOW,
    MANIPULATOR_STORE_BACK_1,
    MANIPULATOR_STORE_BACK_2,
    MANIPULATOR_STORE_BACK_3,
    MANIPULATOR_STORE_BACK_4,
    MANIPULATOR_STORE_BACK_STORE,
    MANIPULATOR_STORE_BACK_HIGH,
    MANIPULATOR_STORE_BACK_LOW,
    MANIPULATOR_COUNT // Dummy, used for last element
} manipulator_state_t;

#ifdef __cplusplus
}
#endif

#endif /* MANIPULATOR_PATH_H */
