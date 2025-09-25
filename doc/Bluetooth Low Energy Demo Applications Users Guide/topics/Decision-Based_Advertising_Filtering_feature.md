# Decision-Based Advertising Filtering \(DBAF\) feature

The Bluetooth LE Shell application also supports the Decision-Based Advertising Filtering \(DBAF\) feature, which can be enabled by performing the following steps:

-   In `app_preinclude.h` file, set `BLE_SHELL_DBAF_SUPPORT` to `1`.

This feature requires two devices:

-   GAP Peripheral: transmits decision PDUs \(ADV\_DECISION\_IND\).

-   GAP Central: scans for decision PDUs and handles filtering policies.


To showcase the functionality, two platforms are used in the following setup.

**Steps to perform on the GAP Peripheral device:**

1.  Configure the extended advertising parameters to use decision PDUs. In the below example, the advertising type is set to connectable and includes TX power, uses decision PDUs and includes AdvA in the decision PDU. The primary PHY is set to Coded PHY.

2.  Configure the extended advertising data using the `gap extadvdecdata` command. The resolvable tag and/or arbitrary data can be set using the parameters available.

3.  Start extended advertising.

    ```
    BLE Shell>gap extadvcfg -phy1 3 -type 449
    BLE Shell>gap extadvdecdata -key 112233445566778899AABBCCDDEEFF00 -prand 5AC317 -decdata 6362 -datalen 2 -restag 0
    BLE Shell>gap extadvstart
    BLE Shell>
    --> GAP Event: Extended Advertising parameters successfully set.
    BLE Shell>
    --> GAP Event: Extended Advertising data successfully set.
    BLE Shell>
    --> GAP Event: Extended Advertising Decision Data Setup Complete.
    BLE Shell>
    --> GAP Event: Advertising state changed successfully!
    BLE Shell>
    ```


**Steps to perform on the GAP Central device:**

1.  Set the scanning parameters to scan only decision PDUs. The scanning PHY is set to match the advertising PHY, in this case Coded PHY.

2.  Set the connection parameters to use only decision PDUs and the connection initiating PHYs corresponding to the primary PHY on which the advertising is performed.

    ```
    BLE Shell>gap scancfg -phy 4 -filter 12
    BLE Shell>gap connectcfg -phy 4 -filter 2
    --> Connection Parameters:
     --> Connection Interval: 200 ms
     --> Connection Latency: 0
     --> Supervision Timeout: 32000 ms
     --> Connecting PHYs: Coded
     --> Connection Filter Policy: 2
    BLE Shell>
    ```

3.  Add decision instructions using the `gap adddecinstr` command. A maximum of `gMaxNumDecisionInstructions_c` instructions can be added. If the set of instructions must be changed, the `gap deldecinstr` command deletes all current instructions.

    ```
    BLE Shell>gap adddecinstr -group 1 -field 0 -criteria 1 -restagkey 112233445566778899AABBCCDDEEFF00
    BLE Shell>gap adddecinstr -group 1 -field 6 -criteria 1 -advmode 6
    BLE Shell>gap adddecinstr -group 0 -field 24 -criteria 1 -arbmask 00000000ffffff
    BLE Shell>gap adddecinstr -group 0 -field 9 -criteria 1 -advacheck 2 -add1type 0 -add1 a6fb0d376000 -add2type 0 -add2 a5fb0d376000
    BLE Shell>gap adddecinstr -group 0 -field 7 -criteria 1 -rssimin -80 -rssimax 0
    BLE Shell>gap adddecinstr -group 0 -field 8 -criteria 5 -lossmin 0 -lossmax 50
    BLE Shell>
    ```

4.  Set the decision instructions using the `gap setdecinstr` command. The instructions are used when listening for advertisements containing decision PDUs.

5.  Start scanning and filter duplicates.

6.  Connect to the desired device in the scanned devices list.

    ```
    BLE Shell>gap setdecinstr
    BLE Shell>
    --> GAP Event: Decision Instructions Setup Complete.
    BLE Shell>gap scanstart filter
    BLE Shell>
    -> GAP Event: Scan started.
    BLE Shell>
    --> GAP Event: Found device 0 : C4603770BCC5 -23 dBm
     Advertising Extended Data:
    gap connect 0
    BLE Shell>
    -> GAP Event: Scan stopped.
    BLE Shell>
    --> GAP Event: Connected to peer 0
    BLE Shell>
    ```


**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)

