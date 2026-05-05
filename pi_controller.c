// File: pi_controller.c
// Author: Camden Downey

// Context:
// The two PI controllers will compute a value that we write into the output compare modules
//  in order to determine the duty cycle. We will take the magnitude of the output value of
//  the PI controllers for duty cycle, and the sign (+/-) will signal when we need to flip
//  the motor input pins on the H-bridge. (pos: in1 = 1, in2 = 0, negative: in1 = 0, in2 = 1)

#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>
#include <math.h>
#include "pi_controller.h"
#include "position_conversion.h"

#define INTEGRAL_MAX 1000.0f
#define INTEGRAL_MIN -1000.0f
#define OUTPUT_MAX 1.0f
#define OUTPUT_MIN -1.0f

// Define global pi controllers
PIController az_controller;
PIController el_controller;

// Function to initialize the PI controller
void init_PI(PIController* controller, float Kp, float Ki, float setpoint, float sample_time)
{
    controller->Kp = Kp;
    controller->Ki = Ki * sample_time; // scale Ki by sample time for consistency
    controller->setpoint = setpoint;
    controller->integral_sum = 0.0;
    controller->control_output = 0.0;
    controller->sample_time = sample_time;
}

void init_motor_controllers(void)
{
    float Ts = 0.01f; // 0.01s sampling rate aka 100Hz
    
    // Set gains here: controller, Kp, Ki, setpoint, sample time
    init_PI(&az_controller, 1.0f, 0.3f, 0.0f, Ts);
    init_PI(&el_controller, 1.2f, 0.2f, 0.0f, Ts);
}

// Function to compute the new control output
float compute_PI(PIController* controller, float actual_value)
{
    // Calculate the error
    float error = controller->setpoint-actual_value;
    
    // Accumulate error for integral portion
    controller->integral_sum += error;
    
    // Clamp the integral term to prevent overshoot, slow recovery, etc
    if (controller->integral_sum > INTEGRAL_MAX)
    {
        controller->integral_sum = INTEGRAL_MAX;
    }
    else if (controller->integral_sum < INTEGRAL_MIN)
    {
        controller->integral_sum = INTEGRAL_MIN;
    }
    
    // Calculate the proportional component
    float proportional_part = controller->Kp * error;
    
    // Calculate the integral component
    float integral_part = controller->Ki * controller->integral_sum;
    
    // Combine proportional and integral components to get control output
    controller->control_output = proportional_part + integral_part;
    
    // Limit output
    if (controller->control_output > OUTPUT_MAX)
    {
        controller->control_output = OUTPUT_MAX;
    }
    else if (controller->control_output < OUTPUT_MIN)
    {
        controller->control_output = OUTPUT_MIN;
    }
    
    // Return the control output
    return controller->control_output;
}


// Timer2 ISR (runs @ 100Hz)
// This is to used to drive the two PI controllers for the motor position control
void __ISR(_TIMER_2_VECTOR, IPL2SRS) Timer2ISR(void)
{
    IFS0bits.T2IF = 0; // clear interrupt flag
    
    float az_current_pos = (float)POS1CNT;  // read current azimuth encoder count
    float az_output = compute_PI(&az_controller, az_current_pos); // calculate PWM needed
    
    float el_current_pos = (float)POS2CNT; // read current elevation encoder count
    float el_output = compute_PI(&el_controller, el_current_pos); // calculate PWM needed
    
    // Determine direction elevation motor must spin
    if(az_output >= 0)
    {
        // clockwise direction
        // in1 = 1
        LATAbits.LATA0 = 1;
        
        // in2 = 0
        LATAbits.LATA12 = 0;
    }
    else
    {
        // counter-clockwise direction
        // input 1; RPA0
        LATAbits.LATA0 = 0;
        // input 2; RPA12
        LATAbits.LATA12 = 1;
    }
    
    // Determine direction azimuth motor must spin
    if(el_output >= 0)
    {
        // clockwise direction
        // in1; RPB8
        LATBbits.LATB8 = 1;
        // in2; RPB9
        LATBbits.LATB9 = 0;
    }
    else
    {
        // counter-clockwise direction
        // in1; RPB8
        LATBbits.LATB8 = 0;
        // in2; RPB9
        LATBbits.LATB9 = 1;
    }
    
    // Compute magnitude of both outputs
    float az_mag = fabsf(az_output); // azimuth magnitude
    float el_mag = fabsf(el_output); // elevation magnitude
    
    // Limit / clamp the output
    if (az_mag > 1.0f) az_mag = 1.0f; // clamp PWM
    if (el_mag > 1.0f) el_mag = 1.0f; // clamp PWM
   
    // Update duty cycle of each PWM signal
    OC1RS = (uint16_t)(az_mag * PR3);
    OC2RS = (uint16_t)(el_mag * PR3);
}