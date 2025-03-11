# Repeated attempts

Applications can be configured to enable protection against repeated Pairing Requests/Peripheral Security Requests coming from the same device. This is to prevent an intruder from repeating the pairing process with a large number of different keys in order to extract information about the local device’s private key. If this feature is enabled, after a pairing procedure fails, another attempt to pair coming from the same device is allowed only after a specific time period has passed. For each failure, the waiting period doubles up until a maximum period.

The following *app\_preinclude.h* macros support this feature:

-   `gRepeatedAttempts_d`
    -   Set to 1 to enable the feature. By default, it is disabled \(0\).
-   `gRepeatedAttemptsNoOfDevices_c`
    -   Number of remote devices to keep track of. By default, the value is 4.
    -   If a new device needs to be added and the list is full, one of the oldest entries will be replaced.
-   `gRepeatedAttemptsTimeoutMin_c`
    -   Minimum waiting period in seconds – default 10.
-   `gRepeatedAttemptsTimeoutMax_c`
    -   Maximum waiting period in seconds – default 640. The waiting period doubles after each failed pairing with the same device.

**Parent topic:**[Application Structure](../topics/application_structure.md)

