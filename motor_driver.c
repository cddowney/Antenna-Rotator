// File: motor_driver.c
// Author: Camden Downey
// Purpose: Fixed-loop motor position control functions & ISR

#include <stdio.h>
#include <xc.h>
#include <sys/attribs.h>
#include <math.h>
#include "init.h"
#include "motor_driver.h"
#include "uart.h"
#include "lcd.h"
#include "position_conversion.h"

#define AZ_HOME     PORTBbits.RB1
#define AZ_MAX      PORTBbits.RB0
#define EL_HOME     PORTCbits.RC9
#define EL_MAX      PORTCbits.RC6
#define DEADBAND    7

// Initialize setpoints to 0. These will be updated inside gs232a.c!
volatile int az_setpoint_counts = 0;
volatile int el_setpoint_counts = 0;

// Initialize e_stop flag
volatile char e_stop = 0;

// Homing sequence used on system startup
void home_motors(void)
{   
    // Disable Timer 2 interrupts
    IEC0bits.T2IE = 0;
    
    // drive motors towards the hall effect sensors on each axis
    // once the hall effect sensors are triggered: stop the motors and then reset position counter
    // update system status to say OK
    
    // Homing azimuth motor
    LATAbits.LATA0  = 0;    // in1 = 0 (CCW)
    LATAbits.LATA12 = 1;    // in2 = 1 (CCW)
    OC1RS = (PR3 * 0.70f);  // 70% duty cycle
    while(PORTCbits.RC11)
    {
        // if AZ home limit switch triggered, reverse motor direction (go CW)
        if(!AZ_HOME)
        {
            LATAbits.LATA0  = 1;    // in1 = 1
            LATAbits.LATA12 = 0;    // in2 = 0
        }
        // if AZ max limit switch triggered, reverse motor direction (go CCW))
        else if(!AZ_MAX)
        {
            LATAbits.LATA0  = 0;    // in1 = 0 (CCW)
            LATAbits.LATA12 = 1;    // in2 = 1 (CCW)
        }
        
    }
    
    // Azimuth motors homed
    // Stop motors
    LATAbits.LATA0  = 0;    // in1 = 0
    LATAbits.LATA12 = 0;    // in2 = 0
    delay_ms(20); // added to ensure motors fully stopped
    OC1RS   = 0;
    POS1CNT = degrees_to_counts(90);
    az_setpoint_counts = degrees_to_counts(90);
    
    delay_ms(500);
    
    // Homing elevation motor
    LATCbits.LATC7 = 0;     // in1 = 0 (down)
    LATBbits.LATB9 = 1;     // in2 = 1 (down)
    OC2RS = PR3 * 0.70f;    // 70% duty cycle
    while(PORTCbits.RC12) // while elevation switch not triggered
    {
        // if EL home limit switch triggered, reverse motor direction (go CW)
        if(!EL_HOME)
        {
            LATCbits.LATC7 = 1;     // in1 = 1 (up)
            LATBbits.LATB9 = 0;     // in2 = 0 (up)
        }
        // if EL max limit switch triggered, reverse motor direction (go CCW))
        else if(!EL_MAX)
        {
            LATCbits.LATC7 = 0;     // in1 = 0 (down)
            LATBbits.LATB9 = 1;     // in2 = 1 (down)
        }
    }
    
    // Elevation motor homed
    LATBbits.LATB9 = 0;
    LATCbits.LATC7 = 0;
    delay_ms(20); // added to ensure motors fully stopped
    OC2RS = 0;
    POS2CNT = degrees_to_counts(90);
    el_setpoint_counts = degrees_to_counts(90);
   
    delay_ms(200);  // added for stability
    
    // Homing sequence completed
    sys_status = STATUS_OK;
    IEC0bits.T2IE = 1;  // re-enable Timer 2 interrupts
    
}

// Timer2 ISR (runs @ 100Hz)
// This is to used to drive the motor control
// This ISR is long, but the computations are not intense
// It could be broken up into separate functions in a new release
void __ISR(_TIMER_2_VECTOR, IPL2SRS) Timer2ISR(void)
{
    IFS0bits.T2IF = 0; // clear interrupt flag
    
    // check status of e_stop
    e_stop = (PORTDbits.RD8 == 0); // active low
    
    // read current position of encoders
    int az_current_pos = POS1CNT;  // azimuth encoder count
    int el_current_pos = POS2CNT;  // elevation encoder count
    
    // if the e-stop is activated stop motor
    if(e_stop == 1)
    {
        // azimuth motor
        LATAbits.LATA0 = 0; // in1 = 0
        LATAbits.LATA12 = 0;// in2 = 0
        OC1RS = 0;
        
        // elevation motor
        LATCbits.LATC7 = 0; // in1 = 0
        LATBbits.LATB9 = 0; // in2 = 0
        OC2RS = 0;
        
        // send error message only once
        if(sys_status != STATUS_STOPPED)
        {
            sys_status = STATUS_STOPPED;
        }
   
        return;
    }
    
        // If we've hit azimuth LOW limit switch & trying to drive CCW
    if (!AZ_HOME && (az_setpoint_counts < az_current_pos))
    {
        LATAbits.LATA0 = 0;
        LATAbits.LATA12 = 0;
        OC1RS = 0;
        az_setpoint_counts = az_current_pos;
        sys_status = STATUS_STOPPED;
        return;
    }

    // If we've hit azimuth HIGH limit switch & trying to drive CW
    if (!AZ_MAX && (az_setpoint_counts > az_current_pos))
    {
        // stop motor
        LATAbits.LATA0 = 0;
        LATAbits.LATA12 = 0;
        OC1RS = 0;
        az_setpoint_counts = az_current_pos;
        sys_status = STATUS_STOPPED;
        return; 
    }

    // If we've hit elevation LOW limit switch & trying to drive down
    if (!EL_HOME && (el_setpoint_counts < el_current_pos))
    {
        // stop motor
        LATCbits.LATC7 = 0; // in1 = 0
        LATBbits.LATB9 = 0; // in2 = 0
        OC2RS = 0;
        el_setpoint_counts = el_current_pos;
        sys_status = STATUS_STOPPED;
        return;
    }

    // If we've hit elevation HIGH limit switch & trying to drive up
    if (!EL_MAX && (el_setpoint_counts > el_current_pos))
    {
        // stop motor
        LATCbits.LATC7 = 0; // in1 = 0
        LATBbits.LATB9 = 0; // in2 = 0
        OC2RS = 0;
        el_setpoint_counts = el_current_pos;
        sys_status = STATUS_STOPPED;
        return;
    }

    
    if(az_current_pos >= (uint32_t)ENCODER_COUNTS_PER_REV)
    {
        POS1CNT = 0;
        az_current_pos = 0;
        az_setpoint_counts = 0;
    }
    
    if(el_current_pos >= (uint32_t)ENCODER_COUNTS_PER_REV)
    {
        POS2CNT = 0;
        el_current_pos = 0;
        el_setpoint_counts = 0;
    }
    
    if(!e_stop && sys_status == STATUS_STOPPED)
    {
        // Reset setpoints so nothing moves immediately
        az_setpoint_counts = POS1CNT;
        el_setpoint_counts = POS2CNT;
        sys_status = STATUS_OK;
    }
    
    // Calculate needed duty cycle based on how close we are to the target
    int az_output = az_setpoint_counts - az_current_pos;
    int el_output = el_setpoint_counts - el_current_pos;
    
    // Compute magnitude of both outputs
    int az_mag = (az_output >= 0) ? az_output : -az_output; // azimuth magnitude
    int el_mag = (el_output >= 0) ? el_output : -el_output; // elevation magnitude
    
    // Azimuth manual movement switch checking & PWM setting
    if(!PORTAbits.RA7 & !AZ_MAX)  // AZ: clockwise
    {
        LATAbits.LATA0 = 1;
        LATAbits.LATA12 = 0;
        OC1RS = PR3 * 0.50f;
        az_setpoint_counts = POS1CNT;
    }
    else if(!PORTBbits.RB14 & !AZ_HOME) // AZ: counter clockwise
    {
        LATAbits.LATA0 = 0;
        LATAbits.LATA12 = 1;
        OC1RS = PR3 * 0.50f;
        az_setpoint_counts = POS1CNT;
    }
    else
    {
        // Determine direction azimuth motor must spin, if not in deadband
        if(az_mag < DEADBAND)
        {
            // stop motor
            LATAbits.LATA0 = 0; // in1 = 0
            LATAbits.LATA12 = 0;// in2 = 0
            OC1RS = 0;
        }
        else    
        {
            if(az_output > DEADBAND)
            {
                // Spin clock wise
                LATAbits.LATA0 = 1; // in1 = 1
                LATAbits.LATA12 = 0;// in2 = 0
            }
            else
            {
                // Spin counter clockwise
                LATAbits.LATA0 = 0; // in1 = 0
                LATAbits.LATA12 = 1;// in2 = 1
            }

            // Set Azimuth PWM duty cycle
            if (az_mag > 1000) {OC1RS = PR3;}               // 100% duty cycle
            else if (az_mag > 500) {OC1RS = PR3 * 0.80f;}   // 80% duty cycle
            else if (az_mag > 250) {OC1RS = PR3 * 0.65f;}   // 65% duty cycle
            else if (az_mag > 100) {OC1RS = PR3 * 0.50f;}   // 50% duty cycle
            else if (az_mag > 20) {OC1RS = PR3 * 0.40f;}    // 30% duty cycle
        }
    }
    
    // Elevation manual movement switch checking & PWM setting
    if(!PORTBbits.RB12 & !EL_MAX) // EL: Up
    {
        LATCbits.LATC7 = 1;
        LATBbits.LATB9 = 0;
        OC2RS = PR3 * 0.50f;
        el_setpoint_counts = POS2CNT;
    }
    else if(!PORTAbits.RA10 & !EL_HOME) // EL: down
    {
        LATCbits.LATC7 = 0;
        LATBbits.LATB9 = 1;
        OC2RS = PR3 * 0.50f;
        el_setpoint_counts = POS2CNT;
    }
    else
    {
        // Determine direction elevation motor must spin, if not in deadband
        if(el_mag < DEADBAND)
        {
            // stop motor
            LATCbits.LATC7 = 0; // in1 = 0
            LATBbits.LATB9 = 0; // in2 = 0
            OC2RS = 0;
        }      
        else
        {  
            if(el_output > DEADBAND)
            {
                // Rotate up
                LATCbits.LATC7 = 1; // in1 = 1
                LATBbits.LATB9 = 0; // in2 = 0
            }
            else
            {
                // Rotate down
                LATCbits.LATC7 = 0; // in1 = 0
                LATBbits.LATB9 = 1; // in2 = 1
            }

            // Set Elevation PWM duty cycle
            if (el_mag > 1000) {OC2RS = PR3;}               // 100% duty cycle
            else if (el_mag > 500) {OC2RS = PR3 * 0.80f;}   // 80% duty cycle
            else if (el_mag > 250) {OC2RS = PR3 * 0.65f;}   // 65% duty cycle
            else if (el_mag > 100) {OC2RS = PR3 * 0.50f;}   // 50% duty cycle
            else if (el_mag > 20) {OC2RS = PR3 * 0.40f;}    // 40% duty cycle
        }
    }

}