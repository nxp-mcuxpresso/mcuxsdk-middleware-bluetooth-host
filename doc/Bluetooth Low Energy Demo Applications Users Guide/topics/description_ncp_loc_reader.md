# Description

The BLE NCP Localization Reader application is a demonstration program for the radio core (Core 1) based on the MCUXpresso SDK. This application implements a BLE device that performs distance measurement using Channel Sounding (CS) technology.

The application is designed to work together with the corresponding loc_reader_host application running on the application core (Core 0) in a dual-core architecture.

## Core 1 Responsibilities

The device handles all BLE-related operations:
- Connection establishment and management
- Channel Sounding procedure execution
- Service discovery (RAS/BTCS)
- Local and remote CS data collection
- Data fragmentation and transmission to Core 0 via FSCI

## Data Transfer to Core 0

The application fragments Channel Sounding measurement data into FSCI-sized messages and transmits them to Core 0 using the following event sequence:
- **gIQLocalTrStart_c**: First fragment with measurement header
- **gIQLocalTrCont_c**: Continuation fragments
- **gIQLocalTrEnd_c**: Final fragment

The same sequence is repeated for remote data (gIQRemoteTrStart_c/Cont_c/End_c).

Each message includes device ID to support multi-peer scenarios.

**Parent topic:** [NCP Localization Reader](../topics/ncp_loc_reader.md)