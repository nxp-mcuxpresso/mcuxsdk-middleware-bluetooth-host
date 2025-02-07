# Creating a Bluetooth LE application when the Host Stack runs on another processor

This section describes how to create a Bluetooth Low Energy application \(host\), when the Bluetooth Low Energy Host Stack is running on another processor \(blackbox\). The section alsp provides sample code to explain how to achieve this.

The supported serial interfaces between the two chips \(application and the Bluetooth Low Energy Host Stack\) are UART, SPI, or USB.

Typical applications employing Bluetooth LE Host Stack blackboxes are host systems such as a PC tool or an embedded system that has an application implementation. This chapter describes an embedded application.

For more information, refer to *Bluetooth Low Energy Host Stack FSCI Reference Manual*. This document provides explicit information on exercising the Bluetooth Low Energy Host Stack functionality through a serial communication interface to a host system.


```{include} ../topics/serial_manager_and_fsci_configuration.md
:heading-offset: 1
```

```{include} ../topics/bluetooth_low_energy_host_stack_initialization.md
:heading-offset: 1
```

```{include} ../topics/gatt_database_configuration.md
:heading-offset: 1
```

```{include} ../topics/fsci_host_layer.md
:heading-offset: 1
```

