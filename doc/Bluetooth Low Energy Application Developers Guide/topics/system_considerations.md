# System considerations

The KW45/K32W1 has a dual-core architecture and has two separated power domains:

-   The main domain for the Cortex M33
-   The Radio domain which comprises the Cortex M3 core and the NBU \(Narrow Band Unit\).

The two power domains can go into or exit independently different low-power modes, namely Wait for instruction \(WFI\), Deep-sleep mode, Power-down mode, and Deep Power-down mode.

In Wait For Interrupt \(WFI\) mode, the CPU core is powered ON but is in an idle mode with the clock turned OFF.

In Deep Sleep mode, the fast clock is turned off, and the CPU along with the main power domain are placed into a retention state, with the voltage being scaled down to support state retention only. As no high frequency clock runs in this mode, the voltage applied on the power domain can be reduced to reduce leakage on the hardware logic. This reduces the overall power consumption in the Deep Sleep mode. When waking up from Deep Sleep mode, the core voltage is increased back to nominal voltage, the fast clock \(FRO\) is turned back on, and the peripheral in this domain can be reused as normal.

In Power-down mode, both the clock, and power are shut off to the CPU and the main peripheral domain. SRAM is retained, but register values are lost. The SDK power manager handles the restore of the processor registers and dependencies such as interrupt controller and similar ones transparently from the application.

In Deep Power-down mode the SRAM is not retained. This is the lowest power mode available. It is exited through the reset sequence.

**Parent topic:**[Low-Power Management](../topics/low-power_management.md)

