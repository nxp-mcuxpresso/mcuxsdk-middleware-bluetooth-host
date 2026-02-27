(examples__wireless_examples__bluetooth__digital_key_device_docs)=
# Digital Key Device

The Digital Key Device demo application emulates a CCC Digital Key-enabled smartphone. It acts as a Bluetooth Low Energy central device, scanning for advertising coming from a Digital Key Car Anchor.

The [Figure 1](../images/cccfigure.png) shows an example deployment of CCC Digital Key R3. Inside the car, there are two Bluetooth Low Energy anchors connected to each other. The Controller Area Network \(CAN\) bus connects these anchors and is used to exchange security data and other information. Any anchor can perform both advertising and scanning and it is able to connect to a CCC Digital Key R3-enabled smartphone as well as to a non-CCC key fob.

**Anchor communicating with a smartphone (CCC) and a key fob (non-CCC)**

![](../images/cccfigure.png "Anchor communicating with a smartphone (CCC) and a key fob (non-CCC)")

**Parent topic:**[CCC Digital Key applications overview](../topics/ccc_digital_key_applications_overview.md)

