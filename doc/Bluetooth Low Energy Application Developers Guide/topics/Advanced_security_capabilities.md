# Advanced Secure Mode \(kw45\_k32w\)

This section describes the advanced security capabilities of the Bluetooth LE Host Stack which are available on the KW45/K32W1 platform via the EdgeLock Secure Enclave \(ELKE\).

The security capabilities are enabled at application, Host and Controller level by setting Advanced Secure Mode to active. To do this, the user must set the *gAppSecureMode\_d* macro to *1* in the project’s *app\_preinclude.h* file. This macro is defined by default as *0* in *app\_preinclude\_common.h*:

```
#if (gAppSecureMode_d == 1U)
#define gSecLibSssUseEncryptedKeys_d     (1U)
#define gHostSecureMode_d                (1U)
#else
#define gHostSecureMode_d                (0U)
#endif
```

At application level, when Advanced Secure Mode is enabled, the security mode and level for pairing is automatically enforced as Mode1 Level 4, ensuring LE Secure Connections pairing. Legacy pairing is not supported in this mode.

When enabled, the main benefit of Advanced Secure Mode is the secured storage and handling of Bluetooth LE security keys. The EdgeLock Secure Enclave is capable of generating, importing, and exporting security keys as plain text or as encrypted blobs. All encrypted blobs are created by the EdgeLock Secure Enclave using a die unique key, which makes them impossible to decrypt by devices other than the one that created them. The Bluetooth LE security keys are managed in Advanced Secure Mode as follows:

-   IRK
    -   The peer IRKs received after pairing and bonding are no longer stored into NVM as plaintext, but as encrypted blobs 40 bytes in length \(or ELKE blobs\). They can still be retrieved and converted to plaintext.
    -   The local IRK is no longer generated using the default method of hashing over the board’s UID at every startup. It is instead generated once using the EdgeLock Secure Enclave and stored into a new NVM dataset as an ELKE blob.
    -   Local and peer IRKs are no longer transmitted through HCI in plaintext but as EIRK \(Encrypted IRK\) blobs, 16 bytes in length, which can be decrypted by the Controller.
-   LTK
    -   The LTK is no longer stored into NVM as plaintext, but as an ELKE blob. Furthermore, the plaintext of the LTK is never available to the Host/application. Generating the LTK via the ECDH process and generating the Session Key for individual connections is done via the EdgeLock Secure Enclave and custom vendor HCI messages which are transparent to the application.
-   CSRK
    -   The local CSRK is no longer generated using the default method of hashing over the board’s UID at every startup. It is instead generated once using the EdgeLock Secure Enclave and stored into a new NVM dataset as an ELKE blob.

At startup, Advanced Secure Mode for the Controller is enabled dynamically by calling:

```
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) 
    (void) PLATFORM_EnableBleSecureKeyManagement(); 
#endif
```

This call can be found in BluetoothLEHost\_Init, as part of the initialization sequence.

**Parent topic:**[Application Structure](../topics/application_structure.md)

