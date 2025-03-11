# Creating a GATT database dynamically

To define a GATT Database at runtime, the *gGattDbDynamic\_d* macro must be defined in *app\_preinclude.h* with the value equal to 1.

Then, the application must use the APIs provided by the *gatt\_db\_dynamic.h* interface to add and remove Services and Characteristics as needed.

See [Creating static GATT database](creating_static_gatt_database.md) for a detailed description of Service and Characteristic parameters.


```{include} ../topics/memory_considerations.md
:heading-offset: 2
```

```{include} ../topics/initialization_and_release.md
:heading-offset: 2
```

```{include} ../topics/adding_services.md
:heading-offset: 2
```

```{include} ../topics/adding_characteristics_and_descriptors.md
:heading-offset: 2
```

```{include} ../topics/removing_services_and_characteristics.md
:heading-offset: 2
```

**Parent topic:**[Creating GATT database](../topics/creating_gatt_database.md)

