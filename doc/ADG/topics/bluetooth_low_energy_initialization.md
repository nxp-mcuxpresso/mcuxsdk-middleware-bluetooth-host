# Bluetooth Low Energy initialization

The *ble\_init.h* and *ble\_init.c* files contain the declaration and the implementation of the following function:

```
bleResult_t **Ble\_Initialize**
(
    gapGenericCallback_t gapGenericCallback
)
{
#if defined(gUseHciTransportDownward_d) && gUseHciTransportDownward_d
    /* HCI Transport Init */
    if (gHciSuccess_c != Hcit_Init(Ble_HciRecvFromIsr))
    {
        return gHciTransportError_c;
    }
#if defined(KW45B41Z83_SERIES) || \
    defined(KW45B41Z82_SERIES) || \
    defined(K32W1480_SERIES)
    /*
     * Set BD Address in Controller. Must be done after HCI init
     * and before Host init.
     */
      Ble_SetBDAddr();
#endif /* KW45B41Z83_SERIES */
    /* Check for available memory storage */
    if (!Ble_CheckMemoryStorage())
    {
        return gBleOutOfMemory_c;
    }
    /* Bluetooth Low Energy Host Tasks Init */
    if (KOSA_StatusSuccess != Ble_HostTaskInit())
    {
        return gBleOsError_c;
    }
    /* Bluetooth Low Energy Host Stack Init */
    return Ble_HostInitialize(gapGenericCallback, Hcit_SendPacket);
#elif defined(gUseHciTransportUpward_d) && gUseHciTransportUpward_d
#else /* gUseHciTransportUpward_d */
#endif /* gUseHciTransportUpward_d */
}
```

**Note:** This function should be used by your application because it correctly performs all the necessary Bluetooth Low Energy initialization.

Step-by-step analysis is provided below:

1.  First, the HCI interface is initialized by calling *Hcit\_Init*. This initializes communication between the Host and the Controller.
2.  After setting the BD address into the Controller \(Ble\_SetBDAddr\) and performing memory validation checks \(*Ble\_CheckMemoryStorage*\), the Host task is initialized by calling *Ble\_HostTaskInit*.
3.  Finally the *Ble\_HostInitialize* function initializes the Host with the transport packet transmit function used as the *hciHostToControllerInterface\_t parameter*.

**Parent topic:**[Application Structure](../topics/application_structure.md)

