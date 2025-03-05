# EATT Credits management

Credits for the L2CAP channels used by Enhanced ATT bearers may be managed internally if the `autoCreditsMgmt` parameter is set to TRUE in the `Gap_EattConnectionRequest` or `Gap_EattConnectionAccept` function call. Otherwise, the application is responsible for credits management.

If the application chooses to manage the credits of the L2CAP channels used as Enhanced ATT bearers, it should use the following function to send credits for a specified bearer to a peer device:

```
bleResult_t Gap_EattSendCredits
(
    deviceId_t  deviceId,
    bearerId_t  bearerId,
    uint16_t    credits
);
        
```

If the local credits or peer credits of the L2CAP channel used by an Enhanced ATT bearer reaches `0`, a`gConnEvtEattBearerStatusNotification_c` connection event is updated with a status value of`gEnhancedBearerSuspendedNoLocalCredits_c`, or `gEnhancedBearerNoPeerCredits_c` respectively.

**Parent topic:**[Enhanced ATT](../topics/enhanced_att.md)

