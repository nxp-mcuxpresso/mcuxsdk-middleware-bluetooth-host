# Image ID

This is a unique short identifier for the image file. It is used to request parts of an image file. This number should be unique for all images available on a Bluetooth LE OTAP Server.

-   The value 0x0000 is reserved for the current running image.

-   The value 0xFFFF is reserved as a “no image available” code for New Image Info Response commands.


This field value must be used in the *ImageID* field in the *New Image Notification* and *New Image Info Response* commands.

**Parent topic:**[Bluetooth Low Energy OTAP header](../topics/bluetooth_le_otap_header.md)

