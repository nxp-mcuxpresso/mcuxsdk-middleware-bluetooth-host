# Start task

The Start Task \(*start\_task*\) is the first task created by the operating system and is also the one that initializes the rest of the system. It initializes framework components \(Memory Manager, Timers Manager etc.\) and it calls *BluetoothLEHost\_AppInit* from *app.c*, which is used to initialize the Bluetooth LE Host Stack as well as peripheral drivers specific to the implemented application.

The function calls *BluetoothLEHost\_HandleMessages*, which represents the Application Task and is used to process events and messages from the Host Stack.

The stack size and priority of the main task are defined in *fsl\_os\_abstraction\_config.h*:

```
**\#ifndef** gMainThreadStackSize_c
**\#define** gMainThreadStackSize_c 1024
**\#endif**
**\#ifndef** gMainThreadPriority_c
**\#define** gMainThreadPriority_c 7
**\#endif**
```

**Parent topic:**[Application main framework](../topics/application_main_framework.md)

