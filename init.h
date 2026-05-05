// File name: init.h
// Purpose: Function prototypes for init.c
// Author: Camden Downey

#ifndef INIT_H
#define INIT_H

// ----------- System Unlock Macros -----------
#define SYS_LOCK           SYSKEY = 0x00000000
#define SYS_UNLOCK_STEP1   SYSKEY = 0xAA996655
#define SYS_UNLOCK_STEP2   SYSKEY = 0x556699AA

// ----------- System Initialization -----------
void sys_init(void);

// ----------- Peripheral Initialization -----------
void UART_init(void);
void SPI_init(void);
void I2C_init(void);
void LCD_init(void);
void timer2_init(void);
void timer3_init(void);
void oc1_init(void);
void oc2_init(void);

// ----------- Motor Initialization -----------
void azimuth_motor_init(void);
void elevation_motor_init(void);

// ----------- QEI Initialization -----------
void QEI1_init(void);
void QEI2_init(void);

// ----------- ISR prototypes -----------
void isr_timer_tick(void);

#endif
