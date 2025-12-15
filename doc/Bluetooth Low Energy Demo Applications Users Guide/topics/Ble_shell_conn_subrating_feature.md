#  Connection Subrating feature

The Bluetooth LE Shell application also supports the Connection Subrating feature, which can be enabled by performing the following steps:

- In `app_preinclude.h` file, set `BLE_SHELL_CONN_SBR_SUPPORT` to `1`.

This feature requires two connected devices to be demonstrated. There are three ble_shell commands implemented to handle this feature.

**Connection subrating configuration**

- gap sbrcf does not require a connection to be established.

- gap sbrcf can be issued without any parameters to return the default configuration.

- optional parameters can be used to configure connection subrating: gap sbrcf[-sbrmin minSubrateFactor] [-sbrmax maxSubrateFactor] [-latency latency] [-contnum continuationNumber] [-timeout timeout in ms]

```
   BLE Shell>gap sbrcfg

-->  Connection Subrate Parameters:
    -->  subrateMin: 4
    -->  subrateMax: 4
    -->  Connection Latency: 0
    -->  continuationNumber: 2
    -->  Supervision Timeout: 32000 ms
BLE Shell>
```
**Set default connection subrating parameters**

- gap setdefsbrparam does not require a connection to be established. 
- It can be issued only on the central side.
- It sets the parameters exposed/set by the gap sbrcfg command.

```
BLE Shell>gap setdefsbrparam
BLE Shell>
-->  GAP Event: Default Connection Subrate Parameters Setup Complete.

BLE Shell>
```
**Connection subrate request**

- gap connsbrreq peerID requires a connection to be established and the peerId as parameter. 
- It can be issued on the central or on the peripheral side.
- When the connection subrating is applied the subrate change event is received.

```
BLE Shell>
-->  GAP Event: Subrate Changed 0
    -->  subrateFactor: 4
    -->  peripheralLatency: 0
    -->  continuationNumber: 2
    -->  supervisionTimeout: 32000 ms
BLE Shell>
```
**Usage**

To showcase the functionality, the throughput feature can be used.

```
BLE Shell>thrput start rx
BLE Shell>
->  GAP Event: Scan started.

Found device:
THR_PER
006037F3C4FF
->  GAP Event: Scan stopped.

-->  GAP Event: Connected to peer 0
BLE Shell>Throughput test started.
Receiving packets...

************************************
***** TEST REPORT FOR PEER ID 0 ****
************************************

Packets received: 1000
Total bytes: 244000
Receive duration: 3977 ms
Average bitrate: 490 kbps

************************************
********** END OF REPORT ***********
************************************

BLE Shell>gap setdefsbrparam
BLE Shell>
-->  GAP Event: Default Connection Subrate Parameters Setup Complete.

BLE Shell>gap connsbrreq 0
BLE Shell>
-->  GAP Event: Subrate Changed 0
    -->  subrateFactor: 4
    -->  peripheralLatency: 0
    -->  continuationNumber: 2
    -->  supervisionTimeout: 32000 ms
BLE Shell>thrput start 0 rx
Throughput test started.
Receiving packets...
BLE Shell>
************************************
***** TEST REPORT FOR PEER ID 0 ****
************************************

Packets received: 1000
Total bytes: 244000
Receive duration: 4004 ms
Average bitrate: 487 kbps

************************************
********** END OF REPORT ***********
************************************
```
As can be observed the throughput with the subrating applied is almost similar with the throughput without subrating while the energy efficiency when no data is being transmitted is improved given the effective connection interval is subrate factor bigger.

**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)
