# Application configuration

This section describes configuration options available for the applications. The configuration can be done either at compile time, by using macros added in the `app_preinclude.h` file of the respective sample, or at runtime, via shell commands. The available configurations are:

-   The Channel Sounding role can be set through the `role` shell command at runtime only if there are no active connections. The default role is set through the `gCsDefaultRole_c` define in `app_preinclude.h`. In the default configuration, the Anchor is in the CS Initiator role (`gCsRoleInitiator_c`) and the Device is in the CS Reflector role (`gCsRoleReflector_c`).
-   The number of Channel Sounding procedures to be run via the CS Procedure Repeat mechanism, for each triggered distance measurement. The default value can be set through the `gCsProcRepeatMaxNumProcedures_c` define. At runtime, the value can be changed using the `setnumprocs` command.
-   The ranging algorithm to run once the local and remote measurement data is available. Each of the two supported algorithms can be disabled at compile time for code size reasons. By default, both are enabled. To disable an algorithm at compile time, define its respective macro with the value `0` inside `app_preinclude.h`. The macros are `gAppUseCDEAlgorithm_d`, `gAppUseRADEAlgorithm_d`. At runtime, the `setalgo` shell command can be used to configure the algorithm that runs upon completion of the CS procedure and data transfer. Any combination of algorithms can be selected. By default, RADE is selected.
-   The timing information display feature can be activated by setting the `gAppCsTimeInfo_d` macro to value `1` inside `app_preinclude.h`. Additional timing information such as the duration of the CS config phase, CS procedure, BTCS transfer, and running of the algorithms is displayed on the console.
-   The BTCS L2CAP transfer watchdog is an **NXP proprietary extension** (the CCC BTCS specification does not define any timeout for the L2CAP measurement transfer). When the CS procedure auto-restart loop is enabled on the BTCS client, this watchdog ensures the loop does not stall if a BTCS L2CAP transfer is lost or never completes. The feature is client-side only and is always enabled. The watchdog timeout duration in seconds can be tuned through the `gBtcsTimeoutSeconds_c` macro (default `3`). If a transfer does not complete before the timeout expires, the watchdog only restarts the CS procedure loop when the full procedure sequence (`maxNumProcedures`) has been reached; otherwise it simply returns to waiting for the next procedure's measurement data.


For better performance and user experience, the optional LE Coded PHY advertising should be disabled by setting `gAppLeCodedAdvEnable_d` to `0` inside `app_preinclude.h` on the Anchor.

-   Maximum concurrent Channel Sounding procedures**: Configure the maximum number of Channel Sounding procedures that can run simultaneously across all connections using the `gChannelSoundingMaxConcurrentProcedures_c` define in `app_preinclude.h`.

**Parent topic:**[Localization scenarios](../topics/localization_scenarios.md)

