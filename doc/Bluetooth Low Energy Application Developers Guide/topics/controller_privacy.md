# Controller privacy

To enable or disable Controller Privacy, the following API may be used:

```
bleResult_t Gap_EnableControllerPrivacy
(
    bool_t                               enable,
    const uint8_t *                      aOwnIrk,
    uint8_t                              peerIdCount,
    const gapIdentityInformation_t*      aPeerIdentities
);
```

When *enable* is set to TRUE, *aOwnIrk*parameter shall not be NULL, *peerIdCount* shall not be zero or greater than *gMaxResolvingListSize\_c*, and *aPeerIdentities* shall not be NULL.

The IRK defined by *aOwnIrk* is used by the Controller to periodically generate a new Resolvable Private Address \(RPA\). The lifetime of the RPA is a number of seconds contained by the *gGapControllerPrivacyTimeout* external constant, which is defined in the *ble\_globals.c* source file. The default value for this is 900 \(15 minutes\). The *gGapControllerPrivacyTimeoutMin* and *gGapControllerPrivacyTimeoutMax* external constants, also defined in *ble\_globals.c*, contain the minimum and maximum values for the RPA lifetime if the Controller supports the Randomized RPA Timeout feature. The default value for both is 900 - same behavior as the non-randomized timeout.

The *aPeerIdentities* is an array of identity information for each bonded device. The identity information contains the device’s identity address \(public or random static address\) and the device’s IRK. This array can be obtained from the Host with the *Gap\_GetBondedDevicesIdentityInformation*API.

Enabling Controller Privacy involves a quick sequence of commands to the Controller. When the sequence is complete, the *gControllerPrivacyStateChanged\_c* generic event is triggered.


```{include} ../topics/privacy_mode.md
:heading-offset: 3
```

```{include} ../topics/scanning_and_initiating.md
:heading-offset: 3
```

```{include} ../topics/advertising.md
:heading-offset: 3
```

```{include} ../topics/connected.md
:heading-offset: 3
```

**Parent topic:**[Privacy feature](../topics/privacy_feature.md)

