# Loc Reader Host

## Overview
The Loc Reader Host application together with Loc Reader NCP application emulate the loc_reader SDK example.
The application run on the application core and is meant to be used together with the ncp_loc_reader application running on the radio core.

The Loc Reader Host handles the user interaction: receives shell commands and passes them to the Core 1 application,
prints events to inform the user of the algorithm run results and application flow.
The loc_reader_host application takes the Channel Sounding data from ncp_loc_reader and runs the localization algorithm based on it.
It also implements shell functionality for user interaction.

For more details on how the loc_reader SDK example works, please check AN13974 Bluetooth Low Energy Localization Application Note.

## Supported Boards
The Loc Reader Host application is supported on the following platform:
- MCX-W72-EVK
- FRDM-MCXW72
