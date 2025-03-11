# Header version

This 2 byte field contains the major and minor version number. The high byte contains the major version and the low byte contains the minor version. The current value is “**0x0100**” with the major version “**01**” and the minor version **“00**”. A change to the minor version means the OTA upgrade file format is still backward compatible, while a change to the major version suggests incompatibility.

**Parent topic:**[Bluetooth Low Energy OTAP header](../topics/bluetooth_le_otap_header.md)

