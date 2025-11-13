# Digital Key Device application

The Digital Key Device demo application emulates a CCC Digital Key-enabled smartphone. It acts as a Bluetooth Low Energy central device, scanning for advertising coming from a Digital Key Car Anchor.

The figure below shows an example deployment of CCC Digital Key R3.

**Anchor communicating with a smartphone (CCC) and a key fob (non-CCC)**

![](../images/cccfigure.png "Anchor communicating with a smartphone (CCC) and a key fob (non-CCC)")

Inside the car, there are two Bluetooth Low Energy Anchors connected to each other through Controller Area Network (CAN) bus. These Anchors are used to exchange security data and other information. An Anchor can connect to a CCC Digital Key R3-enabled smartphone and to a non-CCC key fob and can perform both advertising and scanning.

For the purposes of localization, the Digital Key Device acts as either a Channel Sounding Initiator or Reflector and as a Client/Server in the context of the BTCS. The BTCS Server stores the local measurement data and provides it to the BTCS Client.

**Parent topic:**[CCC Digital Key applications overview](../topics/ccc_digital_key_applications_overview.md)

