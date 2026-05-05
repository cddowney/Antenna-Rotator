// File: motor_driver.c
// Author: Camden Downey

// Context:
// This contains the logic to drive the motors through the use of an H-Bridge
// and two adjustable PWM signals to change the speed of rotation.

#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>
#include <math.h>
#include "position_conversion.h"


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