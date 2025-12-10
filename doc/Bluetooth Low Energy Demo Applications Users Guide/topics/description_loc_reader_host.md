# Description

The Loc Reader Host application is a Bluetooth Low Energy (BLE) localization application that runs on the application core of dual-core NXP platforms. It works in conjunction with the ncp_loc_reader application running on the radio core to perform distance measurement and localization using Channel Sounding (CS) technology.

## Dual-Core Architecture
The application is part of a dual-core system and it:
- runs on the application core (Core 0) while the radio/BLE stack runs on Core 1
- communicates with the radio core via FSCI messages
- receives IQ (In-phase/Quadrature) measurement data from Core 1 for processing and measurement computation

## Data Reception and Processing
The application receives Channel Sounding measurement data via FSCI messages from the NCP Localization Reader running on the radio core. This data is processed to:
- compute distance measurements based on Channel Sounding algorithms
- display measurement results via UART shell interface

**Parent topic:**[Localization Reader Host](../topics/loc_reader_host.md)

