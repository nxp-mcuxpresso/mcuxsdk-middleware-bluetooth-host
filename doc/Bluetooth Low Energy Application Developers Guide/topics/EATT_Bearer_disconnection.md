# EATT Bearer disconnection

Individual Enhanced ATT bearers can be disconnected by calling the `Gap_EattDisconnect` API as shown below:

```

bleResult_t result = gBleSuccess_c;
result = Gap_EattDisconnect(peerDeviceId, bearerId);
if (gBleSuccess_c != result)
{
    /* Treat error */
}

```

The application should look for a connection event of type `gEnhancedBearerDisconnected_c` in the connection callback.

**Parent topic:**[Enhanced ATT](../topics/enhanced_att.md)

