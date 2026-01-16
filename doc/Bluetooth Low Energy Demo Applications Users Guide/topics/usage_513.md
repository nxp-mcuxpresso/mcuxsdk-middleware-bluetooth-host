# Usage

The application is built to work with any other Bluetooth LE device. To showcase the functionality, two platforms are used in the following setup.

1.  Open a serial port terminal and connect them to the two boards, in the same manner described in [Testing devices](testing_devices.md). The start screen is displayed after the board is reset. All LEDs are flashing on both devices.
2.  Configure one of the devices as a GAP peripheral and a Heart Rate server. Change name to HRS. Start advertising on this device.

    ```
    BLE Shell>gap devicename HRS
    --> GATTDB Event: Attribute Written
    HRS>gap advdata 1 6
    HRS>gap advdata 8 HRS
    HRS>gap advstart
    HRS>
    --> GAP Event: Advertising parameters successfully set.
    HRS>
    --> GAP Event: Advertising data successfully set.
    HRS>
    --> GAP Event: Advertising state changed successfully!
    HRS>gattdb addservice 0x180D
    --> Heart Rate
    - Heart Rate Measurement Value Handle: 14
    ```

3.  Configure the other device as a GAP central. Change its name to 'Collector'. Start scanning and connect to the HRS device by selecting the corresponding device index from the list of scanned devices. In the example below, the HRS device is device number 2. The number of listed scanned devices can be controller through the `mShellGapMaxScannedDevicesCount_c` define in `shell_gap.c`.

    ```
    BLE Shell>gap devicename Collector
    --> GATTDB Event: Attribute Written
    Collector>gap scanstart filter
    --> GAP Event: Scan started.
    Collector>
    --> GAP Event: Found device 0 : 880F102F500E 0 dBm
    --> GAP Event: Found device 1 : NXP_CSCS 00049F000006 0 dBm
    --> GAP Event: Found device 2 : HRS 00049F0000FF 0 dBm
    Collector>gap connect 2
    --> GAP Event: Scan stopped.
    Collector>
    --> GAP Event: Connected to peer 0
    ```

4.  Optionally, the devices can be paired \(gAppUsePairing\_d and gAppUseBonding\_d must be set in app\_preinclude.h\). On the collector initiate the pairing.

    ```
    Collector>gap pair 0
    --> Pairing...
    --> GAP Event: Link Encrypted
    --> GAP Event: Device Paired
    ```

5.  On the Collector, start service discovery. The device discovers the GAP, GATT, and Heart Rate services.

    ```
    Collector>gatt discover 0 -all
    --> Discovered primary services: 3
    --> Generic Attribute Start Handle: 1 End Handle: 4
    - Service Changed Value Handle: 3
    - Client Characteristic Configuration Descriptor Handle: 4
    --> Generic Access Start Handle: 5 End Handle: 11
    - Device Name Value Handle: 7
    - Appearance Value Handle: 9
    - Peripheral Preferred Connection Parameters Value Handle: 11
    --> Heart Rate Start Handle: 12 End Handle: 19
    - Heart Rate Measurement Value Handle: 14
    - Client Characteristic Configuration Descriptor Handle: 15
    - Body Sensor Location Value Handle: 17
    - Heart Rate Control Point Value Handle: 19
    ```

6.  Configure the HRS to send notifications by writing the CCCD from the Collector. Send a GATT write command with value `1` to the CCCD handle discovered, 15.

    ```
    Collector>gatt write 0 15 0x0001
    --> GATT Event: Characteristic Value Written!
    ```

7.  Send heart rate measurement notifications from the HRS device by using the value handle obtained after adding the service in the previous step.

    ```
    HRS>gatt notify 0 14
    ```

8.  A notification appears on the Collector console.

    ```
    Collector>
    --> GATT Event: Received Notification
    Handle: 14
    Value: B400
    ```



```{include} ../topics/extended_advertising_513.md
:heading-offset: 3
```

```{include} ../topics/rssi_monitor_513.md
:heading-offset: 3
```

```{include} ../topics/ble_shell_tak.md
:heading-offset: 3
```

**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)

