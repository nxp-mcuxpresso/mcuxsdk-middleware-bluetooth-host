# Wireless UART Host

This section describes the implemented profiles and services, user interactions, and testing methods for the Wireless UART Host application.

The Wireless UART Host application is a demonstration of the Extended NBU architectural concept, where the Bluetooth LE Host Stack and the Link Layer both run on the NBU core, while the user-facing application runs on the Application core. The communication between the application and the Host is done via the FSCI protocol running on the inter-core RPMSG transport.

The Wireless UART Host application must be flashed together with the NCP FSCI Blackbox application.


```{include} ../topics/implemented_profile_and_services_w_uart_host.md
:heading-offset: 2
```

```{include} ../topics/supported_platforms_w_uart_host.md
:heading-offset: 2
```

```{include} ../topics/user_interface_w_uart_host.md
:heading-offset: 2
```

[Switches and pins](../topics/switches_and_pins.md#wireless-uart-host)

```{include} ../topics/usage_w_uart_host.md
:heading-offset: 2
```

**Parent topic:**[Bluetooth LE stack and demo applications](../topics/bluetooth_le_stack_and_demo_applications.md)

