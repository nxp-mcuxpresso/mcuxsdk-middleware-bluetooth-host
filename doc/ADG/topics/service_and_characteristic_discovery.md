# Service and characteristic discovery

There are multiple APIs that can be used for Discovery. The application may use any of them, according to its necessities.

All of the following APIs have an enhanced counterpart of the form *GattClient\_Enhanced\[procedure\]*. A *bearerId*parameter was added to specify on which bearer the transaction should take place. A value of *0* for the bearer Id identifies the Unenhanced ATT bearer. Values higher than *0* are used to identify the Enhanced ATT bearer used for the ATT procedure.


```{include} ../topics/discover_all_primary_services.md
:heading-offset: 3
```

```{include} ../topics/discover_primary_services_by_uuid.md
:heading-offset: 3
```

```{include} ../topics/discover_included_services.md
:heading-offset: 3
```

```{include} ../topics/discover_all_characteristics_of_a_service.md
:heading-offset: 3
```

```{include} ../topics/discover_characteristics_by_uuid.md
:heading-offset: 3
```

```{include} ../topics/discover_characteristic_descriptors.md
:heading-offset: 3
```

**Parent topic:**[Client APIs](../topics/client_apis.md)

