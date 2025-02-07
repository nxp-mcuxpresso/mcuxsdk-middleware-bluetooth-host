# Declaring characteristic values

The Characteristic Value declaration immediately follows the Characteristic declaration and uses one of the following macros:



-   *VALUE \(name, uuid16, permissions, valueLength, valueByte1, valueByte2, …\)*
-   *VALUE\_UUID32 \(name, uuid32, permissions, valueLength, valueByte1, valueByte2, …\)*
-   *VALUE \_UUID128\(name, uuid128, permissions, valueLength, valueByte1, valueByte2, …\)*
    -   See [Declaring a service](declaring_a_service.md) for description of the *uuidXXX* parameter.
    -   The *permissions* parameter is a bit mask, whose flags are defined in *gattAttributePermissionsBitFields\_t* .
    -   The *valueLength* is the number of bytes to be allocated for the Characteristic Value. After this parameter, exactly \[*valueLength* \] bytes follow in 0xZZ format, representing the initial value of this Characteristic.

These macros are used to declare Characteristic Values of *fixed lengths*.

Some Characteristics have *variable length values*. For those, the following macros are used:



-   *VALUE\_VARLEN \(name, uuid16, permissions, maximumValueLength, initialValueLength, valueByte1, valueByte2, …\)*
-   *VALUE\_UUID32\_VARLEN \(name, uuid32, permissions, maximumValueLength, initialValueLength, valueByte1, valueByte2, …\)*
-   *VALUE\_UUID128\_VARLEN \(name, uuid128, permissions, maximumValueLength, initialValueLength, valueByte1, valueByte2, …\)*
    -   The number of bytes allocated for this Characteristic Value is *maximumValueLength*.
    -   The number of *valueByteXXX* parameters shall be equal to *initialValueLength*.

Obviously, *initialValueLength* is, at most, equal to *maximumValueLength*.

**Parent topic:**[Declaring a characteristic](../topics/declaring_a_characteristic.md)

