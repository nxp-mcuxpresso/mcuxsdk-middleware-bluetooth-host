# Bluetooth LE Host task configuration

Application developers are provided with two files for RTOS task initialization:

-   *ble\_host\_task\_config.h*, and *ble\_host\_tasks.c* for the Host.

Reusing these files is recommended because they perform all the necessary RTOS-related work. The application developer must only modify the macros from *\*\_config.h* files whenever tasks need a bigger stack size or different priority settings. The new values should be overridden in the *app\_preinclude.h* file.

**Parent topic:**[RTOS specifics](../topics/rtos_specifics.md)

