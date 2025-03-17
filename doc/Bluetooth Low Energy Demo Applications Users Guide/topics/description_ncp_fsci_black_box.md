# Description

The NCP FSCI Black Box application is a demonstration of the Extended NBU architectural concept, where the Bluetooth LE Host Stack and the Link Layer both run on the NBU core, while the user-facing application runs on the Application core. The communication between the application and the Host is done via the FSCI protocol running on the inter-core RPMSG transport.

The NCP FSCI Black Box application runs on the NBU core and provides access to the Bluetooth LE Host Stack through FSCI to the application running on the Application core. It can be paired with the [FSCI Bridge](../topics/fsci_bridge.md) application to obtain a functionality that is identical to the regular Bluetooth FSCI Black Box application, or with the [Wireless UART Host](../topics/wireless_uart_host.md) application to obtain a functionality that is identical to the regular Wireless UART application.

**Parent topic:**[NCP FSCI Black Box](../topics/ncp_fsci_black_box.md)

