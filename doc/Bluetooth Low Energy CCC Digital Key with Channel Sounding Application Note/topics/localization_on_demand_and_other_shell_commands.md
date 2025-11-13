# Localization on demand and other shell commands

Localization can also be performed on demand via the `tdm` shell command, which takes the device ID of the peer as an argument. The command must be executed on the device that is configured as CS Initiator. As shown in the figure below, the Device triggers the localization process, and the default 5 measurements complete successfully.

**Triggering the localization process**

![](../images/fig23_tdm.png "Triggering the localization process")

Other available shell commands are:

-   `setcsconfig`: This command overwrites the default parameters for the *Channel Sounding Create Config* command. It takes the following parameters, in the order below:
    -   `peerId`
    -   `mainModeType`
    -   `subModeType`
    -   `mainModeMinSteps`
    -   `mainModeMaxSteps`
    -   `mainModeRepetition`
    -   `mode0Steps`
    -   `role`
    -   `RTTType`
    -   `channelMap`
    -   `channelMapRepetition`
    -   `channelSelectionType`
-   `setcsproc`: This command overwrites the default parameters for the *Channel Sounding Set Procedure Parameters* command. It takes the following parameters, in the order below:
    -   `peerId`
    -   `maxProcedureDuration`
    -   `minPeriodBetweenProcedures`
    -   `maxPeriodBetweenProcedures`
    -   `maxNumProcedures`
    -   `minSubeventLen`
    -   `maxSubeventLen`
    -   `antCfgIndex`
-   `verbosity`: Sets the verbosity level during the CS procedure.
-   `setnumprocs`: This command overwrites the default value `gCsProcRepeatMaxNumProcedures_c` parameter for the CS Procedure Repeat. It takes the following parameters, in the order below:

    -   `peerId`
    -   `maxNumProcedures`

**Parent topic:**[Localization scenarios](../topics/localization_scenarios.md)

