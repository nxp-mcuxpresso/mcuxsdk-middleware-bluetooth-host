# "system" – system commands

```
-----------commands-------------
build_info | Show SW build information
debug [0|1] | Enable debug features (DTEST...)
verbosity [<0...255>] | Set/show verbosity flags (1=Info, 2=Debug, 4=MeasurementInfo, 8=MeasurementData, 16=MeasurementDebug, 32=BoardInfo, 64=Profiling)
unique_id | Show Unique Id
version | Show SW version number
reset | Reset MCU
store | Store all parameters
factory | Reload and store factory defaults
events | Show events
timing | Show BLE timing
baudrate <value> | Set/show baudrate of serial interface (default: 115200, volatile)
output_format | Show output format
calibrate [<MCIQ dist> <ToF dist> [store]] | Set/Show zero-distance calibration, distances are in meters, Q10
-------------end----------------
```
**Table. Wireless ranging system commands**
| Command       | Default | Comment |
| ------------- | ------- | ------- |
| verbosity     | 13      | Level of details of the range response in the form of bits field. Add verbosity flag values to combine them (for instance 12 for Measurement Info and Measurement Data). |
| unique_id     | NA      | 6 bytes unique identifier. Reserved for future use; all zeros for now |
| version       | NA      | Application version. For example: v2.0.0 |
| build_info    | NA      | Build version information (Git hash, and so on) |
| reset         | NA      | Reset the CPU |
| store         | NA      | Save settings to flash |
| factory       | NA      | Revert to default settings |
| events        | NA      | Reserved for future use |
| timing        | NA      | Reserved for future use |
| baudrate      | 115200  | Change the baud rate of the UART interface. This setting is volatile, that is, it is restored to the default value after a power cycle or reset. |
| output_format | 4       | Internal usage |

**Parent topic:**[Main menu](../topics/main_menu.md)

