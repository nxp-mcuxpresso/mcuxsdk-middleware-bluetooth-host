# HCI entry and exit points

The HCI entry point of the Host Stack is the second function located in the *ble\_general.h* file:

```
void Ble_HciRecv
(
    hciPacketType_t packetType,
    void* pHciPacket,
    uint16_t packetSize
);
```

This is the function that the application must call to insert an HCI message into the Host.

An equivalent exists, to be used in ISR context:

```
bleResult_t Ble_HciRecvFromIsr
(
    hciPacketType_t     packetType,
    void*               pHciPacket,
    uint16_t            packetSize
);
```

Therefore, the *Ble\_HciRecv* function and the *hostToControllerInterface* parameter of the *Ble\_HostInitialize* function represent the two points that need to be connected to the LE Controller \(see [Bluetooth Low Energy Host Stack overview](../images/figure_1.png)\), either directly \(if the Controller software runs on the same chip as the Host\) or through a physical interface \(for example, UART\).

**Parent topic:**[Bluetooth LE Host Stack Initialization and APIs](../topics/bluetooth_le_host_stack_initialization_and_apis.md)

