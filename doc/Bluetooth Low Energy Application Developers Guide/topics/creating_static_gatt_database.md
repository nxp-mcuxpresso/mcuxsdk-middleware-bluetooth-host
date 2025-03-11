# Creating static GATT database

To define a GATT Database at compile-time, several macros are provided by the GATT\_DB API. These macros expand in many different ways at compilation, generating the corresponding *Attribute Database* on which the Attribute Protocol \(ATT\) may operate.

This is the default way of defining the database.

The GATT Database definition is written in two files that are required to be added to the application project together with all macro expansion files:

-   ***gatt\_db.h*** - contains the actual declaration of Services and Characteristics.
-   ***gat\_uuid128.h*** – contains the declaration of Custom UUIDs \(16-byte wide\); these UUIDs are given a user-friendly name that is used in *gatt\_db.h* file instead of the entire 16-byte sequence.


```{include} ../topics/declaring_custom_128-bit_uuids.md
:heading-offset: 2
```

```{include} ../topics/declaring_a_service.md
:heading-offset: 2
```

```{include} ../topics/declaring_a_characteristic.md
:heading-offset: 2
```

```{include} ../topics/static_gatt_database_definition_examples.md
:heading-offset: 2
```

**Parent topic:**[Creating GATT database](../topics/creating_gatt_database.md)

