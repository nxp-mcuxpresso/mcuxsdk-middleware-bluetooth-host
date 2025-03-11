# Task and event queues

The task queues are declared in the *ble\_host\_tasks.h* as follows:

```
/*! App to Host message queue for the Host Task */
    extern messaging_t   gApp2Host_TaskQueue;
/*! HCI to Host message queue for the Host Task */
    extern messaging_t   gHci2Host_TaskQueue;
/*! Event for the Host Task Queue */
    extern OSA_EVENT_HANDLE_DEFINE(gHost_TaskEvent);
```

See [Initialization](initialization.md#) for more details about the RTOS tasks required by the Bluetooth LE Host Stack.

**Parent topic:**[Prerequisites](../topics/prerequisites.md)

