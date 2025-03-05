# Data management of bonded devices

The Host handles the management of the bonding data without requiring application intervention. The application must provide the NVM write, read, and erase functions presented in [Non-Volatile Memory \(NVM\) access](non-volatile_memory_nvm_access.md). The Host creates bonds if bonding is required after the pairing.

The bonded data structure is presented below, together with the GAP APIs that access it, for most APIs require a connection to be established with the device in the bonded list, the others can be accessed any time using the NVM index.

1.  Bond Header – identity address and address type that uniquely identify a device together with the IRK and privacy mode.
    -   `Gap_GetBondedDevicesIdentityInformation` – for all bonds
2.  Bond Data Dynamic - security counters for signed operations – managed by the stack
3.  Bond Data Static – LTK, CSRK, Rand, EDIV, security information for read and write authorizations
    -   `Gap_SaveKeys`– NVM index
    -   `Gap_LoadKeys`– NVM index
    -   `Gap_LoadEncryptionInformation` - deviceId
    -   `Gap_Authorize – deviceId` - GATT Server only
4.  Bond Data Legacy - Legacy pair information and CSRK
    -   `Gap_LoadEncryptionInformation` - deviceId
5.  Bond Data Device Info - custom peer information \(service discovery data\) and device name
    -   `Gap_SaveCustomPeerInformation` - deviceId
    -   `Gap_LoadCustomPeerInformation` - deviceId
    -   `Gap_SaveDeviceName` - deviceId
    -   `Gap_GetBondedDeviceName` – NVM index
6.  Bond Data Descriptor List - configuration of indications and notifications for CCCD handles – GATT Server only
    -   `Gap_CheckNotificationStatus` - deviceId
    -   `Gap_CheckIndicationStatus`- deviceId

However, there may be some cases when an application wants to manage this data to read data from a bonded device created by the Host, create a bond obtained out-of-band or update an existing bond. For this use case, two GAP APIs and a GAP event have been added.

1.  **Load the Keys of a bonded device**.

    The user can call the following function to read the keys exchanged during pairing and stored by the Bluetooth LE Host Stack in the bond area when the pairing is complete.

    The application is informed of the NVM index through the `gBondCreatedEvent_c` sent by the stack immediately after the bond creation. The application is responsible for passing the memory in the `pOutKeys OUT` parameter to fill in the keys, if any of the keys are set to NULL, the stack does not fill that information. The `pOutKeyFlags OUT` parameter indicates to the application which of the keys were stored by the stack as not all of them may have been distributed during pairing.

    The `pOutLeSc` indicates if Bluetooth LE 4.2 LE Secure Connections Pairing was used, while the `pOutAuth` indicates if the peer device is authenticated for MITM protection. All these OUT parameters are recommended to be retrieved from the bond and added if later passed as input parameters for the save keys API.

    This function executes synchronously.

    ```
    bleResult_t Gap_LoadKeys
    (
        uint8_t            nvmIndex,
        gapSmpKeys_t*      pOutKeys,
        gapSmpKeyFlags_t*  pOutKeyFlags,
        bool_t*            pOutLeSc,
        bool_t*            pOutAuth);
    );
    ```

    The `gapSmpKeys_t` is the structure used during the key distribution phase, as well as in the `gConnEvtKeysReceived_c` event and is as follows. The difference is that the Bluetooth LE device address cannot be set to NULL neither when loading a bond or when creating one as it identifies the bonded device together with the NVM index.
    <br>  
    **Table 3. 'gapSmpKeys_t' structure**
                        
                            
| Event Data  | Data type         | Data Description                                                                                                                                                      |
| ----------- | ----------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| cLtkSize    | uint8_t           | Encryption Key Size filled by the stack. If aLtk is NULL, this is ignored.<br>In Advanced Secure Mode, this should be the size of the LTK encrypted blob of 40 bytes. |
| aLtk        | uint8_t\*         | Long Term (Encryption) Key or LTK encrypted blob if Advanced Secure Mode is enabled. NULL if LTK is<br>not distributed, else size is given by cLtkSize                |
| aIrk        | uint8_t\*         | Identity Resolving Key. NULL if aIrk is not distributed.                                                                                                              |
| aCsrk       | uint8_t\*         | Connection Signature Resolving Key. NULL if aCsrk is not distributed.                                                                                                 |
| cRandSize   | uint8_t           | Size of RAND filled by the stack; usually equal to gcSmpMaxRandSize_c. If<br>aLtk is NULL, this is ignored.                                                           |
| aRand       | uint8_t\*         | RAND value used to identify the LTK. If aLtk is NULL, this is ignored.                                                                                                |
| ediv        | uint16_t          | EDIV value used to identify the LTK. If aLtk is NULL, this is ignored.                                                                                                |
| addressType | bleAddress Type_t | Public or Random address.                                                                                                                                             |
| aAddress    | uint8_t\*         | Device Address. It cannot be NULL.                    

The structure for the GAP SMP Key Flags is the following:

   **Table 4. GAP SMP Key Flags**
  | Flag Type | Description                                      |
  | --------- | ------------------------------------------------ |
  | gNoKeys_c | No key is available.                             |
  | gLtk_c    | Long-Term Key is available.                      |
  | gIrk_c    | Identity Resolving Key is available.             |
  | gCsrk_c   | Connection Signature Resolving Key is available. |

2.  Save the Keys to create a bond or update an existing bonded device.

    The user can call the following function to create a bond on a device based on information obtained Out of Band. For instance, one can use the output of `Gap_LoadKeys` from the previous section. This can be useful in transferring a bond created by the stack after a pairing procedure or if the application wants to manipulate bonding data. The behavior of the stack remains the same, if the bonding is required after a pairing, the stack stores the bonding information if possible. In this case, the NVM index is passed to the application through `gBondCreatedEvent_c`.

    This function executes asynchronously, as the stack can create a bond during the execution. The application should listen for the previous mentioned event `gBondCreatedEvent_c`. The result of the function call is passed synchronously. However, if an asynchronous error has occurred during the actual save, it is passed to the application through the `gInternalError_c` event with a `gSaveKeys_c` error source.

    The stack creates a bond if the NVM index is free or update the keys from an NVM index if it stores a valid entry.

    The address from the GAP SMP Keys structure must not be NULL. If other members of the structure are NULL, they are ignored.

    LE SC flag indicates if Bluetooth LE 4.2 Secure Connections was used during pairing and Auth specifies if the peer is authenticated for MITM protection.

    ```
    bleResult_t Gap_SaveKeys
    (
        uint8_t         nvmIndex,
        gapSmpKeys_t*   pKeys,
        bool_t          leSc,
        bool_t          auth
    );
    ```

3.  Bond created event.

    A GAP event is added to the Bluetooth LE Generic Callback to inform the application of the NVM index whenever the stack creates a bond or when a `Gap_SaveKeys` request succeeds. The event is also generated if the NVM index was a valid occupied entry and only some of the keys in the bonded information have been updated.

    The NVM index is then used in the GAP APIs to save or load information from the bond.

    |Event Data|Data type|Data Description|
    |----------|---------|----------------|
    |nvmIndex|uint8\_t|NVM index for the new created bond|
    |addressType|bleAddressType\_t|Public or Random \(static\) address of the bond|
    |address|bleDeviceAddress\_t|Address of the bond|



```{include} ../topics/application_removal_of_bonded_devices_data.md
:heading-offset: 2
```

**Parent topic:**[Generic Access Profile \(GAP\) Layer](../topics/generic_access_profile_gap_layer.md)

