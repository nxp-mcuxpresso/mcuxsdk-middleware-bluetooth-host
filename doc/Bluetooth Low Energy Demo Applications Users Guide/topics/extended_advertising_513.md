# Extended advertising

Use the Bluetooth LE Shell application to exercise the advertising extension features:

**On the GAP Peripheral device**:

1.  Configure the extended advertising parameters. In the below example, the advertising type is set to connectable and includes TX power and the primary PHY is set to Coded PHY.
2.  Configure the extended advertising data. The Bluetooth LE Shell applications has the feature to send for test, a large data payload. Use the extended advertisement default configuration \(not call "`gap extadvcfg`"\), pass the command "`gap extadvdata`" with no parameters and the default data is added. The length is configurable at compile time through `SHELL_EXT_ADV_DATA_SIZE` and the data pattern is `SHELL_EXT_ADV_DATA_PATTERN`. Start the default test with call for "`gap extadvstart`".

    The advertising data type is set to shortened local name \(8\) and the advertising data content is set to `test_ext_adv_data`.

    **Note:** Users must note that extended connectable advertising does not allow for chained advertising data. The data length must be limited to what can fit in a single AUX\_ADV\_IND PDU \(251 bytes at maximum\). This means that passing the `gap extadvdata` with no parameters and the default value of `SHELL_EXT_ADV_DATA_SIZE` \(500 bytes\) after having set the advertising type to connectable will result in an error when trying to start advertising.

3.  Start extended advertising.

    ```
    BLE Shell>gap extadvcfg -type 65 -phy1 3
    BLE Shell>gap extadvdata 8 test_ext_adv_data
    BLE Shell>gap extadvstart
    --> GAP Event: Extended
    Advertising parameters successfully set.
    --> GAP Event:
    Extended Advertising data successfully set.
    --> GAP Event: Advertising state changed successfully!
    ```

4.  **On the GAP Central device**

    Set the scanning parameters. The scanning PHY is set to match the advertising PHY, in this case Coded PHY.

5.  Start scanning and filter duplicates.

    ```
    BLE Shell>gap scancfg -phy 4
    BLE Shell>gap scanstart filter
    BLE Shell>
    -> GAP Event: Scan started.
    BLE Shell>
    --> GAP Event: Found device 0 : 0060375BCEC6 -23 dBm
    Advertising Extended Data:
    test_ext_adv_data
    ```

6.  Set the connection initiating PHYs corresponding to the primary PHY on which the advertising is performed.
7.  Connect to the desired device in the scanned devices list.

    ```
    BLE Shell>gap connectcfg -phy 4
    --> Connection Parameters:
    --> Connection Interval: 200 ms
    --> Connection Latency: 0
    --> Supervision Timeout: 32000 ms
    --> Connecting PHYs: Coded
    BLE Shell>gap connect 0
    BLE Shell>
    -> GAP Event: Scan stopped.
    BLE Shell>
    --> GAP Event: Connected to peer 0
    ```


**Parent topic:**[Usage](../topics/usage_513.md)

