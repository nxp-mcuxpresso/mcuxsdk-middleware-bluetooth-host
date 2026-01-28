# Description

The Loc Reader Host application is a Bluetooth Low Energy (BLE) localization application that runs on the application core of dual-core NXP platforms. It works in conjunction with the ncp_loc_reader application running on the radio core to perform distance measurement and localization using Channel Sounding (CS) technology.

## Dual-Core Architecture

The application is part of a dual-core system where:
- **Core 0 (Application Core)**: Runs loc_reader_host
  - Receives uncompress IQ measurement data from Core 1 via FSCI messages
  - Executes localization algorithms (RADE, CDE)
  - Displays results via UART shell
  
- **Core 1 (Radio Core)**: Runs ncp_loc_reader
  - Handles BLE stack and Channel Sounding procedures
  - Collects and transmits uncompress local/remote IQ data to Core 0

### Inter-Core Communication

Cores communicate using FSCI messages with event types:
- **gIQLocalTrStart_c/Cont_c/End_c**: Local uncompress IQ data fragments
- **gIQRemoteTrStart_c/Cont_c/End_c**: Remote uncompress IQ data fragments

Each message includes device ID, event type, and payload data (up to gFsciMaxPayloadLen_c).

### Data Flow

1. Core 1 performs CS procedure and collects uncompress IQ samples
2. Core 1 fragments and transmits data to Core 0 via FSCI
3. Core 0 reassembles fragments into csAppData_t buffers
4. Core 0 runs localization algorithms and displays results

**Parent topic:**[Localization Reader Host](../topics/loc_reader_host.md)

