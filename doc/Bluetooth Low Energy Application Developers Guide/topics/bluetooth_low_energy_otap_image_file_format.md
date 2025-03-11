# Bluetooth Low Energy OTAP image file format

The Bluetooth LE OTAP Image file has a binary file format. It is composed of a header followed by a number of sub-elements. The header describes general information about the file. There are some predefined sub-elements of a file but an end manufacturer could add manufacturer-specific sub-elements. The header does not have details of the sub-elements. Each element is described by its type.

The general format of an image file is shown in the table below.

|Image File Element|Value Field Length \(bytes\)|Description|
|------------------|----------------------------|-----------|
|Header|Variable|The header contains general information about the image file.|
|Upgrade Image Sub-element|Variable|This sub-element contains the actual binary executable image, which is copied into the flash memory of the target device. The maximum size of this sub-element depends on the target hardware.|
|Image File CRC Sub-element|2|This is a 16-bit CCITT type CRC which is calculated over all elements of the image file with the exception of the Image File CRC sub-element itself. This must be the last sub-element in an image file.|

Each sub-element in a Bluetooth LE OTAP Image File has a Type-Length-Value \(TLV\) format. The type identifier provides forward and backward compatibility as new sub-elements are introduced. Existing devices that do not understand newer sub-elements may ignore the data.

The following table shows the general format of a Bluetooth LE Image File sub-element.

|Subfield|Size \(Bytes\)|Format|Description|
|--------|--------------|------|-----------|
|Type|2|uint16|Type Identifier – determines the format of the data contained in the value field|
|Length|4|uint32|Length of the *Value* field of the sub-element.|
|Value|variable|uint8\[\]|Data payload|

Some sub-element type identifiers are reserved while others are left for manufacturer-specific use. The table below shows the reserved type identifiers and the manufacturer-specific ranges.

|Type Identifiers|Description|
|----------------|-----------|
|0x0000|Upgrade Image|
|0x0001 – 0xefff|Reserved|
|0xf000 – 0xffff|Manufacturer-Specific Use|

The OTAP Demo applications use two of the manufacturer-specific sub-element type identifiers while the rest remain free to use. The two are shown in the table below along with a short description.

|Manufacturer-specific Type Identifiers|Sub-element Name|Notes|
|--------------------------------------|----------------|-----|
|0xf000|Sector Bitmap|Bitmap that signals the bootloader the sectors of the internal flash, which should be overwritten and which should remain as is.|
|0xf100|Image File CRC|16-bit CRC that is computed over the image file with the exception of the CRC sub-element itself.|


```{include} ../topics/bluetooth_le_otap_header.md
:heading-offset: 2
```

**Parent topic:**[Over the Air Programming \(OTAP\)](../topics/over_the_air_programming_otap.md)

