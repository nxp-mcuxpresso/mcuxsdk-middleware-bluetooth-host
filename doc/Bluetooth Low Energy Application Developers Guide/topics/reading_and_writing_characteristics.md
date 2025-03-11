# Reading and writing characteristics

All the APIs described in the following sections have an enhanced counterpart of the form *GattClient\_Enhanced\[procedure\]*. A *bearer id* parameter was added to specify on which bearer the transaction should take place. A value of *0* for the bearer id identifies the Unenhanced ATT bearer. Values higher than *0* are used to identify the Enhanced ATT bearer used for the ATT procedure.


```{include} ../topics/characteristic_value_read_procedure.md
:heading-offset: 3
```

```{include} ../topics/characteristic_read_by_uuid_procedure.md
:heading-offset: 3
```

```{include} ../topics/characteristic_read_multiple_procedure.md
:heading-offset: 3
```

```{include} ../topics/characteristic_write_procedure.md
:heading-offset: 3
```

**Parent topic:**[Client APIs](../topics/client_apis.md)

