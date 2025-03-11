# When/how to enter low power

To enable low power at application level, the `gAppLowpowerEnabled_d` define should be set to `1` in `app_preinclude.h` file.

The system should enter low power when the entire system is idle, and all software layers agree on that. The device enters low power by calling the `PWR_EnterLowPower` function.

For FreeRTOS applications, the low-power entry point is placed in the FreeRTOS idle task, which has the lowest priority in the system. From that task, the `vPortSuppressTicksAndSleep` function is called, which at its turn, calls the `PWR_EnterLowPower` to enter low power.

For the BareMetal examples, the application low power entry point is placed in the main function and is called when there are no messages to be processed by other tasks.

The wake-up sources that can be configured for the application are UART or button. Note that Low-power timer wake-up source and wake-up from the Radio domain are directly enabled from the Connectivity framework.

Each software layer/entity running on the system can prevent it from entering low power by calling *PWR\_LowPowerEnterCritical* function. The system stays awake until all software layers that called *PWR\_LowPowerEnterCritical* call back *PWR\_LowPowerExitCritical* and the system reaches the low-power entry point.

When going to low power, the SDK Power Manager selects the best low-power mode that fits all the constraints.

The default low-power mode for each application is Deep-sleep mode. Users can change the behavior by setting a new low-power constraint for the application.

For example, if the low power constraint set from the application is Deep-sleep mode, and no other constraint is set, the**SDK Power Manager** selects Deep-sleep the next time the device enters low power. However, there might be a case when a WFI constraint is set \(*PWR\_SetLowPowerModeConstraint \(PWR\_WFI\)*\) by another component, such as the SecLib module that operates Hardware encryption. In such cases, the **SDK Power Manager** selects this WFI mode until the constraint is released by the SecLib module \(*PWR\_ReleaseLowPowerModeConstraint\(PWR\_WFI\)*\).

If it is required to change the mode from Deep-sleep to Power-down mode, the deep sleep constraint \(*PWR\_ReleaseLowPowerModeConstraint\(PWR\_DeepSleep\)*\) must be released and the power-down constraint \(*PWR\_SetLowPowerModeConstraints\(PWR\_PowerDown\)*\) must be set. The **SDK Power Manager** selects the Power-down mode when the device enters low power.

**Parent topic:**[Low-Power Management](../topics/low-power_management.md)

