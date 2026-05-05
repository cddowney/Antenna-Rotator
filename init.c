// File name: init.c
// Purpose: Master initialization file
// Author: Camden Downey

// PIC32MK0512MCJ048 Configuration Bit Settings
// DEVCFG3
#pragma config USERID = 0xFFFF          // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config PWMLOCK = OFF            // PWM IOxCON lock (PWM IOxCON register writes accesses are not locked or protected)
#pragma config PGL1WAY = OFF             // Permission Group Lock One Way Configuration bit (Allow multiple reconfiguration)
#pragma config PMDL1WAY = OFF            // Peripheral Module Disable Configuration (Allow multiple reconfiguration)
#pragma config IOL1WAY = OFF             // Peripheral Pin Select Configuration (Allow multiple reconfiguration)
#pragma config FUSBIDIO1 = OFF          // USB1 USBID Selection (USBID pin is controlled by the port function)
#pragma config FVBUSIO1 = OFF           // USB2 VBUSON Selection bit (VBUSON pin is controlled by the port function)
// DEVCFG2
#pragma config FPLLIDIV = DIV_1         // System PLL Input Divider (1x Divider)
#pragma config FPLLRNG = RANGE_5_10_MHZ // System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK = PLL_FRC       // System PLL Input Clock Selection (FRC is input to the System PLL)
#pragma config FPLLMULT = MUL_30        // System PLL Multiplier (PLL Multiply by 30)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (2x Divider)
#pragma config BORSEL = HIGH            // Brown-out trip voltage (BOR trip voltage 2.1v (Non-OPAMP deviced operation))
#pragma config UPLLEN = OFF             // USB PLL Enable (USB PLL Disabled)
// DEVCFG1
#pragma config FNOSC = SPLL             // Oscillator Selection Bits
#pragma config DMTINTV = WIN_127_128    // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN = ON             // Secondary Oscillator Enable (Enable Secondary Oscillator)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM = CSECME           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS = NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT = DMT31           // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN = OFF             // Deadman Timer Enable (Deadman Timer is enabled)
// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config TRCEN = OFF              // Trace Enable (Trace features in the CPU are enabled)
#pragma config BOOTISA = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON = ECC_DECC_DISABLE_ECCON_WRITABLE// Dynamic Flash ECC Configuration Bits (ECC and Dynamic ECC are disabled (ECCCON<1:0> bits are writable))
#pragma config FSLEEP = OFF             // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER = PG_ALL          // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR = MCLR_NORM        // Soft Master Clear Enable (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN = G3            // Secondary Oscillator Gain Control bits (Gain is G3)
#pragma config SOSCBOOST = ON           // Secondary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCGAIN = G3            // Primary Oscillator Coarse Gain Control bits (Gain Level 3 (highest))
#pragma config POSCBOOST = ON           // Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCFGAIN = G3           // Primary Oscillator Fine Gain Control bits (Gain is G3)
#pragma config POSCAGCDLY = AGCRNG_x_25ms// AGC Gain Search Step Settling Time Control (Settling time = 25ms x AGCRNG)
#pragma config POSCAGCRNG = ONE_X       // AGC Lock Range bit (Range 1x)
#pragma config POSCAGC = Automatic      // Primary Oscillator Gain Control bit (Automatic Gain Control for Oscillator)
#pragma config EJTAGBEN = NORMAL        // EJTAG Boot Enable (Normal EJTAG functionality)
// DEVCP
#pragma config CP = OFF                 // Code Protect (Protection Disabled)
// SEQ
#pragma config TSEQ = 0xFFFF            // Boot Flash True Sequence Number (Enter Hexadecimal value)
#pragma config CSEQ = 0xFFFF            // Boot Flash Complement Sequence Number (Enter Hexadecimal value)




#include <xc.h>
#include <sys/attribs.h>
#include "init.h"
#include "pi_controller.h"


void sys_init(void)
{
    // Configure PBCLK2 = SYSCLK / 1 = 60 MHz
    PB2DIVbits.PBDIV = 0;   // divide by 1
    PB2DIVbits.ON = 1;      // enable PBCLK2

   
    // Unlock SYSKEY
    SYS_UNLOCK_STEP1;
    SYS_UNLOCK_STEP2;
    
    // Unlock PPS
    CFGCONbits.IOLOCK = 0;
    
    
    
    // Disable analog on all pins, only enable pins when needed.
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    
    oc1_init();
    //oc2_init();
    timer2_init();
    timer3_init();
    QEI1_init();
    //QEI2_init();
    azimuth_motor_init();
    elevation_motor_init();
    init_motor_controllers();
    //SPI_init();
    UART_init();
    //I2C_init();
    //LCD_init();
    
    __builtin_enable_interrupts();  // global interrupts enabled
    
    // Lock PPS
    CFGCONbits.IOLOCK = 1;
    
    // Lock SYSKEY
    SYS_LOCK;
}

// ---------------- UART Setup ----------------
void UART_init(void)
{
    // --- Disable UART before configuration ---
    U1MODEbits.ON = 0;

    // --- UART mode configuration ---
    U1MODEbits.BRGH = 1;     // High-speed mode
    U1MODEbits.PDSEL = 0;    // 8-bit, no parity
    U1MODEbits.STSEL = 0;    // 1 stop bit

    // --- Baud rate: 115200 @ PBCLK2 = 60 MHz ---
    U1BRG = 129;

    // --- Pin configuration ---
    ANSELBbits.ANSB7 = 0;    // RB7 digital
    TRISBbits.TRISB7 = 0;    // TX output
    TRISCbits.TRISC15 = 1;   // RX input

    // --- PPS mapping (IOLOCK must be unlocked in sys_init) ---
    RPB7R = 1;               // U1TX -> RPB7
    U1RXR = 1;               // U1RX -> RPC15

    // --- Interrupt priority setup (OK to do now) ---
    IPC9bits.U1RXIP = 3;
    IPC9bits.U1RXIS = 2;
    IPC10bits.U1TXIP = 3;
    IPC10bits.U1TXIS = 3;

    // --- Clear interrupt flags BEFORE enabling UART ---
    IFS1CLR = _IFS1_U1RXIF_MASK;
    IFS1CLR = _IFS1_U1TXIF_MASK;

    // --- Disable UART interrupts during init ---
    IEC1CLR = _IEC1_U1RXIE_MASK;
    IEC1CLR = _IEC1_U1TXIE_MASK;

    // --- Enable UART module ---
    U1MODEbits.UARTEN = 1;

    // --- Enable TX and RX state machines ---
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;

    // --- Clear overrun if present ---
    if (U1STAbits.OERR) U1STAbits.OERR = 0;

    // DO NOT enable interrupts here.
    // Enable them later, AFTER sys_init() enables MVEC + global interrupts.
}



// ---------------- SPI Setup ----------------
void SPI_init(void)
{
    // Configure PPS pins
    // Set data direction for pins
    RPC0R = 0b0100; // assign RPC0 to SD02
    SCK2R = 0b0110; // assign RPC1 to SCK2
    TRISCbits.TRISC0 = 0; // SDO2 (pin 17) as output
    TRISCbits.TRISC1 = 0; // SCK2 (pin 18) as output
    
    //TRISCbits.TRISC2 = 1; // SDI2 (pin 19) as input - not really needed.

    // Clear overflow flag
    SPI2STATCLR = _SPI2STAT_SPIROV_MASK; 

    // Set SPI Clock = PBCLK / (2 * (BRG + 1))
    SPI2BRG = 9;     

    // SPI mode configuration
    SPI2CONbits.MSTEN = 1;  // Master mode
    SPI2CONbits.CKP = 0;    // SPI Mode 0
    SPI2CONbits.CKE = 1;    //
    SPI2CONbits.SMP = 0;    // Data is sampled at middle of output time  
    SPI2CONbits.MODE16 = 0; // 8-bit mode
    SPI2CONbits.MODE32 = 0; // 8-bit mode
    SPI2CONbits.ON = 1;     // Enable SPI     
}

// ---------------- I2C Setup ----------------
void I2C_init(void)
{   
    // Important note: I2C pins are open-drain and REQUIRE pull up resistors!!!
    // Set pin data directions
    TRISAbits.TRISA8 = 1; // set SDA1 as input
    TRISBbits.TRISB4 = 1; // set SCL1 as input
    
    // Peripheral pin select mapping
    SDI1R = 0b0000;  // SDI1 = RPA8
    RPB4R = 0b0100; // SCL1 = RPB4
    
    // Disable I2C module 1
    I2C1CONbits.ON = 0;
    
    // Configure I2C to be setup in master mode
    I2C1BRG = 157;          // 100kHz I2C. PBCLK = 60MHz
    I2C1CONbits.SIDL = 0;   // Continue in idle mode
    I2C1CONbits.A10M = 0;   // Use 7 bit addressing
    I2C1CONbits.DISSLW = 0; // Enable slew rate
    
    // Enable the I2C module 1
    I2C1CONbits.ON = 1;
}

// ---------------- LCD Setup ----------------
void LCD_init(void)
{
    // Set data direction for LCD control lines (all outputs)
    TRISBbits.TRISB5 = 0;   // Set pin 31 (Command select)
    TRISBbits.TRISB6 = 0;   // Set pin 32 (Reset line)
    TRISCbits.TRISC10 = 0;  // Set pin 33 (Data command)
    
    // Set the default states of pins
    LATBbits.LATB5 = 1;     // Command select idles high
    LATBbits.LATB6 = 1;     // Reset line idles high
    LATCbits.LATC10 = 0;    // Data command idles low
    
    // Toggle the reset in order to initialize the LCD
    LATBbits.LATB6 = 0; // Pull the reset line low
    for(volatile int i = 0; i < 10000; i++);
    LATBbits.LATB6 = 1; // Pull reset line back high
    for(volatile int i = 0; i < 10000; i++);
}

// ---------------- Azimuth Motor Setup ----------------
void azimuth_motor_init(void)
{
    // ---------------- Azimuth Motor Input 1 ----------------N 
    // Azimuth motor input 1: RPA0, pin 9
    // Note: "Motor Input" refers to the signal that drives the H-bridge. We set
    //      the pin as an output as we write to this channel to control motor.
    TRISAbits.TRISA0 = 0;   // Set RPA0 as an output
    LATAbits.LATA0 = 0;     // Set RPA0 to 0 as a safety precaution
    
    // ---------------- Azimuth Motor Input 2 ----------------
    // Azimuth motor input 2: RPA12, pin 7
    TRISAbits.TRISA12 = 0;  // Set RPA12 as an output
    LATAbits.LATA12 = 0;    // Sets RPA12 to 0.
    
    // ---------------- Limit Switch Setup ----------------
    // HIGH limit switch: RPB0, pin 11
    TRISBbits.TRISB0 = 1;   // Set RPB0 as an input
    // LOW limit switch: RPB1, pin 12
    TRISBbits.TRISB1 = 1;   // Set RPB1 as an input
    
    // ---------------- Manual Clockwise Rotation Setup ----------------
    // Manual clockwise switch: RPA7, pin 1
    TRISAbits.TRISA7 = 1;   // Set RPA7 as an input
    
    // ---------------- Manual Counter-Clockwise Rotation Setup ----------------
    // Manual counter-clockwise switch: RPB14, pin 2
    TRISBbits.TRISB14 = 1;  // Set RPB14 as an input
}

void elevation_motor_init(void)
{   
    // ---------------- Elevation Motor Input 1 ----------------
    // Elevation motor input 1: RPB8, pin 36
    // Note: "Motor Input" refers to the signal that drives the H-bridge. We set
    //      the pin as an output as we write to this channel to control motor.
    TRISBbits.TRISB8 = 0;   // Set RPB8 as an output
    LATBbits.LATB8 = 0;     // Set RPB8 to 0 as a safety precaution before setup
    
    // ---------------- Elevation Motor Input 2 ----------------
    // Elevation motor input 2: RPB9, pin 37
    TRISBbits.TRISB9 = 0;   // Set RPB9 as an output
    LATBbits.LATB9 = 0;     // Sets RPB9 to 0.
    
    //  ---------------- Limit Switch Setup ----------------
    // HIGH limit switch: RPC6, pin 38
    TRISCbits.TRISC6 = 1;   // Set RPC6 as an input
    // LOW limit switch: RPC9, pin 41
    TRISCbits.TRISC9 = 1;   // Set RPC9 as an input
    
    // ---------------- Manual Up Rotation Setup ----------------
    // Manual up switch: RPB12, pin 46
    TRISBbits.TRISB12 = 1; // Set RPB12as an input
    
    // ---------------- Manual Down Rotation Setup ----------------
    // Manual down switch: RPA10, pin 48
    TRISAbits.TRISA10 = 1; // Set RPA10as an input
}

// ---------------- Quadrature Encoder 1 Setup ----------------
void QEI1_init(void)
{
    // Disable QEI and clear control registers before setup
    QEI1CON = 0;
    QEI1IOC = 0;

    // Assign pins to quadrature encoder interface #1 module
    QEA1R = 0b1000;             // RPA11, pin 8
    QEB1R = 0b0000;             // RPA1, pin 10

    // QEI Control Bits - Leave here for clarity and ease of updating
    QEI1CONbits.QEISIDL = 0;    // Continue in Idle mode
    QEI1CONbits.PIMOD = 0b000;  // index does not affect position counter
    QEI1CONbits.IMV = 0b00;     // index match value. 0 = index pulse does nothing
    QEI1CONbits.INTDIV = 0b000; // no pre-scaler
    QEI1IOCbits.SWPAB = 0;      // do not swap A and B encoder channels
    QEI1IOCbits.QEAPOL = 0;     // Normal polarity
    QEI1CONbits.GATEN = 0;      // no gating
    QEI1CONbits.CCM = 0b00;     // Quadrature mode
    
    QEI1IOCbits.FLTREN = 1;     // Enable digital filter
    QEI1IOCbits.QFDIV = 0b111;  // 128 divide filter
    
    // Analog Disable Note: This is done to ensure we get accurate quadrature
    //      encoder readings. If analog is not disabled, the QEI will not work!
    ANSELAbits.ANSA1 = 0;       // disable analog on RA1
    ANSELAbits.ANSA11 = 0;      // disable analog on RA11
    
    // Configure the pins to be inputs
    TRISAbits.TRISA11 = 1;      // RPA11
    TRISAbits.TRISA1 = 1;       // RPA1
   
    // Clear position counter and enable the QEI
    POS1CNT = 0;                // Clear position count
    QEI1CONbits.QEIEN = 1;      // Enable the QEI module
}

// ---------------- Quadrature Encoder 2 Setup ----------------
void QEI2_init(void)
{
    // Disable QEI and clear control registers before setup
    QEI2CON = 0;
    QEI2IOC = 0;
    
    // RPX mapping
    QEA2R = 0b0011;             // RPB10, pin 44
    QEB2R = 0b0011;             // RPB13, pin 47
    
    // QEI Control Bits - Leave here for clarity and ease of updating
    QEI2CONbits.QEISIDL = 0;    // Continue in Idle mode
    QEI2CONbits.PIMOD = 0b000;  // index does not affect pos counter
    QEI2CONbits.IMV = 0b00;     // index match value. 0 = index pulse does nothing
    QEI2CONbits.INTDIV = 0b000; // no pre-scaler
    QEI2IOCbits.SWPAB = 0;      // do not swap A and B encoder channels
    QEI2IOCbits.QEAPOL = 0;     // Normal polarity
    QEI2CONbits.GATEN = 0;      // no gating
    QEI2CONbits.CCM = 0b00;     // Quadrature mode
    
    // Analog Disable Note: RB10 & RB13 are digital only, so there is no need
    //      to disable analog capability on these pins.
    
    // Configure the pins as inputs
    TRISBbits.TRISB10 = 1;      // RPB10
    TRISBbits.TRISB13 = 1;      // RPB13
    
    // Clear position counter and enable the QEI
    POS2CNT = 0;                // Clear position count
    QEI2CONbits.QEIEN = 1;      // Enable the QEI module
}

// ---------------- Timer 2 Setup  ----------------
void timer2_init(void)
{
    T2CON = 0; // stop timer and clear all configuration bits
    TMR2 = 0; // clears stored timer count
    
    T2CONbits.TCKPS = 0b111; // 1:256 clock prescaler. PBCLK2 runs at 60MHz, so now Timer2 will run at 234,375Hz.    
    // timer 2 runs @ 1/256 speed of Peripheral Bus Clock (60MHz)
    // we want timer 2 to run at 100Hz.
    // 1second/234,375Hz --> 4.267 microsecond tick period.
    // 10millisecond / 4.267 microsecond --> approx 2344 ticks required to get an interrupt every 10ms
    PR2 = 2343; // sets what our timer counts up to until an interrupt occurs
    
    IPC2bits.T2IP = 2; // sets interrupt priority level (0-7)
    IPC2bits.T2IS = 1; // sub-priority
    IFS0bits.T2IF = 0; // clear flag
    IEC0bits.T2IE = 1; // enable interrupt
    T2CONbits.ON = 1; // start the timer
}

// ---------------- Timer 3 Setup: 10kHz ----------------
void timer3_init(void)
{
    T3CON = 0;  // Stop timer and clear config
    TMR3 = 0;   // reset timer count
    
    T3CONbits.TCKPS = 0b000; // 1:1 prescaler value of PBCLK2 (60MHz)
    // PR3 calculation: (60MHz / 10kHz) - 1 = 5999
    PR3 = 5999;
    
    IFS0bits.T3IF = 0;  // clear interrupt flag
    IEC0bits.T3IE = 0;  // disable interrupts
    
    T3CONbits.ON = 1;   // start the timer module
}

// ---------------- OC1 Setup ----------------
void oc1_init(void)
{
    // Azimuth PWM: RPB15 (0b00101) pin 3
    // Note: This PWM signal controls the duty cycle of the azimuth DC motor.
    //      and it connects to the enable pin of the L293D H-bridge
    RPB15R = 0b00101;       // Set PPS output mapping for OC1
    LATBbits.LATB15 = 0;    // Set the PWM signal to 0 before setup
    TRISBbits.TRISB15 = 0;  // Set RPB15 as an output
    
    OC1CONbits.OCM = 0b110; // set OC1 to PWM mode with fault pin disabled
    OC1CONbits.OCTSEL = 1;  // set timer base source
    OC1CONbits.OC32 = 0;    // 16-bit timer mode
    
    OC1CONbits.ON = 1;      // enable OC1 module
}

// ---------------- OC2 Setup ----------------
void oc2_init(void)
{    
    // Elevation PWM: RPB11 pin 44
    // Note: This PWM signal controls the duty cycle of the elevation DC motor.
    //      and it connects to the enable pin of the L293D H-bridge
    RPB11R = 0b00101;       // Set PPS output mapping for OC2
    TRISBbits.TRISB11 = 0;  // Set RPB11 as an output
    LATBbits.LATB11 = 0;    // Set the PWM signal to 0 before setup
    
    OC2CONbits.OCM = 0b110; // set OC2 to PWM mode with fault pin disabled
    OC2CONbits.OCTSEL = 1;  // set timer base source
    OC2CONbits.OC32 = 0;    // 16-bit timer mode
    
    OC2CONbits.ON = 1;      // enable OC2 module
}