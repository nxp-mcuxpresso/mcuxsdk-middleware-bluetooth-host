# Initialization

The application developer is required to configure the Host Task as part of the Host Stack requirement. The task is the context for running all the Host layers \(GAP, GATT, ATT, L2CAP, SM, GATTDB\)

The prototype of the task function is located in the *ble\_host\_tasks.h* file:

```
void Host_TaskHandler(void * args);
```

It should be called with *NULL* as an argument in the task code from the application.

Application developers are required to define task events and queues as explained in [RTOS Task Queues and Events](task_and_events_queues.md#).

If the Controller software runs on the same chip as the Host, the Controller task always has a higher priority than the Host task. The priority value of the Host Task can be configured through the *gHost\_TaskPriority\_c define* \(by default set in *ble\_host\_task\_config.h*\). Note that changing this value can have a significant impact on the Bluetooth Low Energy stack.

**Parent topic:**[Bluetooth LE Host Stack Initialization and APIs](../topics/bluetooth_le_host_stack_initialization_and_apis.md)

