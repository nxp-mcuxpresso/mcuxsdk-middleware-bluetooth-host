# Transient Application Key

Use the Bluetooth LE Shell application to exercise the TAK(Transient Application Key) feature.
First, 'gAppUseTAK_c' must be enabled and 'gConnTakMaxEntries_c' must be configured to the maximum number of keys we want to support on a device. Also 'gBleHostAutoRejectLtkRequestForUnbondedDevices_c' must be set to FALSE in case the device is peripheral.
The key is stored per device ID. The device ID is obtained after connection.
Each key is erased after it is used, as per TAK specification requirements.

**On the GAP Peripheral device**:

1.  Configure the extended advertising data to include TAK identifier and set the TAK key for the desired device.

    The advertising data type is set to shortened local name \(8\) and the advertising data content is set to `TAK_ID`.

2.  Start extended advertising.

    ```
    BLE Shell>gap extadvdata 8 TAK_ID
    BLE Shell>gap extadvstart
    --> GAP Event: Extended
    Advertising parameters successfully set.
    --> GAP Event:
    Extended Advertising data successfully set.
    --> GAP Event: Advertising state changed successfully!
    ...
    --> GAP Event: Connected to peer 0
    BLE Shell>gap tak 0 00112233445566778899aabbccddeeff
    ```

3.  **On the GAP Central device**

    Set the scanning parameters.

4.  Start scanning.

    ```
    BLE Shell>gap scanstart
    BLE Shell>
    -> GAP Event: Scan started.
    BLE Shell>
    --> GAP Event: Found device 0 : 0060375BCEC6 -23 dBm
    Advertising Extended Data:
    TAK_ID
    ...
    BLE Shell>gap tak 0 00112233445566778899aabbccddeeff
    ```

5.  Connect to the desired device in the scanned devices list.

    ```
    BLE Shell>gap connect 0
    BLE Shell>
    -> GAP Event: Scan stopped.
    BLE Shell>
    --> GAP Event: Connected to peer 0
    ```


**Parent topic:**[Usage](../topics/usage_513.md)

