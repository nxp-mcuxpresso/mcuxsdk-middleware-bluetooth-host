# RSSI Monitor

RSSI Monitor is an application that allows monitoring the RSSI of a remote peer on advertising or connection channel. The GAP peripheral device can modify the output TX power on both advertising and connection channels.

1.  **On GAP peripheral device**

    Set the primary advertising PHY to Coded PHY. Start advertising and read the address. Set the TX power in dBm to a value less than 20 dBm.

    ```
    BLE Shell>gap address
    BLE Shell>
    --> GAP Event: Public Address:C4603770BCC5
    BLE Shell>gap extadvcfg -phy1 3
    BLE Shell>gap extadvstart
    BLE Shell>
    --> GAP Event: Extended Advertising parameters successfully set.
    BLE Shell>
    --> GAP Event: Extended Advertising data successfully set.
    BLE Shell>
    --> GAP Event: Advertising state changed successfully!
    BLE Shell>gap txpower adv 0
    BLE Shell>
    --> GAP Event: Success!
    ```

2.  **On GAP Central device**

    Set the scanning PHY to Coded PHY. Start monitoring the RSSI on advertising Channel using the address of the Peripheral device. Scanning starts automatically, if it is not previously enabled.

    ```
    BLE Shell>gap scancfg -phy 4
    BLE Shell>gap rssimonitor C4603770BCC5--> Reading RSSI on advertising channel:
    BLE Shell>
    -> GAP Event: Scan started.
    BLE Shell>
     RSSI: -27 dBm
     RSSI: -27 dBm
     RSSI: -27 dBm
     RSSI: -27 dBm
     RSSI: -27 dBm
     RSSI: -29 dBm
                            
    ```

    In the below example, the RSSI in monitored on a connection channel. On GAP Peripheral, start advertising in connectable mode on Coded PHY and adjust the TX power level.

    ```
    BLE Shell>gap extadvcfg -type 65 -phy1 3
    BLE Shell>gap extadvdata 8 rssimonitortest
    BLE Shell>gap extadvstart
    BLE Shell>
    --> GAP Event: Extended Advertising parameters successfully set.
    BLE Shell>
    --> GAP Event: Extended Advertising data successfully set.
    BLE Shell>
    --> GAP Event: Advertising state changed successfully!
    BLE Shell>
    --> GAP Event: Connected to peer 0
    BLE Shell>
    --> GAP Event: Advertising stopped!
    BLE Shell>gap txpower conn 10
    BLE Shell>
    --> GAP Event: Success!
    ```

    On the GAP Central device, start scanning on the Coded PHY. Update the connection PHY also to Coded PHY, then connect to the remote device and monitor continuously the RSSI on the connection channel.

    ```
    BLE Shell>gap scancfg -phy 4
    BLE Shell>gap connectcfg -phy 4
    -->  Connection Parameters:
        -->  Connection Interval: 200 ms
        -->  Connection Latency: 0
        -->  Supervision Timeout: 32000 ms
        -->  Connecting PHYs: Coded
    BLE Shell>gap scanstart filter
    BLE Shell>
    ->  GAP Event: Scan started.
    BLE Shell>
    -->  GAP Event: Found device 0 : C4603770BCC5 -21 dBm
     Advertising Extended Data:
    rssimonitortest
    gap connect 0
    BLE Shell>
    ->  GAP Event: Scan stopped.
    BLE Shell>
    -->  GAP Event: Connected to peer 0
    BLE Shell>gap rssimonitor 0 -c
    -->  Reading RSSI from connected device:
    BLE Shell>
     RSSI: -22 dBm
     RSSI: -23 dBm
     RSSI: -21 dBm
     RSSI: -22 dBm
     RSSI: -22 dBm
    BLE Shell>gap rssistop
    ```

3.  Update the PHY preference and continue monitoring the RSSI. For coded PHY, the coding scheme can be configured between S2 and S8 \(500 kbit/s and 125 kbit/s\).

    ```
    BLE Shell>gap phy 0 -tx 4 -rx 4 -o 1
    BLE Shell>
    --> GAP Event: Phy update complete with peer 0
    --> TxPhy: Coded
    --> RxPhy: Coded
    
    BLE Shell>gap phy 0 -tx 2 -rx 2
    BLE Shell>
    --> GAP Event: Phy update complete with peer 0
    --> TxPhy: 2M
    --> RxPhy: 2M
    
    BLE Shell>gap rssimonitor 0 -c
    --> Reading RSSI from connected device:
    BLE Shell>
    RSSI: -23 dBm
    RSSI: -23 dBm
    RSSI: -21 dBm
    RSSI: -21 dBm
    --> GAP Event: Phy update complete with peer 0
    --> TxPhy: Coded
    --> RxPhy: Coded
    
    BLE Shell>
    RSSI: -22 dBm
    RSSI: -21 dBm
    RSSI: -22 dBm
    RSSI: -23 dBm
    RSSI: -23 dBm
    --> GAP Event: Phy update complete with peer 0
    --> TxPhy: 2M
    --> RxPhy: 2M
    BLE Shell>
    RSSI: -22 dBm
    RSSI: -21 dBm
    RSSI: -21 dBm
    RSSI: -20 dBm
    ```


**Parent topic:**[Usage](../topics/usage_513.md)

