# Client APIs

A Client can configure the ATT MTU, discover Services and Characteristics, and initiate data exchanges.

All the functions have the same first parameter: a *deviceId* which identifies the connected device whose GATT Server is targeted in the GATT procedure. This is necessary because a Client may be connected to multiple Servers at the same time.

First, however, the application must install the necessary callbacks.


```{include} ../topics/installing_client_callbacks.md
:heading-offset: 2
```

```{include} ../topics/mtu_exchange.md
:heading-offset: 2
```

```{include} ../topics/service_and_characteristic_discovery.md
:heading-offset: 2
```

```{include} ../topics/reading_and_writing_characteristics.md
:heading-offset: 2
```

```{include} ../topics/reading_and_writing_characteristic_descriptors.md
:heading-offset: 2
```

```{include} ../topics/resetting_procedures.md
:heading-offset: 2
```

**Parent topic:**[Generic Attribute Profile \(GATT\) Layer](../topics/generic_attribute_profile_gatt_layer.md)

