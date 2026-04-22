# Running the A2B scenario

The A2B feature allows for the secure transfer of Bluetooth Low Energy security keys information between the Car Anchor devices. The feature is currently supported on the following platforms:

-   KW45B41Z-EVK with a lifecycle state of OEM-Open or higher, this feature is provided by the EdgeLock Secure Enclave.
-   KW47-EVK with a lifecycle state of OEM-Open or higher, this feature is provided by th EdgeLock Secure Enclave.
-   FRDM-KW43 with a lifecycle state of OEM-Open or higher, this feature is provided by the EdgeLock Secure Enclave.

The Car Anchor demo application makes use of the A2B feature to securely synchronize the local IRK and the Bonding Data (LTK and peer IRK) with another Car Anchor.

**Prerequisites**:

-   Three boards (two act as Car Anchors, one as a Device). The demo is currently limited to a maximum of two Car Anchors.
-   The two Car Anchors must be connected via a serial interface as described in the section above.
-   Advanced Secure Mode must be enabled. The following macros must be defined and set to `1` in `app_preinclude.h` for the ***digital_key_car_anchor*** project:
    -   *`gAppSecureMode_d`*
    -   *`gA2BEnabled_d`*
-   One of the Car Anchors must be configured with the *`gA2BInitiator_d`* macro set to `1`, this is called Car Anchor A, and the other must be configured with the *`gA2BInitiator_d`* macro set to `0`, called Car Anchor B. Car Anchor A triggers the EdgeLock-to-EdgeLock \(E2E\) key derivation and local IRK synchronization. Therefore, it should be started up after Car Anchor B.

**Demo steps**:

-   Start Car Anchor B, then Car Anchor A. At initialization the E2E key is derived and the local IRK of Car Anchor A is sent as a secure blob to Car Anchor B in order for both Car Anchors to have the same local IRK as shown in the [Figure 1](../images/AncA_key_deriv.png) and [Figure 2](../images/AncB_key_deriv.png).

**Car Anchor A starts up, derives the E2E key and syncs with Car Anchor B**

![Figure 1](../images/AncA_key_deriv.png "Car Anchor A starts up, derives the E2E key and syncs with Car Anchor B")

**Car Anchor B starts up and syncs with Car Anchor B**

![Figure 2](../images/AncB_key_deriv.png "Car Anchor B starts up and syncs with Car Anchor B")


-   Trigger the Owner Pairing scenario by sending the "`sd`" command on the Device and "`sd op`" on Car Anchor A. Once the bond is created, the Bonding Data containing secured blobs for the LTK and peer IRK is sent to Car Anchor B. The Car Anchors display the Bonding Data in shell with the LTK in blob form \(the LTK is never available in plain text\) and the peer IRK in plain text. Now both Car Anchors have the same Bonding Data. Refer [Figure 3](../images/AncA_key_deriv._oppng.png) and [Figure 4](../images/../images/AncB_key_deriv_op.png).

**Car Anchor A pairs with Device**

![Figure 3](../images/AncA_key_deriv._oppng.png "Car Anchor A pairs with Device")

**Car Anchor B has received the Bonding Data via the serial interface**

![Figure 4](../images/../images/AncB_key_deriv_op.png "Car Anchor B has received the Bonding Data via the serial interface")

-   Disconnect Car Anchor A by sending the "`dcnt`" command.
-   Once Car Anchor A is disconnected, send "`sd`" on the Device and "`sd pe`" on Car Anchor B. Car Anchor B is now able to perform a Passive Entry connection, without pairing as shown in [Figure 5](../images/AncB_key_deriv_pe.png).

**Car Anchor B performs Passive Entry**

![Figure 5](../images/AncB_key_deriv_pe.png "Car Anchor B performs Passive Entry")


