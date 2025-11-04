#  Monitoring Advertisers \(MonAdv\) feature

The Bluetooth LE Shell application also supports the Monitoring Advertisers feature, which can be enabled by performing the following steps:

- In `app_preinclude.h` file, set `BLE_SHELL_MONADV_SUPPORT` to `1`.

This feature requires two (or more) devices:

- GAP Broadcaster (one or more): transmits advertising PDUs.

- GAP Observer: scans for advertising PDUs and shall generate an event if, during the timeout period set by the Application, an advertisement has not been received from that peer with an RSSI value above the RSSI low threshold set by the Application.

To showcase the functionality, two platforms are used in the following setup.

**Steps to perform on the GAP Broadcaster device:**

1. Get the device address of the target device to be monitored:

```
    BLE Shell>gap address

    -->  GAP Event: Random Static Address:D7E448669EA4
```

Note the device address to be used later, at the scanner device.

2. Configure advertising parameters and start advertising.

For this example we will set advertising at 100 ms interval.

```
    BLE Shell>gap advcfg -interval 100
    BLE Shell>
    -->  GAP Event: Advertising parameters successfully set.
```

```
    BLE Shell>gap advstart
    BLE Shell>
    -->  GAP Event: Advertising parameters successfully set.

    BLE Shell>
    -->  GAP Event: Advertising data successfully set.

    BLE Shell>
    -->  GAP Event: Advertising state changed successfully!
```

**Steps to perform on the GAP Observer device:**

1. Using the device address previously obtained, add it to the Monitoring Advertisers list.

```
    BLE Shell>gap monadvadd -addr D7E448669EA4 -type 1 -rssilow -65 -rssihigh -55 -timeout 1
    BLE Shell>
    -->  GAP Event: Device added to Monitored Advertisers List.
```

* The `-type` and `-addr` parameters are used to identify an advertising device.
* The `-rssilow` parameter is set to the RSSI value below which a Monitored Advertisers Report event shall be generated when the associated timer expires for that device.
* The `-rssihigh` parameter is set to the RSSI value equal to or above which a Monitored Advertisers Report event may be triggered for that device.
* The `-timeout` parameter is set to the timeout time in seconds for the device.

2. Start the scanning using the default, predefined parameters:

```
    BLE Shell>gap scanstart
    BLE Shell>
    ->  GAP Event: Scan started.

    BLE Shell>
    -->  GAP Event: Found device 0 : 18C23C3C23C1 -64 dBm
    -->  GAP Event: Found device 1 : 7A7410BFD06C -55 dBm
    -->  GAP Event: Found device 2 : 18C23C3C23CD -61 dBm
    ...
```

3. Enable monitoring advertisers.

```
    BLE Shell>gap monadven -enable 1
    BLE Shell>
    -->  GAP Event: GAP Event: Monitoring Advertisers enabled.
```

If the devices are distant to each other, the event message should be printed at the console:

```
    BLE Shell>
    -->  GAP Event: Monitored Advertiser Report
        Address Type: 1
        Address: D7E448669EA4
        Condition: RSSI Low Threshold
```

If the devices are brought close together, the event message should be printed at the console:

```
    -->  GAP Event: Monitored Advertiser Report
        Address Type: 1
        Address: D7E448669EA4
        Condition: RSSI High Threshold
```

**To restart the Monitoring Advertisers reporting**

Reuse the same command and same parameters as the enable command.

```
    BLE Shell>gap monadven -enable 1
    BLE Shell>
    -->  GAP Event: Monitoring Advertisers restarted.
```

**To stop the Monitoring Advertisers reporting**

```
    BLE Shell>gap monadven -enable 0
    BLE Shell>
    -->  GAP Event: Monitoring Advertisers disabled.
```

**To remove a device from the Monitoring Advertisers list:**

```
    BLE Shell>gap monadvrem -addr D7E448669EA4 -type 1
    BLE Shell>
    -->  GAP Event: Device removed from Monitored Advertisers List.
```

**To clear the Monitoring Advertisers list of the added devices:**

Run the following command:

```
    BLE Shell>gap monadvclear
    BLE Shell>
    -->  GAP Event: Monitored Advertisers List cleared.
```

**To get the size of the Monitoring Advertisers:**

Run the following command:

```
    BLE Shell>gap monadvsize
```

The size should be printed as indicated in this output message:

```
    BLE Shell>
    -->  GAP Event: Monitored Advertisers List size: 8
```

**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)
