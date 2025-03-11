# Bluetooth LE OTAP protocol

The protocol consists of a set of commands \(messages\) which allow the OTAP Client to request or be notified about the available images on an OTAP Server and to request the transfer of parts of images from the OTAP Server.

All commands with the exception of the image data transfer commands are exchanged through the OTAP Control Point characteristic of the OTAP Service. The data transfer commands are sent only from the OTAP Server to the OTAP Client either via the OTAP Data characteristic of the OTAP Service or via a dedicated Credit Based Channel assigned to a L2CAP PSM.


```{include} ../topics/protocol_design_considerations.md
:heading-offset: 2
```

```{include} ../topics/bluetooth_low_energy_otap_commands.md
:heading-offset: 2
```

```{include} ../topics/otap_clientserver_interactions.md
:heading-offset: 2
```

**Parent topic:**[Over the Air Programming \(OTAP\)](../topics/over_the_air_programming_otap.md)

