# GAP configuration

The GAP Central or Peripheral Configuration is used to create common configurations \(such as setting the public address, registering the security requirements, adding the addresses of bonded devices in the Controller Filter Accept List\), which can be customized by the application afterwards. It is called inside the *BluetoothLEHost\_Initialized callback* function, before any application-specific configuration, as shown in the example code below.

```
static void BluetoothLEHost_Initialized()
{
    /* Set common GAP configuration */
    BleConnManager_GapCommonConfig();
    ...
}
```

**Parent topic:**[Bluetooth LE Connection Manager](../topics/bluetooth_le_connection_manager.md)

