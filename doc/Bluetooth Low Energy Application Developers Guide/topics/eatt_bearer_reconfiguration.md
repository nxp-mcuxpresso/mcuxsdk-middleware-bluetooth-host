# EATT Bearer reconfiguration

One of the advantages of Enhanced ATT bearers over the Unenhanced ATT bearer is the ability to increase the MTU multiple times. To reconfigure the MTU and/or MPS of existing Enhanced ATT bearers, the `Gap_EattReconfigureRequest` should be used. If a `mps` value of `0` is given, the maximum available MPS value for that channel is used.

For example, in order to reconfigure the MTU of two bearers from 64 to 128 the application may call `Gap_EattReconfigureRequest` as shown below:

```

bleResult_t result = gBleSuccess_c;
bearerId_t aBearerIds[2] = {1U, 2U};
result = Gap_EattReconfigureRequest(peerDeviceId,
                                    128U,
                                    0U,
                                    2U,
                                    aBearerIds);
if (gBleSuccess_c != result)
{
    /* Treat error */
}

```

The application should monitor the `gConnEvtEattChannelReconfigureResponse_c` connection event of type `gapEattReconfigureResponse_t` for the result.

The procedure triggered by `Gap\_EattReconfigureRequest` updates only the local MTU. The `ATT_MTU` for Enhanced ATT bearers is the minimum of the MTU values of the two devices.

**Parent topic:**[Enhanced ATT](../topics/enhanced_att.md)

