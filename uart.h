#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

//Transmit (TX)
void UART1_WriteByte(uint8_t b);
bool UART1_TxReady(void);
void UART1_WriteString(const char *str);

// Receive (RX)
bool UART1_Available(void);
uint8_t UART1_ReadByte(void);

// Line-Based Support
// terminate lines when we reach '\r'
// ignore '\n'
bool UART1_LineAvailable(void);
int UART1_ReadLine(char *dest, int maxLen);

#endif
