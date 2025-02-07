# Writing and reading attributes

These are the two functions to perform basic attribute operations from the application:

```
bleResult_t **GattDb\_WriteAttribute
**(
    uint16_t           handle,
    uint16_t           valueLength,
    const uint8_t *    aValue
);
```

The value length must be valid, as defined when the database is created. Otherwise, a *gGattInvalidValueLength\_c* error is returned.

Also, if the database is created statically, as explained in [Creating GATT database](creating_gatt_database.md#), the *handle* may be referenced through the enumeration member with a friendly name defined in the *gatt\_db.h*.

```
bleResult_t **GattDb\_ReadAttribute**
(
    uint16_t     handle,
    uint16_t     maxBytes,
    uint8_t *    aOutValue,
    uint16_t *   pOutValueLength
);
```

The *aOutValue* array must be allocated with the size equal to *maxBytes*.

**Parent topic:**[GATT database application interface](../topics/gatt_database_application_interface.md)

