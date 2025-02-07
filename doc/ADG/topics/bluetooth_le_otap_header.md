# Bluetooth Low Energy OTAP header

The format and fields of the Bluetooth Low Energy OTAP Header are summarized in the table below.

|Octets|Data Types|Field Name|Mandatory/Optional|
|4|Unsigned 32-bit integer|Upgrade File Identifier|M|
|2|Unsigned 16-bit integer|Header Version|M|
|2|Unsigned 16-bit integer|Header Length|M|
|2|Unsigned 16-bit integer|Header Field Control|M|
|2|Unsigned 16-bit integer|Company Identifier|M|
|2|Unsigned 16-bit integer|Image ID|M|
|8|8 byte array|Image Version|M|
|32|Character string|Header String|M|
|4|Unsigned 32-bit integer|Total Image File Size

 \(including header\)

|M|

The fields are shown in the order they are placed in memory from the first location to the last.

The total size of the header without the optional fields \(if defined by the *Header Field Control*\) is 58 bytes.

All the fields in the header have a little endian format with the exception of the *Header String* field which is an ASCII character string.

A packed structure type definition for the contents of the Bluetooth LE OTAP Header can be found in the *otap\_interface.h*file.


```{include} ../topics/upgrade_file_identifier.md
:heading-offset: 3
```

```{include} ../topics/header_version.md
:heading-offset: 3
```

```{include} ../topics/header_length.md
:heading-offset: 3
```

```{include} ../topics/header_field_control.md
:heading-offset: 3
```

```{include} ../topics/company_identifier.md
:heading-offset: 3
```

```{include} ../topics/image_id.md
:heading-offset: 3
```

```{include} ../topics/image_version.md
:heading-offset: 3
```

```{include} ../topics/header_string.md
:heading-offset: 3
```

```{include} ../topics/total_image_file_size.md
:heading-offset: 3
```

**Parent topic:**[Bluetooth Low Energy OTAP image file format](../topics/bluetooth_low_energy_otap_image_file_format.md)

