#  Periodic Advertising with Responses \(PAwR\) feature

The Bluetooth LE Shell application also supports the Periodic Advertising with Responses feature, which can be enabled by performing the following steps:

- Flash the NBU: for KW45B41Z-EVK/K32W148-EVK/FRDM-MCXW71 flash the experimental NBU found in `middleware/wireless/ble_controller/bin/experimental`; for KW47-EVK/MCXW72-EVK/FRDM-MCXW72 flash the all-purpose NBU found in `middleware/wireless/ble_controller/bin`.
- Update the application project so that it uses the experimental Bluetooth LE Host library, `middleware/wireless/bluetooth/host/lib_exp/lib_ble_OPT_host_cm33_iar.a`.
- In `app_preinclude.h` file, set `BLE_SHELL_PAWR_SUPPORT` to `1`.

This feature requires two (or more) devices:

- GAP Broadcaster: transmits advertising PDUs at a fixed interval. \(AUX\_SYNC\_SUBEVENT\_IND\)

- GAP Observer (one or more): scans for advertising PDUs and synchronizes with the advertising train. Sends responses PDUs in the synced subevents. \(AUX\_SYNC\_SUBEVENT\_RSP\)

To showcase the functionality, two platforms are used in the following setup.


**Steps to perform on the GAP Broadcaster device:**

1. Configure the periodic advertising parameters. In the below example, a set of implicit values are used. There are 2 subevents per interval, each interval repeating at 1 second. There are 4 response slots configured per subevent, with the first response slot starting at 125ms from the advertising packet.

    ```
    BLE Shell>gap periodiccfg

    -->  Periodic Advertising Parameters:
        -->  Periodic Advertising Handle: 1
        -->  Periodic Advertising Interval: 2000 ms
        -->  Number of subevents: 2
        -->  Interval between subevents: 156.25 ms
        -->  Time between the advertising packet in a subevent and the first response slot: 125.0 ms
        -->  Time between response slots: 1.250 ms
        -->  Number of response slots: 4
    BLE Shell>
    -->  GAP Event: Periodic Advertising parameters successfully set.
    ```

    Alternatively, the command does accept all the mentioned parameters to be changed. The below command can be used to configure the exact same values.

    ```
    BLE Shell>gap periodiccfg -numsubevents 2 -subint 125 -rspslotdelay 100 -rspslotspace 10 -numrspslot 4
    BLE Shell>
    -->  GAP Event: Periodic Advertising parameters successfully set.
    ```

2. Configure the subevent data on the advertiser for each subevent. Each instance of the `gap periodicsubeventdata` command sets each subevent data for the configured number of subevents. In this used example, the first command sets the data for the first subevent, the second command sets the data for the second subevent. The upper limit is the lowest of the following: `SHELL_PER_ADV_MAX_NUM_SUBEVENTS` from `app_preinclude.h` or the number of subevents set using `gap periodiccfg`.

    ```
    BLE Shell>gap periodicsubeventdata 255 696E666F31
    BLE Shell>gap periodicsubeventdata 255 696E666F32
    ```

    When the last subevent data is set, any succeeding command will erase the previously set data for all subevents, and the data in the ongoing command will be set in the first subevent.

    Alternatively, the command accepts more advertising data structures for one subevent, up to `SHELL_EXT_ADV_DATA_MAX_AD_STRUCTURES`.

    ```
    BLE Shell>gap periodicsubeventdata 27 01CAFECAFECAFE 255 696E666F31
    ```

    To manually clear the data set for all the currently set subevents, you may issue the following command:

    ```
    BLE Shell>gap periodicsubeventdata -erase
    ```

3. Start extended advertising.

    ```
    BLE Shell>gap extadvstart
    BLE Shell>
    -->  GAP Event: Extended Advertising parameters successfully set.

    BLE Shell>
    -->  GAP Event: Extended Advertising data successfully set.

    BLE Shell>
    -->  GAP Event: Advertising state changed successfully!
    ```

4. Start periodic advertising.

    ```
    BLE Shell>gap periodicstart
    BLE Shell>Periodic Advertising state changed successfully!
    ```


**Steps to perform on the GAP Observer device:**
1. Start scanning using the `gap scanstart` command. Optionally you can modify the scan parameters using the `gap scancfg` command.

    ```
    BLE Shell>gap scanstart
    BLE Shell>
    ->  GAP Event: Scan started.
    ```

2. Issue a periodic sync command with the target address of the Broadcaster device that is periodic advertising. You can use the `gap address` command on the Broadcaster to obtain the address.

    ```
    BLE Shell>gap periodicsync -peer 006037A55E86
    ```

    When the sync is established, events will be printed at the console.

    ```
    -->  GAP Event: Periodic Advertising Sync Established
    -->  GAP Event: Periodic V2 Device Scanned
            Event Counter: 8
            Subevent Synced: 0
            RSSI: -38 dBm
            Advertising Data: info1
    ```

3. Synchronize the Observer device with the subevents needed. For this example, we will synchronize the Observer with both subevents using the following command. Implicitly, the Observer synchronizes to subevent 0.

    ```
    BLE Shell>gap periodicsyncsubevent -peradvproperties 0 -numsubevents 2 -subevents 0x00,0x01
    ...
    BLE Shell>Set Sync Subevent command successfully completed.
    ```

    After this, GAP Events of type Periodic V2 Device Scanned should be displayed in the console for each of the subevents synchronized.

    ```
    -->  GAP Event: Periodic V2 Device Scanned
            Event Counter: 169
            Subevent Synced: 0
            RSSI: -34 dBm
            Advertising Data: info1
    -->  GAP Event: Periodic V2 Device Scanned
            Event Counter: 169
            Subevent Synced: 1
            RSSI: -38 dBm
            Advertising Data: info2
    ```

4. The last step to have a complete exchange of data is to set the response(s) on the Observer device. The next 2 commands will set the data for the 2 subevents synced at the previous step.

    ```
    BLE Shell>gap periodicresponsedata -responsesubevent 0 -responseslot 0 -data 255 64657669636531696E666F31
    BLE Shell>gap periodicresponsedata -responsesubevent 1 -responseslot 0 -data 255 64657669636531696E666F32
    ```

    The `-responsesubevent` parameter indicates the subevent the data is set for. The `-responseslot` parameter indicates the response slot the data is sent at this value should be within the range 0 to the number of response slots set at `periodiccfg`.

    To scale the demo to use more Observer devices, repeat steps 1-3 for each of the added device. Also use example at step 4, with the adaptation of the `-responseslot` according to the device index. The demo is designed to have a new device for each of the response slots available. Also, change the data to reflect the response of the extra device.

    **_Note:_** _The `-data` parameter must be the last of the `periodicresponsedata` command as it can contain a volatile number of advertising data structures._

    Alternatively, the command may contain additional advertising data structures.

    ```
    BLE Shell>gap periodicresponsedata -responsesubevent 0 -responseslot 0 -data 27 01CAFECAFECAFE 255 64657669636531696E666F31
    BLE Shell>gap periodicresponsedata -responsesubevent 1 -responseslot 0 -data 27 01CAFECAFECAFE 255 64657669636531696E666F32
    ```

    To manually clear the data set for all the currently set subevents, you may issue the following command:

    ```
    BLE Shell>gap periodicresponsedata -erase
    ```


**Initiate a connection from PAwR**

PAwR feature allows for connection initiation from the Broadcaster device. To do this, the devices have to be synchronized. The minimum steps required for this are steps 1, 3, 4 on the GAP Broadcaster device and steps 1, 2, GAP Observer device.

After performing the mentioned steps, the Broadcaster device must issue the following command:

```
BLE Shell>gap connectpawr -subevent 0 -peer 00603728063E -peeraddrtype 0
```

Upon successful connection the following message will be displayed.

```
-->  GAP Event: Connected to peer 0
```


**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)
