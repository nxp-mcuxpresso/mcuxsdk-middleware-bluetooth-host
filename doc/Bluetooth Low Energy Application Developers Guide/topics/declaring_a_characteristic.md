# Declaring a characteristic

A Characteristic must only be declared inside a Service. It belongs to the most recently declared Service, so the GATT Database must always begin with a Service declaration.

The Characteristic declaration attribute has the following UUID, as defined by the Bluetooth SIG:

-   0x2803 a.k.a. *<<Characteristic\>\>*

The Characteristic declaration attribute value contains:

-   the *Characteristic UUID*
-   the *Characteristic Value* ’s declaration handle
-   the *Characteristic Properties* – Read, Write, Notify, and so on. \(1 byte of flags\)

The *Characteristic Range*starts from the Characteristic declaration and ends before a new Characteristic or a Service declaration.

After the Characteristic declaration these follow:

-   A *Characteristic Value* declaration \(mandatory; immediately after the Characteristic declaration\).
-   Zero or more *Characteristic Descriptor* declarations.


```{include} ../topics/characteristic_declaration_macros.md
:heading-offset: 3
```

```{include} ../topics/declaring_characteristic_values.md
:heading-offset: 3
```

```{include} ../topics/declaring_characteristic_descriptors.md
:heading-offset: 3
```

**Parent topic:**[Creating static GATT database](../topics/creating_static_gatt_database.md)

