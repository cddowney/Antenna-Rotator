// File: pi_controller.h
// Author: Camden Downey

#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include <stdint.h>

// Structure for PI controller
typedef struct 
{
    float Kp;               // proportional gain
    float Ki;               // integral gain (already scaled by sample time)
    float setpoint;         // desired target
    float integral_sum;     // accumulated error
    float control_output;   // PI output
    float sample_time;      // dt in seconds
} PIController;

// Functions
void init_PI(PIController* controller, float Kp, float Ki, float setpoint, float sample_time);
float compute_PI(PIController* controller, float actual_value);
void init_motor_controllers(void);

// Global instances (if you want)
extern PIController az_controller;
extern PIController el_controller;

#endif
