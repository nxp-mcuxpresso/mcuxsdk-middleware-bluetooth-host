# Bluetooth Low Energy Host Stack initialization

The Bluetooth Low Energy Host Stack must be initialized when platform setup is complete and all RTOS tasks have been started. This initialization is done by restarting the blackbox using a FSCI CPU Reset Request command. This is performed automatically by the ***Ble\_Initialize****\(App\_GenericCallback\)*function.

```
/* Send FSCI CPU reset command to BlackBox */
FSCI_transmitPayload(gFSCI_ReqOpcodeGroup_c, mFsciMsgResetCPUReq_c, NULL, 0, fsciInterface);
```

The completion of the Bluetooth Low Energy Host Stack initialization is signaled by the reception of the *GAP-GenericEventInitializationComplete.Indication* event \(over the serial communication interface, in FSCI\). The *Bluetooth Low Energy-HostInitialize.Request* command is not required to be sent to the blackbox \(the entire initialization is performed by the blackbox, when it resets\).

**Parent topic:**[Creating a Bluetooth LE application when the Host Stack runs on another processor](../topics/creating_a_bluetooth_low_energy_application_when_t.md)

