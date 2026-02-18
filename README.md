# Beam Boss: Automated Antenna Rotator to Track Satellites

Our senior capstone involes designing and implmenting a custom, automated antenna rotating system which our sponsor/client will be using to communicate with satellites.

## Description

This GitHub contains the firmware created in MPLAB x IDE. Everything is coded in the C programming language, and it targets a PIC32MK0512MCJ048 microcontroller.

The system will control two DC motors which are equipped with quadrature encoders in order to adjust an antenna's azimuth and elevation. The motors are controlled with proportional-integral controllers (PI) and also take into consideration limit switches and an emergency stop button. The PI controller will modulate two seperate pulse width modulation signals (PWM) in order to control the motors speed and direction.

Users will be able to hook-up their laptop to our embedded system via a USB cable and connect the Ham Radio Deluxe software. Ham Radio Deluxe will allow a user to select a satellite of choice and then command our system using the GS-232A rotor command protocol. These commands are sent using UART.

An LCD controlled using SPI will be able to provide live-feedback to the user about the status of the system, current date and time, as well as the current elevation and azimuth readings.

## Authors

Camden Downey (Embedded System)
Paul Riehle (Electrical)
Noah Tugen (Mechanical)
Phil Pash (Sponsor/Client)
