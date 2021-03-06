#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <ch.h>
#include <parameter/parameter.h>
#include "unix_timestamp.h"

#define MOTOR_ID_MAX_LEN 24
#define MOTOR_ID_MAX_LEN_WITH_NUL (MOTOR_ID_MAX_LEN + 1) // terminated C string buffer

#define MOTOR_CONTROL_MODE_DISABLED 0
#define MOTOR_CONTROL_MODE_POSITION 1
#define MOTOR_CONTROL_MODE_VELOCITY 2
#define MOTOR_CONTROL_MODE_TORQUE 3
#define MOTOR_CONTROL_MODE_VOLTAGE 4

#define MOTOR_STREAMS_NB_VALUES 10
#define MOTOR_STREAM_CURRENT 0
#define MOTOR_STREAM_CURRENT_SETPT 1
#define MOTOR_STREAM_MOTOR_VOLTAGE 2
#define MOTOR_STREAM_VELOCITY 3
#define MOTOR_STREAM_VELOCITY_SETPT 4
#define MOTOR_STREAM_POSITION 5
#define MOTOR_STREAM_POSITION_SETPT 6
#define MOTOR_STREAM_INDEX 7
#define MOTOR_STREAM_MOTOR_ENCODER 8
#define MOTOR_STREAM_MOTOR_TORQUE 9

struct pid_parameter_s {
    parameter_namespace_t root;
    parameter_t kp;
    parameter_t ki;
    parameter_t kd;
    parameter_t ilimit;
};

typedef struct {
    char id[MOTOR_ID_MAX_LEN + 1];
    int can_id;
    binary_semaphore_t lock;

    float update_period;
    int control_mode;
    union {
        float position;
        float velocity;
        float torque;
        float voltage;
    } setpt;

    struct {
        parameter_namespace_t root;
        parameter_namespace_t control;
        parameter_t mode; // one of "pot-servo", "enc-servo", "enc-periodic", "dual-enc-periodic"
        struct pid_parameter_s position_pid;
        struct pid_parameter_s velocity_pid;
        struct pid_parameter_s current_pid;
        parameter_t torque_limit;
        parameter_t velocity_limit;
        parameter_t acceleration_limit;
        parameter_t low_batt_th;

        parameter_namespace_t motor;
        parameter_t torque_constant;

        parameter_namespace_t encoders;
        parameter_namespace_t primary;
        parameter_t p;
        parameter_t q;
        parameter_t ticks_per_rev;

        parameter_namespace_t stream; // frequencies of the streams
        parameter_t current_pid_stream;
        parameter_t velocity_pid_stream;
        parameter_t position_pid_stream;
        parameter_t index_stream;
        parameter_t encoder_pos_stream;
        parameter_t motor_pos_stream;
        parameter_t motor_torque_stream;
    } config;

    struct {
        uint32_t change_status;
        float values[MOTOR_STREAMS_NB_VALUES];
        uint32_t value_stream_index_update_count;
    } stream;

    void* can_driver;

} motor_driver_t;

#ifdef __cplusplus
extern "C" {
#endif

// initialize a new motor driver
// - creates a parameter namespace actuator_id for the new driver in the
//   namespace ns
// - the actuator id is stored internally (copied)
void motor_driver_init(motor_driver_t* d,
                       const char* actuator_id,
                       parameter_namespace_t* ns);

// returns a pointer to the stored id string
const char* motor_driver_get_id(motor_driver_t* d);

void motor_driver_set_position(motor_driver_t* d, float position);
void motor_driver_set_velocity(motor_driver_t* d, float velocity);
void motor_driver_set_torque(motor_driver_t* d, float torque);
void motor_driver_set_voltage(motor_driver_t* d, float voltage);
void motor_driver_disable(motor_driver_t* d);

#define CAN_ID_NOT_SET 0xFFFF
int motor_driver_get_can_id(motor_driver_t* d);
void motor_driver_set_can_id(motor_driver_t* d, int can_id);

void motor_driver_lock(motor_driver_t* d);
void motor_driver_unlock(motor_driver_t* d);

// the get control mode and the corresponding setpoint must be called in a
// motor_driver_lock/unlock critical section
int motor_driver_get_control_mode(motor_driver_t* d);

float motor_driver_get_position_setpt(motor_driver_t* d);
float motor_driver_get_velocity_setpt(motor_driver_t* d);
float motor_driver_get_torque_setpt(motor_driver_t* d);
float motor_driver_get_voltage_setpt(motor_driver_t* d);

void motor_driver_set_stream_value(motor_driver_t* d, uint32_t stream, float value);
uint32_t motor_driver_get_stream_change_status(motor_driver_t* d);
float motor_driver_get_and_clear_stream_value(motor_driver_t* d, uint32_t stream);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_DRIVER_H */
