# Hybrid \(Dual-mode\) Bluetooth Low Energy and Generic FSK

The Hybrid \(Dual-mode\) Bluetooth Low Energy and Generic FSK application demonstrates Generic FSK transmission/reception and Bluetooth advertising/scanning/multiple connections coexistence.

The Bluetooth LE part of this demo implements a modified version of the Wireless UART demo application, capable of multiple Bluetooth LE connections.

Based on the Hardware link-layer implementation, the Bluetooth Low Energy has a higher priority than the Generic FSK protocol and as the effect, the Generic FSK communication is executed during the Idle states \(inactive periods\) of the Bluetooth LE. The coexistence of the two protocols is handled internally at the Controller level.

The Bluetooth LE part of the application behaves at first as a GAP central node. It enters GAP Limited Discovery Procedure and searches for other Wireless UART devices to connect. To change the device role to GAP peripheral, use the **ROLESW** button. The device enters GAP General Discoverable Mode and waits for a GAP central node to connect.

The Generic FSK part of the application can either enter in the receive state by double clicking the **SCANSW** button or it can start the periodic transmit by long pressing the **ROLESW** button. It cannot enter in both states at the same time.

The Generic FSK has lower priority than the Bluetooth LE. Therefore, any ongoing Generic FSK receive is paused by the Controller when Bluetooth LE activity is ongoing. The reception is automatically resumed by the Controller when there is no Bluetooth LE activity.

When using Generic FSK, packet loss should be expected. The Generic FSK communication does not offer an acknowledgement mechanism to guarantee success for packet transmission, therefore this communication method does not offer reliability. If the user wants to improve reliability, packet repetition may be used, or the user may implement a custom protocol at application level.

The first Generic FSK transmit command is buffered if there is continuous Bluetooth LE activity \(for example, for continuous Bluetooth LE scanning\). Any succeeding Generic FSK transmit command indicates failure in the command line interface, if the initial buffered transmit command was not sent yet.

This section describes the implemented profiles and services, user interactions, and testing methods for the Hybrid \(Dual-Mode\) Bluetooth Low Energy and Generic FSK application.


```{include} ../topics/implemented_profile_and_services_FSK.md
:heading-offset: 2
```

```{include} ../topics/supported_platforms.md
:heading-offset: 2
```

```{include} ../topics/user_interface_FSK.md
:heading-offset: 2
```

```{include} ../topics/usage.md
:heading-offset: 2
```

```{include} ../topics/customization.md
:heading-offset: 2
```

**Parent topic:**[Bluetooth LE stack and demo applications](../topics/bluetooth_le_stack_and_demo_applications.md)

