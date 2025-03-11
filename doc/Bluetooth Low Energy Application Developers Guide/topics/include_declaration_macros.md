# Include declaration macros

Secondary Services are meant to be included by other Services, usually by Primary Services. Primary Services may also be included by other Primary Services. The inclusion is done using the Include declaration macro:



-   *INCLUDE \(service\_name\)*
    -   The *service\_name* parameter is the friendly name used to declare the Secondary Service.
    -   This macro is used only for Secondary Services with a SIG-defined, 2-byte, Service UUID.
-   *INCLUDE\_CUSTOM \(service\_name\)*
    -   This macro is used for Secondary Services that have either a 4-byte UUID or a 16-byte UUID.

The effect of the service inclusion is that the *including* Service is considered to contain all the Characteristics of the *included* Service.

**Parent topic:**[Declaring a service](../topics/declaring_a_service.md)

