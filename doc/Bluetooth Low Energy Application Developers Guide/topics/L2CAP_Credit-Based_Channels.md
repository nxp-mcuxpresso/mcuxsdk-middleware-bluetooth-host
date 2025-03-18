# L2CAP credit-based channels

The L2CAP layer, which is responsible for protocol multiplexing, segmentation, and reassembly operations, allows devices to communicate via connection-oriented channels. These channels use credit-based flow control, in which a device grants each peer a number of credits which the peer can use to send packets. The number of credits is decremented with every sent packet. A device can grant more credits to its peers over the duration of the connection.

Unlike the fixed L2CAP CIDs used by protocols such as ATT and SMP, credit-based channels use dynamically allocated CIDS \(in the 0x0040-0xFFFF range\). The CIDs are automatically allocated by the Bluetooth LE Host Stack.

The Bluetooth LE Host Stack supports both the Credit-based Flow Control Mode and the Enhanced Credit-based Flow Control Mode. In the Enhanced Credit-based Flow Control Mode, devices can open up to five channels in a single connect request/response exchange. Additionally, these channels can be later reconfigured with new MTU and MPS values. In the Credit-based Flow Control Mode, reconfiguration is not possible.

The first thing an application must do is register the control and data callbacks:

```
bleResult_t   L2ca_RegisterLeCbCallbacks
(
    l2caLeCbDataCallback_t     pCallback,
    l2caLeCbControlCallback_t  pCtrlCallback
);
```

The control callback receives events related to channel management such as connection, disconnection, received credits, reconfiguration, and so on.

The data callback receives the data which is being exchanged on the channel.

To use L2CAP credit-based channels, the application must register a PSM. The PSM is analogous to a TCP/UDP port. It is an identifier used to determine the upper layer protocol which is making use of the L2CAP channel. The dynamic PSM range is 0x0080-0x00FF. The number of PSMs supported by an application can be configured at compile time via the `gL2caMaxLePsmSupported_c` define. The following API must be called in order to register a PSM:

```
bleResult_t  L2ca_RegisterLePsm
( 
   uint16_t     lePsm,
   uint16_t     lePsmMtu
);
```

The MTU configured via this API is used by every channel opened under the PSM, if the Credit-based Flow Control Mode is used. The minimum MTU is 23 and the maximum MTU is 65535.

When the Enhanced Credit-based Flow Control Mode is used, the MTU is specified at each connection request. In this mode, the minimum MTU is 64 and the maximum MTU is 65535.

The local MPS is not configurable by the application. It is set automatically by the Host Stack based on Controller capabilities. Usually, it will be 247.

A previously registered PSM can be deregistered:

```
bleResult_t L2ca_DeregisterLePsm
(
  uint16_t     lePsm
);
```

The number of credit-based channels that can be opened is configurable by the application via the `gL2caMaxLeCbChannels_c` define. This is the total number for all peers. To open a channel, the following API must be called:

```
bleResult_t L2ca_ConnectLePsm
(
   uint16_t     lePsm, 
   deviceId_t   deviceId,   
   uint16_t     initialCredits
);
```

To open up to five channels using Enhanced Credit-based Flow Control Mode, use this API:

```
bleResult_t  L2ca_EnhancedConnectLePsm
(
  uint16_t     lePsm,
  deviceId_t   deviceId,
  uint16_t     mtu,
  uint16_t     initialCredits,
  uint16_t     initialCredits,
  uint16_t     *aCids
);

```

The connect APIs must be called by both the initiator and the responder \(upon receiving the `gL2ca_LePsmConnectRequest_c` or `gL2ca_LePsmEnhancedConnectRequest_c` events in the application\).

If the responder does not wish to accept the connection request, it can use the following APIs:

```
 bleResult_t  L2ca_CancelConnection
    (
      uint16_t      lePsm,
      deviceId_t    deviceId,
     l2caLeCbConnectionRequestResult_t refuseReason
    );
 bleResult_t L2ca_EnhancedCancelConnection
    (
    uint16_t lePsm,
    deviceId_t deviceId,
    l2caLeCbConnectionRequestResult_t refuseReason,
    uint8_t noOfChannels,
    uint16_t *aCids
    );
```

When a channel has been successfully established, the `gL2ca_LePsmConnectionComplete_c` or `gL2ca_LePsmEnhancedConnectionComplete_c` events are received in the application.

To send data on a channel:

```
 bleResult_t L2ca_SendLeCbData
(
   deviceId_t      deviceId,
   uint16_t        channelId,
   const uint8_t*  pPacket,
   uint16_t        packetLength
);
```

The Host Stack keeps track of the credits granted to peers for each channel and decrements them accordingly. When a peer’s credit count reaches zero, the application is notified through the `gL2ca_NoPeerCredits_c` event and it can decide to send more credits to the peer for that channel:

```
 bleResult_t L2ca_SendLeCredit
 (
  deviceId_t     deviceId,
  uint16_t       channelId,
  uint16_t       credits
 );
```

The application can also choose to be notified when the number of credits allocated to a peer for a certain channel is nearing `0`, by setting the `gL2caLowPeerCreditsThreshold_c` macro to a non-zero value. When this limit is reached, the `gL2ca_LowPeerCredits_c` event is received and the application can choose to send more credits.

Similarly, when a device receives credits from a peer, the application is notified through the `gL2ca_LocalCreditsNotification_c` event. When the local device has used its last credit, it receives the same `gL2ca_LocalCreditsNotification_c` event with the `localCredits` field set to `0`. The packet that could not be sent due to exhausting the credits remains queued in the Host Stack and it is sent automatically when the local device receives credits from the peer.

To improve application flow control, two notification-type events are implemented by the Host Stack:

-   `gL2ca_ChannelStatusChannelBusy_c`
-   `gL2ca_ChannelStatusChannelIdle_c`

When the application sends a packet using `L2ca_SendLeCbData`, it receives a `gL2ca_ChannelStatusChannelBusy_c` event in the L2CAP control callback when the Host Stack begins sending the packet. When the Host Stack has sent the packet, a `gL2ca_ChannelStatusChannelIdle_c` event is received. The application can choose to use this event as a signal that it is safe to send the next packet.

To disconnect a channel:

```
bleResult_t L2ca_DisconnectLeCbChannel
(
    deviceId_t  deviceId,
    uint16_t    channelId
);
```

As mentioned previously, channels which use the Enhanced Credit-based Flow Control Mode can be reconfigured. This is achieved via the API:

```
bleResult_t L2ca_EnhancedChannelReconfigure
(
    deviceId_t     deviceId,
    uint16_t       newMtu,
    uint16_t       newMps,
    uint8_t        noOfChannels,
    uint16_t       *aCids
);
```

The reconfiguration request is automatically accepted by the Host Stack if parameters are valid \(as per the Bluetooth Core Spec v5.3, MTU cannot be lowered and MPS cannot be lowered for more than one channel\). On the responder, the `gL2ca_EnhancedReconfigureRequest_c` is received by the application in case of a successful reconfiguration, informing it of the new channel parameters. On the initiator, the `gL2ca_EnhancedReconfigureResponse_c` is received, informing the application about the received response or a timeout.

**Parent topic:**[Generic Access Profile \(GAP\) Layer](../topics/generic_access_profile_gap_layer.md)

