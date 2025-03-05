# Introduction

Starting with Bluetooth 4.2, Privacy can be enabled either in the Host or in the Controller:

-   **Host Privacy** consists of two use cases that are described in detail in the following sections. These are:
    -   Random address generation - Periodically regenerating a random address \(Resolvable or Non-Resolvable Private Address\) inside the Host and then applying it into the Controller.
    -   Random address resolution - Trying to resolve incoming RPAs using the IRKs stored in the bonded devices list. The address resolution is performed when a connection is established with a device or for the autoconnect scan. The advertising packets that have an RPA are not resolved automatically due to the high MCU processing that is required.

        The random address resolution is performed by default by the Host whenever the Controller is not able to resolve an RPA. The Host performs random address generation only when Host Privacy is requested to be enabled. During random address generation, the advertising and scan operations, if active, are stopped and restarted. If errors occur during this process and the scan or advertising cannot be started, the application is notified through the corresponding event \(`gAdvertisingStateChanged_c`, `gExtAdvertisingStateChanged_c` or `gScanStateChanged_c`\)
<br>
-   **Controller Privacy**, introduced by Bluetooth 4.2, consists of writing the local IRK in the Controller, together with all known peer IRKs, and letting the Controller perform hardware, fully automatic RPA generation and resolution. The Controller uses a Resolving List to store these entries. The size of the list is platform dependent and determined by `gMaxResolvingListSize_c`. For RPA resolution, the entries that do not fit in this list are processed by the Host to be resolved using the IRKs from Bonded Devices list.

Either Host Privacy or Controller Privacy can be enabled at any time. Trying to enable one while the other is in progress generates a *gBleInvalidState\_c* error. The same error is returned when trying to enable the same privacy type twice, or when trying to disable privacy when it is not enabled.

The recommended way of using Privacy is the Controller Privacy. However, enabling Controller Privacy requires at least a pair of local IRK and peer IRK, so this can only be enabled only after a pairing is performed with a peer and the IRKs are exchanged during the Key Distribution phase. When a device starts, if Privacy is required, the workflow is the following:

1.  Enable Host Privacy using the local IRK.
2.  Connect to a peer and perform pairing and bonding to exchange IRKs.
3.  Disable Host Privacy.
4.  Enable Controller Privacy using the local IRK and the peer IRK and peer identity address.

After enabling Host Privacy or Controller Privacy, the application must wait for the `gHostPrivacyStateChanged_c` or `gControllerPrivacyStateChanged_c` generic event and verify that privacy has been successfully enabled. Only then it is safe to proceed with setting advertising parameters \(via the `Gap_SetAdvertisingParameters` or `Gap_SetExtAdvertisingParameters` APIs\) or starting scanning \(via the `Gap_StartScanning` API\). Failure to do so could result in unwanted behavior, such as the device advertising or scanning with a public address.


```{include} ../topics/resolvable_private_addresses.md
:heading-offset: 3
```

```{include} ../topics/non-resolvable_private_addresses.md
:heading-offset: 3
```

```{include} ../topics/multiple_identity_resolving_keys.md
:heading-offset: 3
```

**Parent topic:**[Privacy feature](../topics/privacy_feature.md)

