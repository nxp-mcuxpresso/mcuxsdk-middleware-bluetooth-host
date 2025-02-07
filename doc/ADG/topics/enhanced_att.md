# Enhanced ATT

The Enhanced ATT protocol allows concurrent transactions to be handled by the stack. The sequential transaction rule still exists when EATT is used, but its scope is now defined as being per instance of the Enhanced ATT Bearer. EATT transactions might execute in parallel if they are supported by distinct L2CAP channels, which use the Enhanced Credit Based Flow Control Mode \(that is, distinct Enhanced ATT Bearers\).

When using an Enhanced ATT Bearer, ATT MTU and L2CAP MTU are independently configurable and may be reconfigured during a connection. An increase to the MTU is allowed but reducing its size is not. Allowing MTU to be increased without needing to reestablish the connection has an advantage. It eliminates the risk of a second application using the stack, being unable to continue, due to the previously negotiated MTU being too small.

Enhanced ATT bearers are identified through Bearer Ids. Enhanced ATT Bearer Ids are assigned internally and have a valid range between 1 and 251. The Unenhanced ATT bearer is always available for a connected peer device and has the *BearerId* *0*.


```{include} ../topics/eatt_credits_management.md
:heading-offset: 2
```

```{include} ../topics/eatt_connection_establishment.md
:heading-offset: 2
```

```{include} ../topics/eatt_bearer_reconfiguration.md
:heading-offset: 2
```

```{include} ../topics/EATT_Bearer_disconnection.md
:heading-offset: 2
```

**Parent topic:**[Generic Access Profile \(GAP\) Layer](../topics/generic_access_profile_gap_layer.md)

