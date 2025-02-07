# Server APIs

Once the GATT Database has been created and the required security settings have been registered with *Gap\_RegisterDeviceSecurityRequirements*, all ATT Requests and Commands and attribute access security checks are handled internally by the GATT Server module.

Besides this automatic functionality, the application may use GATT Server APIs to send Notifications and Indication and, optionally, to intercept Clients’ attempts to write certain attributes.


```{include} ../topics/server_callback.md
:heading-offset: 2
```

```{include} ../topics/sending_notifications_and_indications.md
:heading-offset: 2
```

```{include} ../topics/attribute_write_notifications.md
:heading-offset: 2
```

**Parent topic:**[Generic Attribute Profile \(GATT\) Layer](../topics/generic_attribute_profile_gatt_layer.md)

