# EATT Connection establishment

In order to take advantage of the Enhanced ATT features, first a number of Enhanced Bearers should be opened for a connected peer device. For this, the function below may be used to create up to five Enhanced ATT bearers at a time:

```
bleResult_t Gap_EattConnectionRequest
(
   deviceId_t  deviceId,
   uint16_t    mtu,
   uint8_t     cBearers,
   uint16_t    initialCredits,
   bool_t      autoCreditsMgmt
);
```

The `mtu` parameter specifies the MTU for all the bearers to be established.

The `cBearers` parameter is used to specify the number of Enhanced ATT bearers to be opened, and should have a value between 1 and 5. The `initialCredits` parameter specifies the initial number of credits of the L2CAP credit based channels used as Enhanced ATT bearers.

The `autoCreditsMgmt` parameter is used to tell the Bluetooth LE Host Stack if it should manage L2CAP channel credits automatically. If set to TRUE the Bluetooth LE Host Stack automatically sends credits to a peer device when exhausted in chunks of `initialCredits`.

For example, to establish two Enhanced ATT bearers with a peer device the application may call the `Gap_EattConnectionRequest` as shown below:

```
bleResult_t result = Gap_EattConnectionRequest(peerDeviceId,
                        64U,
                         2U,
                         3U,
                         TRUE);
if (gBleSuccess_c != result)
{
    /* Treat error */
}
```

If an EATT Connection Request is received from a peer device it would be signaled through the `gConnEvtEattConnectionRequest_c` connection event of type `gapEattConnectionRequest_t` sent to the connection callback. The application should handle this event by calling `Gap_EattConnectionAccept`. The example below shows how an application may accept an incoming EATT Connection Request with the same MTU as requested by the peer device.

```
case gConnEvtEattConnectionRequest_c:
{
    gapEattConnectionRequest_t *pEattConnectionReq = &pConnectionEvent->eventData.eattConnectionRequest;
    
    bleResult_t result = Gap_EattConnectionAccept(peerDeviceId,
                                                TRUE,
                                                pEattConnectionReq->mtu,
                                                3U,
                                                TRUE);
        
    if (gBleSuccess_c != result)
    {
        /* Treat error */
    }
}
break;
```

In case the `localMtu` specified when accepting a connection differs from the MTU requested by the peer device, the minimum of the two would become the MTU of the Enhanced Bearers.

After the `Gap_EattConnectionRequest` or `Gap_EattConnectionAccept` is called, for the result the application should wait for the `gConnEvtEattConnectionComplete_c` connection event of type `gapEattConnectionComplete_t` shown below:

```
typedef struct {
    l2caLeCbConnectionRequestResult_t           status;
    uint16_t                                    mtu;
    uint8_t                                     cBearers;
    bearerId_t                                  aBearerIds[gGapEattMaxBearers];
} gapEattConnectionComplete_t;
```

If successful, the `aBearerIds` array contains the bearer ids, for the Enhanced ATT bearers established. These ids may be used with the GATT Enhanced APIs in order to trigger GATT procedures over Enhanced ATT bearers.

**Parent topic:**[Enhanced ATT](../topics/enhanced_att.md)

