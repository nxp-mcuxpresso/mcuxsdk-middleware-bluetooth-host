# Declaring a service

There are two types of Services:

-   *Primary Services*
-   *Secondary Services* - these are only to be included by other Primary or Secondary Services

The Service declaration attribute has one of these UUIDs, as defined by the Bluetooth SIG:

-   0x2800 a.k.a. *<<Primary Service\>\>* - for a Primary Service declaration
-   0x2801 a.k.a. *<<Secondary Service\>\>* - for a Secondary Service declaration

The Service declaration attribute permissions are read-only and no authentication required. The Service declaration attribute value contains the *Service UUID*. The *Service Range* starts from the Service declaration and ends at the next service declaration. All the Characteristics declared within the Service Range are considered to belong to that Service. For a more comprehensive list of SIG defied UUID values, check `ble_sig_defines.h`.


```{include} ../topics/service_declaration_macros.md
:heading-offset: 3
```

```{include} ../topics/include_declaration_macros.md
:heading-offset: 3
```

**Parent topic:**[Creating static GATT database](../topics/creating_static_gatt_database.md)

