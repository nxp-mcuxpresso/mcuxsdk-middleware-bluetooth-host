# Time client devices

Some applications implement the Current Time Service. To enable this feature, define the `gAppUseTimeService_d` parameter in the `app_preinclude.h` file as `1`. If the Time Client is enabled, the device must synchronize with a Time Server to update its internal date/time to the current date/time. If you connect the device to the phone, the Time Client synchronizes with the phone \(pairing and bonding must be active\).

**Parent topic:**[Building and running a Bluetooth LE example application](../topics/building_and_running_a_bluetooth_le_example_applic.md)

