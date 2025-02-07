# FSCI handlers \(GAP, GATT, and GATTDB\) registration

For receiving messages from all the Bluetooth Low Energy Host Stack serial interfacing layers \(GAP, GATT, and GATTDB\), a function handler must be registered in FSCI for each layer:

```
fsciBleRegister(0);
```

**Parent topic:**[Serial manager and FSCI configuration](../topics/serial_manager_and_fsci_configuration.md)

