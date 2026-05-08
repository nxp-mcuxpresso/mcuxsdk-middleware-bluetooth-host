# Running a Bluetooth Low Energy Localization Scenario Using the Shell Interface

The following Bluetooth Low Energy localization examples are provisioned by default with a shell command-line interface accessible via a Terminal application such as Tera Term:

-   Localization Reader
-   Localization User Device

After connecting with the Terminal application, pressing the **RST** switch on the board displays the application name.

To display the list of available commands and their description, enter the '`help`' command as shown in [Figure 1](../images/Figure6.png).

![](../images/Figure6.png "Localization Reader application available commands")

To run the application, perform the following steps:

-   Enter the `sb` command on both terminals that are connected to the Localization Reader and Localization User Device applications. The devices connect, perform pairing if not previously bonded, and trigger distance measurement.

![](../images/Figure7.png "Starting the Localization Reader application ")

[Figure 2](../images/Figure7.png) illustrates the start of the Localization Reader application.

![](../images/Figure8.png "Starting the Localization User Device application ")



After the devices are connected, the link is encrypted, the Channel Sounding configuration is created, and distance measurement is triggered. This is shown in [Figure 3](../images/Figure8.png). If two consecutive distance measurements return a value lower or equal to 1 meter, the LED1 on the Localization Reader starts blinking. If the measured distance is higher than 1, LED1 turns off.

-  To trigger a new distance measurement, use the "`tdm`" command:
   -  **`tdm`** (without parameters): Triggers distance measurement on **all connected devices**
   -  **`tdm <peerId>`**: Triggers distance measurement on the **specified peer device**
   
   Examples:
   -  `tdm` - Start distance measurement on all connected peers
   -  `tdm 0` - Start distance measurement on peer device with ID 0
   -  `tdm 1` - Start distance measurement on peer device with ID 1

   This command can be executed on either the Localization User Device or the Localization Reader. [Figure 4](../images/Figure5_Localization_User_Device_TriggerDistMeasurement.png) shows an example of triggering the distance measurement from the Localization User Device.


![](../images/Figure5_Localization_User_Device_TriggerDistMeasurement.png "Localization User Device triggering distance measurement")

![](../images/Fig6_Localization_Reader_trigger_distance_measurement.png "Localization Reader trigger distance measurement data")

[Figure 5](../images/Fig6_Localization_Reader_trigger_distance_measurement.png) shows the available distance measurement data.

Other available shell commands are:

-   `setcsconfig`: This command overwrites the default parameters for the *Channel Sounding Create Config* command. It takes the following parameters, in the order below:
    -   `peerId`
    -   `mainModeType`: CS mode to be used as main mode during the CS procedure for this configuration. Possible values:
        -   1 (CS mode-1)
        -   2 (CS mode-2)
        -   3 (CS mode-3)
    -   `subModeType`: CS mode to be used as submode during the CS procedure for this configuration. Possible values:
        -   1 (CS mode-1)
        -   2 (CS mode-2)
        -   3 (CS mode-3)
        -   255 (no submode used)
    -   `mainModeMinSteps`: Lower bound of the range of main mode CS steps to be executed before a submode CS step is executed during the CS procedure. Range:
        -   1 to 255
    -   `mainModeMaxSteps`: Higher bound  of the range of main mode CS steps to be executed before a submode CS step is executed during the CS procedure. Range:
        -   1 to 255
    -   `mainModeRepetition`: Number of main mode CS steps repeated from the previous CS subevent at the beginning of the current CS subevent. Range:
        -   0 to 3
    -   `mode0Steps`: Number of CS mode-0 steps to be included at the beginning of each CS subevent. Range:\
        -   1 to 3
    -   `role`: CS role. Possible values:
        -   0 (initiator)
        -   1 (reflector)
    -   `RTTType`: RTT variant to be used during the CS procedure. Possible values:
        -   0 (RTT AA only)
        -   1 (RTT with 32-bit sounding sequence)
        -   2 (RTT with 96-bit sounding sequence)
        -   3 (RTT with 32-bit random sequence)
        -   4 (RTT with 64-bit random sequence)
        -   5 (RTT with 96-bit random sequence)
        -   6 (RTT with 128-bit random sequence)
    -   `channelMap`: 80-bit map in big endian order indicating channels to be used for the CS procedure. The default value is `fcff7ffcffffffffff1f` (all channels used except 0, 1, 23-25 and 77-78). Bit 79 is reserved for future use. At least 15 channels must be enabled.
    -   `channelMapRepetition`: Number of times the channel map will be cyled through for non-mode-0 steps within a CS procedure. Range:
        -   1 to 255.
    -   `channelSelectionType`: Possible values:
        -   0 (Channel Selection Algorithm #3b)
        -   1 (Channel Selection Algorithm #3c)
    -   `cs_sync_phy`: PHY to be used for CS_SYNC exchanges during the CS procedure for the specified CS configuration. Possible values:
        -   1 (LE 1M PHY)
        -   2 (LE 2M PHY)
        -   3 (LE 2M 2BT PHY) - if supported at Link Layer level
-   `setcsproc`: This command overwrites the default parameters for the *Channel Sounding Set Procedure Parameters* command. It takes the following parameters, in the order below:
    -   `peerId`
    -   `maxProcedureDuration`: Maximum duration for each CS procedure. Range:
        -   1 to 65535 (units of 0.625ms)
    -   `minPeriodBetweenProcedures`: Minimum number of connection events between consecutive CS procedures. Range:
        -   1 to 65535
    -   `maxPeriodBetweenProcedures`: Maximum number of connection events between consecutive CS procedures. Range:
        -   1 to 65535
    -   `maxNumProcedures`: Maximum number of CS procedures to be scheduled. Possible values:
        -   0 (CS procedures to continue until disabled)
        -   1 to 65535
    -   `minSubeventLen`: Minimum suggested duration for each CS subevent in microseconds. Range:
        -   1250 microseconds to 3.999999 seconds
    -   `maxSubeventLen`: Maximum suggested duration for each CS subevent in microseconds. Range:
        -   1250 microseconds to 3.999999 seconds
    -   `antCfgIndex`: Antenna Configuration Index as described in the Core specification. Range:
        -   0 to 7
    -   `snr_control_init`: SNR Control to be applied to the Initiator in the CS procedure (if supported at Link Layer level). Possible values:
        -   0 (SNR control adjustment of 18 dB)
        -   1 (SNR control adjustment of 21 dB)
        -   2 (SNR control adjustment of 24 dB)
        -   3 (SNR control adjustment of 27 dB)
        -   4 (SNR control adjustment of 30 dB)
        -   255 (SNR control is not to be applied)
    -  `snr_control_refl`: SNR Control to be applied to the Reflector in the CS procedure (if supported at Link Layer level). Possible values:
        -   0 (SNR control adjustment of 18 dB)
        -   1 (SNR control adjustment of 21 dB)
        -   2 (SNR control adjustment of 24 dB)
        -   3 (SNR control adjustment of 27 dB)
        -   4 (SNR control adjustment of 30 dB)
        -   255 (SNR control is not to be applied)
-   `verbosity`: Sets the verbosity level during the CS procedure.
-   `setnumprocs`: This command overwrites the default value `gCsProcRepeatMaxNumProcedures_c` parameter for the CS Procedure Repeat. It takes the following parameters, in the order below:
    -   `peerId`
    -   `maxNumProcedures`: Number of procedures in hex format. Example: `setnumprocs 0 0x0005`.
-   `setmcp`: This command overwrites the default value `gChannelSoundingMaxConcurrentProcedures_c` parameter for the number of maximum supported simultaneous CS procedures across all connections. It takes the following parameters:
    -   `maxNumConcurrentProcedures`: Number of procedures in decimal format. Example: `setmcp 2`.
-  `loop`: This command toggles the automatic restart of the CS procedure (whether with CS Procedure Repeat enabled or not) upon its end. By default, the CS procedure will not restart automatically.

**Parent topic:**[Running the Bluetooth Low Energy Localization Scenario](../topics/running_bluetooth_low_energy_localization_scenario.md)

