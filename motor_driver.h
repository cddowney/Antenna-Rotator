// File: motor_driver.h
// Author: Camden Downey
#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "lcd.h"

extern volatile int az_setpoint_counts;
extern volatile int el_setpoint_counts;
extern volatile SystemStatus sys_status;
extern volatile char e_stop;
void home_motors(void);


#endif