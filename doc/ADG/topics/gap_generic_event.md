# GAP generic event

The GAP Generic Event is triggered by the Bluetooth LE Host Stack and sent to the application via the generic callback. Before any application-specific interactions, the Connection Manager callback is called to handle common application events, such as device address storage.

```
**void** **BleApp\_GenericCallback** ( gapGenericEvent_t * pGenericEvent)
{
    /* Call Bluetooth Low Energy Conn Manager */
    BleConnManager_GenericEvent(pGenericEvent);
    **switch** (pGenericEvent-> eventType )
    {
        ...
    }
}
```

In the **BleConnManager\_GenericEvent** function, local keys are generated.

-   The local LTK, IRK, and CSRK as well as the EDIV and RAND are obtained hashing over the board’s UID and stored in RAM as plain-text every time the **gInitializationComplete\_c** event is received.
-   In **Advanced Secure** mode, local IRK and CSRK are generated using the EdgeLock Secure Enclave and stored into a dedicated NVM data set as ELKE blobs \(40 bytes blob encrypted using unique die key\) on the first `gInitializationComplete_c` event received.

The NBU Decryption key for IRK is generated and distributed to the NBU over the private key bus. The EIRK blob \(16 bytes blob which can be decrypted only by NBU hardware using NBU Decryption key for IRK\) is generated from the IRK ELKE blob and stored in the RAM to be used for controller privacy on every `gInitializationComplete_c` event received. For the host privacy, the ELKE IRK blob is used instead. For details, refer to the section "[Advanced security capabilities](Advanced_security_capabilities.md)".

**Parent topic:**[Bluetooth LE Connection Manager](../topics/bluetooth_le_connection_manager.md)

