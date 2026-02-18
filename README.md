# Beam Boss: Automated Antenna Rotator to Track Satellites

Our senior capstone involes designing and implmenting a custom, automated antenna rotating system which our sponsor/client will be using to communicate with satellites.

## Description

This GitHub contains the firmware created in MPLAB x IDE. Everything is coded in the C programming language, and it targets a PIC32MK0512MCJ048 microcontroller. The system will control two DC motors which are equipped with quadrature encoders in order to adjust an antenna's azimuth and elevation. The motors are controlled with proportional-integral controllers (PI) and also take into consideration limit switches and an emergency stop button. 

## Authors

Camden Downey (Embedded System)
Paul Riehle (Electrical)
Noah Tugen (Mechanical)
Phil Pash (Sponsor/Client)
