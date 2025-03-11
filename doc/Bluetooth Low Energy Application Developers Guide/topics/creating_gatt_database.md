# Creating GATT database

The GATT Database contains several *GATT Services* where each Service must contain at least one *GATT Characteristic*.

The Attribute Database contains a collection of *attributes*. Each attribute has four fields:

-   The *attribute handle* – a 2-byte database index, which starts from 0x0001 and increases with each new attribute, not necessarily consecutive; maximum value is 0xFFFF.
-   The *attribute type* or *UUID* – a 2-byte or 16-byte UUID.
-   The *attribute permissions* – 1 byte containing access flags; this defines whether the attribute’s value can be read or written and the security requirements for each operation type
-   The *attribute value* – an array of maximum 512 bytes.

The ATT does not interpret the UUIDs and values contained in the database. It only deals with data transfer based on the attributes’ handles.

The GATT gives meaning to the attributes based on their UUIDs and groups them into Characteristics and Services.

There are two possible ways of defining the GATT database:

-   At compile-time \(statically\) or
-   At runtime \(dynamically\)


```{include} ../topics/creating_static_gatt_database.md
:heading-offset: 1
```

```{include} ../topics/creating_a_gatt_database_dynamically.md
:heading-offset: 1
```

```{include} ../topics/gatt_caching.md
:heading-offset: 1
```

