/* 
 * File:   main.c
 * Author: Camden Downey
 *
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "uart.h"
#include "init.h"
#include "gs232a.h"




int main(void)
{
    sys_init();
    
    //UART1_WriteString("Testing UART Transmission\r\n");

    
    //GS232A_Parse("AZ030\r\n");
    
    //char line[64];

    while (1)
    {
        /*
        // Only read when a full line ending in '\r' is available
        if (UART1_LineAvailable())
        {
            int len = UART1_ReadLine(line, sizeof(line));

            if (len > 0)
            {
                GS232A_Parse(line);
            }
        }
        */
        
        if (U1STAbits.URXDA)
        {
            volatile uint8_t b = U1RXREG;  // read it so it clears
            (void)b;
            // set a dummy variable or toggle a pin here if you want
        }

    }
}

