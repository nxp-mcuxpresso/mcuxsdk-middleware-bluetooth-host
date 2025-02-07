# GATT database application interface

For over-the-air packet exchanges between a Client and a Server, the GATT Server module automatically retrieves data from the GATT database and responds to all ATT Requests from the peer Client, provided it passes the security checks. This ensures that the Server application does not have to perform any kind of searches over the database.

However, the application must have access to the database to write meaningful data into its characteristics. For example, a temperature sensor must periodically write the temperature, which is measured by an external thermometer, into the Temperature Characteristic.

For these kinds of situations, a few APIs are provided in the *gatt\_db\_app\_interface.h* file.

**Note:** All functions provided by this interface are executed synchronously. The result of the operation is saved in the return value and it generates no event.


```{include} ../topics/writing_and_reading_attributes.md
:heading-offset: 1
```

```{include} ../topics/finding_attribute_handles.md
:heading-offset: 1
```

