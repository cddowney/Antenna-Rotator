// File: gs232a.c
// Author: Camden Downey
// Purpose: Parse incoming GS-232A commands from UART

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <xc.h>
#include "gs232a.h"
#include "motor_driver.h"
#include "position_conversion.h"
#include "uart.h"

void GS232A_Parse(const char *cmd)
{      
    // Stop command
    if(strcmp(cmd, "S") == 0)
    {
        // To add: Stop motors
        az_setpoint_counts = POS1CNT;
        el_setpoint_counts = POS2CNT;
        UART1_WriteString("OK\r\n");
        return;
    }
    
    // C2: return azimuth and elevation: "+0aaa+0eee\r\n"
    if(strcmp(cmd, "C2") == 0)
    {
        int az = (int)counts_to_degrees((float)POS1CNT);
        int el = (int)counts_to_degrees((float)POS2CNT);
        char buf[32];
        sprintf(buf, "+0%03d+0%03d\r\n", az, el);
        UART1_WriteString(buf);
        return;
    }
    
    // C: Report only azimuth
    if(strcmp(cmd, "C") == 0)
    {
        int az = (int)counts_to_degrees((float)POS1CNT);
        char buf[16];
        sprintf(buf, "+0%03d\r\n", az);
        UART1_WriteString(buf);
        return;
    }
    
    // B: Report only elevation
    if(strcmp(cmd, "B") == 0)
    {
        int el = (int)counts_to_degrees((float)POS2CNT);
        char buf[16];
        sprintf(buf, "+0%03d\r\n", el);
        UART1_WriteString(buf);
        return;
    }
    
     // W: set azimuth and elevation: "Waaa eee"
    if (cmd[0] == 'W')
    {
        
        int az_deg, el_deg;
        if (sscanf(cmd + 1, "%d %d", &az_deg, &el_deg) == 2)
        {
            az_setpoint_counts = (int)degrees_to_counts((float)az_deg);
            el_setpoint_counts = (int)degrees_to_counts((float)el_deg);
            UART1_WriteString("\r");
        }
        else
        {
            UART1_WriteString("? >\r\n");
        }
        return;
    }

    // M: set azimuth only: "Maaa"
    if (cmd[0] == 'M')
    {
        int az_deg = atoi(cmd + 1);
        az_setpoint_counts = (int)degrees_to_counts((float)az_deg);
        UART1_WriteString("\r");
        return;
    }
    
    // Unknown / invalid command sent
    UART1_WriteString("? >\r\n");
}