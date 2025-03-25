# FSCI Messages XML


## Overview

The Framework Serial Communication Interface (FSCI) module supports interfacing the BLE Host Stack with a host or a PC tool (Test Tool for Connectivity Products) using a serial communication line. If the FSCI is used with the NXP Test Tool for Connectivity products, the tool can use an XML file containing a detailed meta-description for serial commands and events. This XML file is provided in the software packages that include this document.

**Bluetooth LE Host Stack interface set**

The Bluetooth LE Host Stack layers that offer access using FSCI are L2CAP, L2CAP CB, GATT, GATTDB, GAP, FSCI, and NVM. Each layer provides primitives that an upper layer (profile/application) uses to access services of that layer. This document includes a detailed list of the provided primitives.

**Basic requirements**

Following are the hardware requirements for using FSCI API RM:
- IAR Embedded Workbench for Arm architecture
- NXP development boards (to be loaded with the **ble_fscibb** application)
- Optional Bluetooth LE sniffer for testing
- PC tool (Test Tool for Connectivity Products) or another host processor capable of communicating through UART, USB, SPI, or I<sup>2</sup>C


## FSCI message description

This chapter describes the L2CAP, L2CAP CB, GATT, GATTDB, GAP, FSCI, and NVM commands (requests) and events (confirms and indications) in detail.

The `BLE.xml` document is found in the `ble_fsci_app` subfolder, and provides an XML representation of all these commands and events.
The `BLE_x.x.x.xml` document is found in the `..\tools\wireless\xml_fsci\` subfolder, and provides an XML representation of all these commands and events.

**How to interpret the XML**

The XML is structured in `<Commands>` and `<Events>` tags, the contents grouped by layer. Each group contains the description `<GroupDesc>` and multiple `<Cmd>` tags (as shown below).

```xml
<BLE>
  <Commands>
    <L2CAP>
      <GroupDesc>L2CAP Commands</GroupDesc>
      <Cmd>
        <CmdName>L2CAP-Init.Request</CmdName>
        <CmdDesc>L2CAP initialization function</CmdDesc>
        <CmdHeader>41 01</CmdHeader>
        <CmdParms>
        </CmdParms>
      </Cmd>
    [...]
    </L2CAP>
    <L2CAPCB>
    [...]
    </L2CAPCB>
  [...]
  </Commands>
  <Events>
    [...]
  </Events>
  [...]
</BLE>
```

Every command or event is described in a `<Cmd>` that contains:
- the name - `<CmdName>`
- the description - `<CmdDesc>`
- the opGroup and opCode - `<CmdHeader>`
- the parameters - `<CmdParms>`

The parameters of a command/event are specified using the `<Parm>` tag and contain:
- the name - `<ParmName>`
- the description - `<ParmDesc>`
- the size - `<ParmSize>`
- the type - `<ParmType>`
- the last value - `<ParmLastValue>`
- the default value - `<ParmDefaultValue>`
- the type specifics - `<ParmTypeSpecifics>`

The type specifics tag is used where needed to specify certain behaviours:
- `<ParmItem>` for specifying _enum_ items
- `<ParmState>` for specifying the state of a variable (e.g. _ReadOnly_)
- `<ParmRange>` for specifying the range of the values taken by a variable
- `<Depend>` for specifying a dependence on another variable (e.g.an _array_'s dependence on the length variable)
- `<Parm>` for specifying the parameters of a _struct_


## FSCI message example

For example, the `L2CAP-SendAttData.Request` command has the following XML structure:
```xml
<Cmd>
  <CmdName>L2CAP-SendAttData.Request</CmdName>
  <CmdDesc>Sends a data packet through ATT Channel</CmdDesc>
  <CmdHeader>41 03</CmdHeader>
  <CmdParms>
    <Parm>
      <ParmName>DeviceId</ParmName>
      <ParmDesc>The DeviceId for which the command is intended</ParmDesc>
      <ParmSize>1</ParmSize>
      <ParmType>tInt</ParmType>
      <ParmLastValue>00</ParmLastValue>
      <ParmDefaultValue>00</ParmDefaultValue>
    </Parm>
    <Parm>
      <ParmName>PacketLength</ParmName>
      <ParmDesc>Length of the ATT data packet to be sent</ParmDesc>
      <ParmSize>2</ParmSize>
      <ParmType>tInt</ParmType>
      <ParmLastValue>0001</ParmLastValue>
      <ParmDefaultValue>0001</ParmDefaultValue>
      <ParmTypeSpecifics>
        <ParmRange>0x0001-0xFFFF</ParmRange>
      </ParmTypeSpecifics>
    </Parm>
    <Parm>
      <ParmName>Packet</ParmName>
      <ParmDesc>The ATT data packet to be transmitted</ParmDesc>
      <ParmSize>1</ParmSize>
      <ParmType>tArray</ParmType>
      <ParmLastValue>00</ParmLastValue>
      <ParmDefaultValue>00</ParmDefaultValue>
      <ParmTypeSpecifics>
        <Depend>
          <DependName>PacketLength</DependName>
          <DependMask>0xFFFF</DependMask>
          <DependShift>0x0000</DependShift>
        </Depend>
      </ParmTypeSpecifics>
    </Parm>
  </CmdParms>
</Cmd>
```

This translates to:
| Tag                   | Value                                   | Description                      |
| --------------------- | --------------------------------------- | -------------------------------- |
| `<CmdName>`           | L2CAP-SendAttData.Request               | Name of the command/event |
| `<CmdDesc>`           | Sends a data packet through ATT Channel | Description of the command/event |
| `<CmdHeader>`         | 41 03     | opGroup and opCode of the command/event in hexadecimal format |
| `<CmdParms>`          | `<Parm>`  | List of the parameters used in the command/event |
| `<ParmName>`          |  DeviceId | Name of the parameter |
| `<ParmDesc>`          | The DeviceId for which the command is intended | Description of the parameter |
| `<ParmSize>`          | 1    | Parameter size (in bytes) |
| `<ParmType>`          | tInt | Type of the parameter |
| `<ParmLastValue>`     | 00   | Last value of the parameter in hexadecimal format |
| `<ParmDefaultValue>`  | 00   | Default value of the parameter in hexadecimal format |
| `<ParmTypeSpecifics>` | `<ParmRange>` | Specify range of the parameter |
|                       | `<Depend>`    | Specify dependency of the parameter "Packet" of type "tArray" of the variable `<DependName>` that is the PacketLength |


**Parent topic:** [Bluetooth Low Energy Host Stack FSCI Application Programming](../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md)
