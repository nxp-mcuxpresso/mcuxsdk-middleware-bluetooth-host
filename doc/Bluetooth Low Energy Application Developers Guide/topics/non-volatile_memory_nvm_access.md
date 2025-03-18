# Non-Volatile Memory \(NVM\) access

The Bluetooth LE Host Stack implements an internal module responsible for managing device information. This module relies on accessing a Non-Volatile Memory module for storing and loading bonded devices data.

The application developers determine the NVM access mechanism through the definition of three functions and one variable. The functions must first pre-process the information and then perform standard NVM operations \(erase, write, read\). The declarations are as follows:

```
bleResult_t App_NvmErase
(
    uint8_t mEntryIdx
);
bleResult_t App_NvmRead
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint8_t  mDescriptorIndex
);
bleResult_t App_NvmWrite
(
    uint8_t  mEntryIdx,
    void*    pBondHeader,
    void*    pBondDataDynamic,
    void*    pBondDataStatic,
    void*    pBondDataLegacy,
    void*    pBondDataDeviceInfo,
    void*    pBondDataDescriptor,
    uint8_t  mDescriptorIndex
);
```

The device information is divided into several components to ensure that even software wear leveling mechanisms can be used optimally. The components sizes are fixed \(defined in *ble\_constants.h*\) and have the following meaning:

|API pointer to bond component|Component size \(*ble\_constants.h*\)|Description|
|-----------------------------|-------------------------------------|-----------|
|**pBondHeader**: points to a bleBondIdentityHeaderBlob\_t element|gBleBondIdentityHeaderSize\_c|Bonding information which is sufficient to identify a bonded device.|
|**pBondDataDynamic:** points to a bleBondDataDynamicBlob\_t element|gBleBondDataDynamicSize\_c|Bonding information that might change frequently.|
|**pBondDataStatic**: points to a bleBondDataStaticBlob\_t element|gBleBondDataStaticSize\_c|Bonding information that is unlikely to change frequently.|
|**pBondDataLegacy**: points to a bleBondDataLegacyBlob\_t element|gBleBondDataLegacySize\_c|Stores legacy pairing and Connection Signature Resolving Key \(CSRK\) bond information.|
|**pBondDataDeviceInfo**: points to a bleBondDataDeviceInfoBlob\_t element|gBleBondDataDeviceInfoSize\_c|Additional bonding information that can be accessed using the host stack API.|
|**pBondDataDescriptor**: points to a bleBondDataDescriptorBlob\_t element|gBleBondDataDescriptorSize\_c|Bonding information used to store one Client Characteristic Configuration Descriptor \(CCCD\).|

The Bluetooth LE Host Stack handles the format of the bonding information. Therefore, application developers need not to take care of this aspect.

Each bonding data slot must contain one bonding header blob, one dynamic data blob, one static data blob, one data legacy blob, one device information blob, and an array of descriptor blobs equal to *gcGapMaximumSavedCccds\_c.*

**Note:** *The application must define the *gcGapMaximumSavedCccds\_c.* macro according to its requirement. The default value can be found in the `ble_constants.h` file*.\)

A slot is uniquely identified by the *mEntryIdx* parameter.

A descriptor is uniquely identified by the pair *mEntryIdx - mDescriptorIndex*.

If one or more pointers passed as parameters are NULL, the read from or write to the corresponding blob of the bonding slot must be ignored. The erase function must clear the entire bonding data slot specified by the entry index.

## Note: 

When Advanced Secure Mode is chosen \( `gAppSecureMode_d` is defined as `1` in `app_preinclude.h`\), two additional application NVM functions are defined to handle local keys encrypted blob storage. Their declaration is:

```
bleResult_t App_NvmWriteLocalKeys
(
    uint8_t mEntryIdx,
    void* pLocalKey
)
bleResult_t App_NvmReadLocalKeys
(
    uint8_t mEntryIdx,
    void* pLocalKey
)
```

The functions write/read a structure of type `bleLocalKeysBlob_t` into/from NVM using a dedicated data set. The parameter `mEntryIdx` can be `0` \(local IRK is handled\) or `1` \(local CSRK is handled\).

The format of the local keys blob nor about the generation and storage of the local keys is automatically handled in **BLE Connection Manager** \(*BleConnManager\_GenericEvent*\). Therefore the application developer need not manage this aspect.

The current implementation of the aforementioned functions uses either the framework NVM module or a RAM buffer. Additional details about the NVM configuration and functionality can be found in the *Connectivity Framework Reference Manual*. See [References](reference_documentation.md).

To enable the NVM mechanism, ensure the following points:

-   `gAppUseNvm_d` \(in *app\_preinclude.h*\) is set to `1` and
-   `gUseNVMLink_d` is set to `1` in the linker options of the toolchain.

**Note:**

-   If `gAppUseNvm_d` is set to `0`, then all bonding data is stored in the RAM and is accessible until reset or power cycle.
-   If `gAppUseNvm_d` is set to `1`, the default NVM module configurations are applied in the *app\_preinclude.h* file.

**Parent topic:**[Prerequisites](../topics/prerequisites.md)

