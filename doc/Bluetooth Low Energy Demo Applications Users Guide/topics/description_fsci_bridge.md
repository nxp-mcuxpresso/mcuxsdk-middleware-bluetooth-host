# Description

The FSCI Bridge demo application is a demonstration of the Extended NBU architectural concept, where the Bluetooth LE Host Stack and the Link Layer both run on the NBU core, while the user-facing application runs on the Application core. The communication between the application and the Host is done via the FSCI protocol running on the inter-core RPMSG transport

The FSCI Bridge runs on the Application core and it is a simple application whose purpose is to pass FSCI commands received over the serial interface to the NCP FSCI Black Box running on the NBU core. Effectively, the functionality is identical to the regular Bluetooth FSCI Black Box application.

For FSCI protocol details, command/event definitions, and Python scripting examples, see the [Bluetooth Low Energy Host Stack FSCI Application Programming](../../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming/Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md) guide.

**Parent topic:**[FSCI Bridge](../topics/fsci_bridge.md)

