# Image version

This is the full identifier of the image file. It should allow a Bluetooth LE OTAP Client to identify the target hardware, stack version, image file build version, and other parameters if necessary. The recommended format of this field \(which is used by the OTAP Demo applications\) is shown below but an end device manufacturer could choose different format. The subfields are shown in the order they are placed in memory from the first location to the last. Each subfield has a little-endian format, if applicable. Refer [Table 1](#TABLE_IGZ_LWM_CY)

|Subfield|Size \(bytes\)|Format|Description|
|--------|--------------|------|-----------|
|Build Version|3|uint8\[\]|Image build version.|
|Stack Version|1|uint8|0x41 for example for Bluetooth Low Energy Stack version 4.1.|
|Hardware ID|3|uint8\[\]|Unique hardware identifier.|
|End Manufacturer Id|1|uint8|ID of the hardware–specific to the end manufacturer|

This field value must be used in the *ImageVersion* field in the New Image Notification and New Image Info Response commands.

**Parent topic:**[Bluetooth Low Energy OTAP header](../topics/bluetooth_le_otap_header.md)

