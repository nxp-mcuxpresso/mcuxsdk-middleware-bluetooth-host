# Bluetooth Low Energy OTAP service-profile

The Bluetooth Low Energy OTAP Service is implemented using the GATT Server which runs on the OTAP Client \(GAP Peripheral\).

The Bluetooth LE OTAP Service does not require any other Bluetooth LE services because it is a custom service it has a 128-bit UUID. The service has 2 custom characteristics which also have 128-bit UUIDs.

The service must be included in the GATT database of the GATT Server as described in [Creating GATT database](creating_gatt_database.md) of this document.


```{include} ../topics/otap_service_and_characteristics.md
:heading-offset: 2
```

```{include} ../topics/otap_server_and_otap_client_interactions.md
:heading-offset: 2
```

**Parent topic:**[Over the Air Programming \(OTAP\)](../topics/over_the_air_programming_otap.md)

