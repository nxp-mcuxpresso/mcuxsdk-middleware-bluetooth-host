# Demo Functionality Overview

The two demo applications provided showcase the following Bluetooth Low Energy localization features:

-   Distance measurement through Channel Sounding
-   Ranging Service \(RAS\) and Ranging Profile \(RAP\) implementation
-   Distance measurement data transfer through RAS
-   Complex-domain Distance Estimation \(CDE\) ranging algorithm
-   Rapid Accurate Distance Estimator \(RADE\) ranging algorithm

**Application configuration options:**

-   **Channel Sounding role:**Set the Channel Sounding role by using the `gCsDefaultRole_c` define in `app_preinclude.h`. In the default configuration, the **loc\_reader** application is in the reflector role \(`gCsRoleReflector_c`\) and the **loc\_user\_device** application is in the initiator role \(`gCsRoleInitiator_c`\).
-   **GATT CCCD type for RAS characteristics:** Configure whether RAS characteristics use INDICATION or NOTIFICATION for data transfer by using the `gRasUseNotifOrInd_c` define in `app_preinclude.h`. Set to `gCccdIndication_c` for reliable delivery with acknowledgment (slower, guaranteed) or `gCccdNotification_c` for best-effort delivery without acknowledgment (faster, no guarantee). This affects RAS Control Point, Data Ready, Data Overwritten, and On-Demand Data characteristics. Default: `gCccdNotification_c`.
-   **Channel Sounding procedure repeat**: Use the Channel Sounding procedure repeat to specify the number of channel sounding procedures to be performed for each distance measurement triggered. This macro can be set through the `gCsProcRepeatMaxNumProcedures_c` define.
-   **Ranging algorithm**: The ranging algorithm must be run once the local and remote measurement data is available. Both supported algorithms can be disabled at compile time for code size reasons. By default, both are enabled. To disable an algorithm at compile time, define its respective macro with value `0` in the file `app_preinclude.h`. Use the macros `gAppUseCDEAlgorithm_d` and `gAppUseRADEAlgorithm_d`. At runtime, the `setalgo` shell command can be used to configure the algorithm that runs on completion of the CS procedure and RAS transfer. Any combination of algorithms can be selected.
-   **Timing information display**: To enable this feature, set the `gAppCsTimeInfo_d` macro to the value `1` in the `app_preinclude.h` file. The console then displays details of the timing information \(such as the duration of the CS configuration phase, CS procedure, RAS transfer, and running of the algorithm\(s\)\).
-   **Data quality information display**: This feature can be enabled by setting the `gAppParseQualityInfo_d` macro to the value `1` in the file `app_preinclude.h` for the Anchor. Additional information \(such as RSSI and Tone Quality Indicator values for the measurement data\) is displayed in the console.
-   **GAP role**: Set the GAP role using `gAppIsPeripheral_d`, 1 for peripheral role, 0 for central role. In the default configuration, the `loc_reader` application is central.
-   **Maximum concurrent Channel Sounding procedures**: Configure the maximum number of Channel Sounding procedures that can run simultaneously across all connections using the `gChannelSoundingMaxConcurrentProcedures_c` define in `app_preinclude.h`.
