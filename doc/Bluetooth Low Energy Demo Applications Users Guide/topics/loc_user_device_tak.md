# Transient Application Key

Use the Bluetooth LE Localization User Device application to exercise the TAK(Transient Application Key) feature.
First, 'gAppUseTAK_c' must be enabled and 'gConnTakMaxEntries_c' must be configured to the maximum number of keys we want to support on a device. Also 'gBleHostAutoRejectLtkRequestForUnbondedDevices_c' must be set to FALSE. 'gAppTAKAdvID_c' must be set to be the same string as in loc_reader.
The key is stored per device ID.
Each key is erased after it is used, as per TAK specification requirements.

A Transient Application Key must be set before starting the application, using the command:

tak 0 00112233445566778899aabbccddeeff

**Parent topic:**[Usage](../topics/loc_reader.md)
