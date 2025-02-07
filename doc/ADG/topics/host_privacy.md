# Host privacy

To enable or disable Host Privacy, the following API may be used:

```
bleResult_t **Gap\_EnableHostPrivacy**
(
    bool_t              enable,
    const uint8_t *     aIrk
);
```

When *enable* is set to TRUE, the *aIrk*parameter defines which type of Private Address to generate. If *aIrk* is NULL, then a new NRPA is generated periodically and written into the Controller. Otherwise, an IRK is copied internally from the *aIrk* address and it is used to periodically generate a new RPA.

The lifetime of the Private Address \(NRPA or RPA\) is a number of seconds contained by the *gGapHostPrivacyTimeout*external constant, which is defined in the *ble\_config.c* source file. The default value for this is 900 \(15 minutes\).

When Host Privacy is enabled, the Host ignores the ownAddressType value for the advertising, scanning or connect parameters. It will always use the random address type in order to use the RPA configured in the Controller in the packets sent over the air.

As mentioned in the Introduction section, call this API for random address generation. For random address resolution there is no need to do so, it is performed by default against the bonded devices list.

**Parent topic:**[Privacy feature](../topics/privacy_feature.md)

