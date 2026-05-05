# Beam Boss: Automated Antenna Rotator to Track Satellites

Our senior capstone involes designing and implmenting a custom, automated antenna rotating system which our sponsor/client will be using to communicate with satellites.

## Key Features
- Closed-loop dual-axis control
- Quadrature encoder position tracking
- Threshold-based proportional control
- Deadband stabilization
- GS-232A protocol support (UART)
- Manual override controls
- Limit switch safety system
- Homing and calibration routine
- Real-time system feedback (SPI LCD)

## Description

This GitHub contains the firmware created in MPLAB x IDE. Everything is coded in the C programming language, and it targets a PIC32MK0512MCJ048 microcontroller.

The system will control two DC motors which are equipped with quadrature encoders in order to adjust an antenna's azimuth and elevation. The motors are controlled with a closed-loop, proportional-style, threshold-based control that has deadband logic and veloctiy scaling.

Users will be able to hook-up their laptop to our embedded system via a USB cable and connect the Ham Radio Deluxe software. Ham Radio Deluxe will allow a user to select a satellite of choice and then command our system using the GS-232A rotor command protocol. These commands are sent using UART.

An LCD controlled using SPI will be able to provide live-feedback to the user about the status of the system, current date and time, as well as the current elevation and azimuth readings.

## System Overview
<img width="1023" height="808" alt="image" src="https://github.com/user-attachments/assets/e7882166-8aac-44ec-80bf-e37e0a8d8c64" />


## Authors

Camden Downey (Embedded System)

Paul Riehle (Electrical)

Noah Tugen (Mechanical)

Phil Pash (Sponsor/Client)
