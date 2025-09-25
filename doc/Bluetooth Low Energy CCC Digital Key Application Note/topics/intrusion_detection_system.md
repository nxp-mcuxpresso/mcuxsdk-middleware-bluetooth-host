# Intrusion Detection System

The Intrusion Detection System (IDS) is a proprietary feature which allows the Car Anchor to monitor anomalous events related to the traffic and conditions generated during the operation of the Bluetooth LE software stack. These events are detected at Host and Controller level. The application registers a callback where it receives and handles reports.

The IDS feature is supported on the following platforms:

-   KW45B41Z-EVK
-   KW47-EVK

To enable IDS, modify the Digital Key Car Anchor as per the steps listed below:

-   In `app_preinclude.h` file, set `gIntrusionDetectionSystem_d` to `TRUE`.

