// File: gs232a.c
// Author: Camden Downey
// Purpose: Parse incoming GS-232A commands from UART

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <xc.h>
#include "gs232a.h"
#include "pi_controller.h"
#include "position_conversion.h"
#include "uart.h"

void GS232A_Parse(const char *cmd)
{
    // Azimuth command
    if(strncmp(cmd, "AZ", 2) == 0)
    {
        float deg = atof(cmd + 2);
        az_controller.setpoint = degrees_to_counts(deg);
        //UART1_WriteString("OK\r\n");
        return;
    }
    
    // Elevation command
    if(strncmp(cmd, "EL", 2) == 0)
    {
        float deg = atof(cmd + 2);
        el_controller.setpoint = degrees_to_counts(deg);
        UART1_WriteString("OK\r\n");
        return;
    }
    
    // Report azimuth command
    if(strncmp(cmd, "C2", 2) == 0)
    {
        float az_degree = counts_to_degrees((float)(POS1CNT));
        
        char tempBuff[16];
        sprintf(tempBuff, "%03d\r\n", (int)az_degree);
        UART1_WriteString(tempBuff);
    }
    
    // Report elevation command
    if(strncmp(cmd, "C3", 2) == 0)
    {
        float el_degree = counts_to_degrees((float)(POS2CNT));
        
        char tempBuff[16];
        sprintf(tempBuff, "%03d\r\n", (int)el_degree);
        UART1_WriteString(tempBuff);
    }
    
    // Stop command
    if(strncmp(cmd, "S", 1) == 0)
    {
        // Stop motors
        UART1_WriteString("OK\r\n");
        return;
    }
    
    // Unknown / invalid command sent
    UART1_WriteString("?\r\n");
    
}