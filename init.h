// File name: init.h
// Purpose: Function prototypes for init.c
// Author: Camden Downey

#ifndef INIT_H
#define INIT_H

// ----------- System Unlock Macros -----------
#define SYS_LOCK            SYSKEY = 0x00000000
#define SYS_UNLOCK_STEP1    SYSKEY = 0x00000000   // force lock first
#define SYS_UNLOCK_STEP2    SYSKEY = 0xAA996655   // key 1
#define SYS_UNLOCK_STEP3    SYSKEY = 0x556699AA   // key 2

// ----------- Delay loop -----------
static inline void delay_ms(unsigned int ms)
{
    unsigned int start = _CP0_GET_COUNT();
    unsigned int ticks = ms * 60000;  // 60,000 ticks per ms at 120MHz CPU
    while((_CP0_GET_COUNT() - start) < ticks);
}

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
void estop_init(void);

// ----------- Motor Initialization -----------
void azimuth_motor_init(void);
void elevation_motor_init(void);

// ----------- QEI Initialization -----------
void QEI1_init(void);
void QEI2_init(void);

// ----------- ISR prototypes -----------
void isr_timer_tick(void);

#endif
