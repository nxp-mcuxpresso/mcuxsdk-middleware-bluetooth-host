# Integration

Perform the following steps to integrate A2A application common module in an application:

1. Add `app_a2a_interface.c` and `app_a2a_interface.h` from `middleware\wireless\bluetooth\application\common\auto\` in the application project.
2. Initialize the Connection Handover application common module by calling `A2A_Init()`.
3. Use `A2A_SendCommand()` as the communication callback for the Connection Handover application common module.
4. Use `A2A_SendSetBondingDataCommand()` in the application to send the bonding data from the connected anchor to the target anchor after pairing a new device is successful.