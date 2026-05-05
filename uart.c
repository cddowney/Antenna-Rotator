// File: uart.c
// Author: Camden Downey
// Purpose: UART logic handling; TX and RX. Interrupt based; non-polling

#include <xc.h>
#include <sys/attribs.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "uart.h"

// Configuration macros
#define UART_BUFFER_SIZE   256
#define UART_LINE_BUFFER_SIZE 64

static volatile bool rxLineReady = false;

// Setup RX Ring Buffer
static volatile uint8_t rxBuffer[UART_BUFFER_SIZE];
static volatile uint16_t rxHead = 0;
static volatile uint16_t rxTail = 0;

// Setup TX ring buffer
static volatile uint8_t txBuffer[UART_BUFFER_SIZE];
static volatile uint16_t txHead = 0;
static volatile uint16_t txTail = 0;

// RX Interrupt Service Routine
void __ISR(_UART1_RX_VECTOR, IPL3AUTO) UART1_RX_ISR(void)
{
    // Clear overrun if it happened
    if (U1STAbits.OERR)
        U1STAbits.OERR = 0;

    while (U1STAbits.URXDA)
    {
        uint8_t data = U1RXREG;

        uint16_t nextHead = (rxHead + 1) % UART_BUFFER_SIZE;

        if (nextHead != rxTail)
        {
            rxBuffer[rxHead] = data;
            rxHead = nextHead;
            
            if(data == '\r')
            {
                rxLineReady = true;
            }
        }
    }

    IFS1CLR = _IFS1_U1RXIF_MASK;
}

// TX ISR
void __ISR(_UART1_TX_VECTOR, IPL3AUTO) UART1_TX_ISR(void)
{
    if (txHead == txTail)
    {
        IEC1CLR = _IEC1_U1TXIE_MASK;   // disable interrupt
    }
    else
    {
        U1TXREG = txBuffer[txTail];
        txTail = (txTail + 1) % UART_BUFFER_SIZE;
    }

    IFS1CLR = _IFS1_U1TXIF_MASK;
}

// Basic RX Functions
// Returns true if data available
bool UART1_Available(void)
{
    return (rxHead != rxTail);
}

// Reads one byte (non-blocking)
uint8_t UART1_ReadByte(void)
{
    uint8_t data = 0;

    if (rxHead != rxTail)
    {
        data = rxBuffer[rxTail];
        rxTail = (rxTail + 1) % UART_BUFFER_SIZE;
    }

    return data;
}

// Send single byte
void UART1_WriteByte(uint8_t b)
{
    uint16_t nextHead = (txHead + 1) % UART_BUFFER_SIZE;

    // Wait if buffer full
    while (nextHead == txTail);

    // Store byte
    txBuffer[txHead] = b;
    txHead = nextHead;

    // Enable TX interrupt so ISR starts sending
    IEC1SET = _IEC1_U1TXIE_MASK;

    // If TX shift register is empty, kickstart
    if (U1STAbits.TRMT)
    {
        U1TXREG = txBuffer[txTail];
        txTail = (txTail + 1) % UART_BUFFER_SIZE;
    }
}


// Send null-terminated string
void UART1_WriteString(const char *str)
{
    while (*str)
    {
        UART1_WriteByte(*str++);
    }
}

// Checks if a full line (ending in '\r') exists
bool UART1_LineAvailable(void)
{
    return rxLineReady;
}

// Reads a full line into dest
// Returns length of line (excluding '\r')
int UART1_ReadLine(char *dest, int maxLen)
{
    if (!rxLineReady)
        return 0;

    int count = 0;

    while (UART1_Available() && count < (maxLen - 1))
    {
        uint8_t c = UART1_ReadByte();

        if (c == '\n')
            continue;   // ignore \n

        if (c == '\r')
            break;      // stop when line-return is hit

        dest[count++] = c;
    }

    dest[count] = '\0';
    rxLineReady = false;
    return count;
}