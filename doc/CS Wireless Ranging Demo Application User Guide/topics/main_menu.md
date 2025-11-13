# Main menu

By sending the "help" command, a help menu gets listed \(see below\). Commands are provided to initiate a ranging measurement, display, and modify parameters.

```
-----------commands-------------
range [<ble-address>] | Run HADM procedure measurement with given peer
test [<tx,rx>] | Run HADM test mode measurement
setup [<ble-address>] | Run HADM setup (from Central only)
communication [options] | Communication commands
parameter [options] | Parameter commands
system [options] | System commands
misc [options] | Miscellaneous commands
-------------end----------------
```

The options displayed are explained in the table below:

**Table: Wireless ranging top level commands**
| Command       | Description                                                                                                                                                                                        |
| ------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| range         | Execute a CS measurement using the Bluetooth Low Energy connection (implicitly set service role of the board as "client"). Limitation: should be issued from the same board after first invocation |
| test          | Execute a CS measurement using CS test either as initiator (TX) or reflector (RX)                                                                                                                  |
| parameter     | Submenu to set/display CS parameters                                                                                                                                                               |
| communication | Set Bluetooth Low Energy role (central/peripheral)                                                                                                                                                 |
| system        | Submenu to set/display system-related parameters                                                                                                                                                   |
| misc          | Submenu to set/display miscellaneous parameters                                                                                                                                                    |


```{include} ../topics/parameter_-_cs_parameters.md
:heading-offset: 2
```

```{include} ../topics/system__system_commands.md
:heading-offset: 2
```

```{include} ../topics/misc__miscellaneous_commands.md
:heading-offset: 2
```

```{include} ../topics/firmware_response_interpretation.md
:heading-offset: 2
```

**Parent topic:**[Embedded firmware command reference](../topics/embedded_firmware_command_reference.md)

