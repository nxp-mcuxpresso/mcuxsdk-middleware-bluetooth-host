# Radio TX Power level

The controller interface includes APIs that can be used to set the Radio TX Power to a different level than the default one.

The power level can be set differently for advertising and connection channels by calling the function *Controller\_SetTxPowerLevelDbm\(\)* with the channel parameter set to *gAdvTxChannel\_c* or *gConnTxChannel\_c*.

**Parent topic:**[Bluetooth LE Host Stack Initialization and APIs](../topics/bluetooth_le_host_stack_initialization_and_apis.md)

