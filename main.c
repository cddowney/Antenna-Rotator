// File:   main.c
// Author: Camden Downey

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/attribs.h>
#include "uart.h"
#include "spi.h"
#include "lcd.h"
#include "init.h"
#include "gs232a.h"
#include "motor_driver.h"
#include "position_conversion.h"

// System status flag
volatile SystemStatus sys_status = STATUS_OK;

int main(void)
{
    // Initialize hardware components and interrupts
    sys_init();

    // Set initial LCD screen
    IEC0bits.T2IE = 0;  // disable Timer 2 interrupts
    LCD_fillScreen(COLOR_BLACK);
    LCD_updateStatus(STATUS_HOMING, 0.0f, 0.0f);
    IEC0bits.T2IE = 1; // re-enable Timer 2 interrupts
    
    // Get motors into a home position before continuing
    // This is a blocking function on purpose
    home_motors();
    
    // Motors have been homed
    IEC0bits.T2IE = 0;
    LCD_fillScreen(COLOR_BLACK);
    LCD_updateStatus(STATUS_OK, 0.0f, 0.0f);
    IEC0bits.T2IE = 1;
    
    char line[64]; // holds a UART command
    uint32_t last_update = _CP0_GET_COUNT(); // used to count to 500ms for lcd
    
    while (1)
    {
        // Only read when a full line ending in '\r' is available
        if (UART1_LineAvailable())
        {
            int len = UART1_ReadLine(line, sizeof(line));

            if (len > 0)
            {
                GS232A_Parse(line);
            }
        }
        
        uint32_t now = _CP0_GET_COUNT();
        if ((now - last_update) >= 30000000)
        {
            last_update = now;

            float az = counts_to_degrees((float)POS1CNT);
            float el = counts_to_degrees((float)POS2CNT);
            
            int dummy = sys_status;
            (void)dummy;

            LCD_updateStatus(sys_status, az, el);
        }
    }
}

