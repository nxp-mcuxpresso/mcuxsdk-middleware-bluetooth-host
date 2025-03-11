# Throughput feature

The Bluetooth LE Shell application also has a throughput test feature that can be used to test different combinations of the parameters \(connection interval, payload size, and packet count\) to determine the best data-rate.

This feature requires two devices:

-   GAP Peripheral: transmits the test packets
-   GAP Central: receives the packets and displays a report

All throughput-related commands are grouped under the `**thrput**` keyword:

-   `thrput setparam`: configures connection interval, packet count and payload size.
-   `thrput start tx`: configures the device as a GAP Peripheral and starts advertising. Once the receiving device is connected, the packet transmission begins. The packet size and count can also be specified \(`-s`<size\_value\>`-c`<count\_value\>\).
-   `thrput start rx`: configures the device as a GAP Central and starts scanning. Once a transmitter device is found, it connects to it and waits for the test to begin. The connection interval can also be configured \(`-ci`<value\>\).
-   `thrput stop`: stops the test and disconnects the devices.

Once a connection is established between the devices and initial throughput test is complete, one can start a new throughput transmission test with a new set of parameters \(packet size / count\).

The receiving device generates the report if no packets are received for more than three consecutive connection events.

The default configuration of the throughput test is the following:

-   Packet count: 1000

-   Payload size: 20 bytes


Connection interval \(min, max\): 160, 160 \(200 ms\)

The example of a test report is shown below:

```

BLE Shell>thrput start tx
BLE Shell>
-->  GAP Event: Advertising parameters successfully set.
BLE Shell>
-->  GAP Event: Advertising data successfully set.
BLE Shell>
-->  GAP Event: Advertising started.
-->  GAP Event: Connected to peer 0
BLE Shell>
-->  GAP Event: Advertising Throughput test started.
Sending packets...
-->  MTU Exchanged.
BLE Shell>
Throughput test with peer 0 has finished.

BLE Shell>thrput start rx
BLE Shell>
->  GAP Event: Scan started.
Found device:
THR_PER
0060375BCEC6
->  GAP Event: Scan stopped.
-->  GAP Event: Connected to peer 0
BLE Shell>Throughput test started.
Receiving packets...


************************************
***** TEST REPORT FOR PEER ID 0 ****
************************************
Packets received: 1000
Total bytes: 244000
Receive duration: 5017 ms
Average bitrate: 389 kbps
************************************
********** END OF REPORT ***********


```



**Parent topic:**[Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)

