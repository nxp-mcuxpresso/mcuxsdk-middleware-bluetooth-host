# Declaring characteristic descriptors

Characteristic’s Descriptors are declared after the Characteristic Value declaration and before the next Characteristic declaration.

The macros used to declare Characteristic Descriptors are very similar to those used to declare fixed-length Characteristic Values:



-   *DESCRIPTOR \(name, uuid16, permissions, descriptorValueLength, descriptorValueByte1, descriptorValueByte2, …\)*
-   *DESCRIPTOR\_UUID32 \(name, uuid32, permissions, descriptorValueLength, descriptorValueByte1, descriptorValueByte2, …\)*
-   *DESCRIPTOR\_UUID128\(name, uuid128, permissions, descriptorValueLength, descriptorValueByte1, descriptorValueByte2, …\)*

A special Characteristic Descriptor that is used very often is the *Client Characteristic Configuration Descriptor \(CCCD\)*. This is the descriptor where clients write some of the bits to activate Server notifications and/or indications. It has a reserved, 2-byte, SIG-defined UUID \(0x2902\), and its attribute value consists of only 1 byte \(out of which 2 bits are used for configuration, the other 6 are reserved\).

Because the CCCD appears very often in Characteristic definitions for standard Bluetooth Low Energy profiles, a special macro is used for CCCD declaration:



-   *CCCD \(name\)*

This simple macro is basically equivalent to the following Descriptor declaration:

```
* DESCRIPTOR \(name, *
    *0x2902, *
    (gGattAttPermAccessReadable_c
    | gGattAttPermAccessWritable_c),
    2, 0x00, 0x00)
```

**Parent topic:**[Declaring a characteristic](../topics/declaring_a_characteristic.md)

