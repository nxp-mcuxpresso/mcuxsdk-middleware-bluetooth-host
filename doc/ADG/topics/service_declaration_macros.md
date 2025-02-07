# Service declaration macros

The following macros are to be used for declaring a Service:



-   *PRIMARY\_SERVICE \(name, uuid16\)*
    -   Most often used.
    -   The *name* parameter is common to all macros; it is a universal, user-friendly identifier for the generated attribute.
    -   The *uuid16* is a 2-byte SIG-defined UUID, written in 0xZZZZ format.
-   *PRIMARY\_SERVICE\_UUID32 \(name, uuid32\)*
    -   This macro is used for a 4-byte, SIG-defined UUID, written in 0xZZZZZZZZ format.
-   *PRIMARY\_SERVICE\_UUID128 \(name, uuid128\)*
    -   The *uuid128* is the friendly name given to the custom UUID in the *gatt\_uuid128.h* file.
-   *SECONDARY \_SERVICE \(name, uuid16\)*
-   *SECONDARY\_SERVICE\_UUID32 \(name, uuid32\)*
-   *SECONDARY \_SERVICE\_UUID128 \(name, uuid128\)*
    -   All three are similar to Primary Service declarations.

**Parent topic:**[Declaring a service](../topics/declaring_a_service.md)

