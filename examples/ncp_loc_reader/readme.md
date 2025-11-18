# Loc Reader NCP

## Overview
The Loc Reader Host application together with Loc Reader NCP application emulate the loc_reader SDK example.
The application runs on the radio core and is meant to be used together with the loc_reader_host
application running on the application core.

The Loc Reader NCP handles the Ble Connection with the loc_user_device.
By default, it is a peripheral and Ranging Service Client.
The application runs the Channel Sounding procedure and collects the localization data send by the device via the Ranging Service.
It then sends the localization data to the Loc Reader Host to run the localization algorithm.

For more details on how the loc_reader SDK example works, please check AN13974 Bluetooth Low Energy Localization Application Note.

## Supported Boards
The Loc Reader NCP application is supported on the following platform:
- MCX-W72-EVK
- FRDM-MCXW72
