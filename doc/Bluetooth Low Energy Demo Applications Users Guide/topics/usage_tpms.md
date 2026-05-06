# Usage

The setup requires two supported platforms, one for the TPMS Sensor application and one for the TPMS Monitor application.

1.  Open a serial port terminal and connect it to the TPMS Monitor board, in the same manner as described in [Testing devices](testing_devices.md). The start screen is displayed after the board is reset.
2.  To start connectable advertising on the TPMS Sensor, press the CONNADVSW button. The device will start advertising.
3.  To start scanning on the TPMS Monitor, press the SCANSW button.
4.  If the TPMS Monitor connects to a sensor node, it will:
    1.  perform service discovery
    2.  pair and bond to establish itself as a Primary Monitor
        1.  if the Sensor already has a Primary Monitor, bonding will not be performed and the Monitor will have the role of Secondary Monitor
        2.  The following steps assume the role is that of Primary Monitor
    3.  read the TPMS Properties, TPMS Signing Key, TPMS Tire Pressure, TPMS Tire Temperature and TPMS Tire Acceleration characteristic values
    4.  update the TPMS Duty Cycle by writing to the corresponding characteristic
    5.  enable indications for the TPMS Signing Key characteristic
    6.  enable notifications for the TPMS Tire Pressure and TPMS Tire Temperature characteristics
5.  While in connection, pressing CONNADVSW on the TPMS Sensor will notify the Tire Pressure and Temperature to the Monitor, while pressing NCONNADVSW will update the Signing Key and indicate this to the Monitor.
6.  At this point, the connection can be terminated by either disconnecting via long press of the SCANSW on the TPMS Monitor or by resetting the boards. 
7.  To start non-connectable advertising on the TPMS Sensor, press the NCONNADVSW button. The device will start advertising in non-connectable mode (if no Primary Monitor exists, the advertising will be connectable).
8.  To start scanning on the TPMS Monitor, press the SCANSW button. The TPMS Monitor will display the TPMS data in the non-connectable advertising of the TPMS Sensor if it successfully authenticates the payload as described by the TPMS Profile specification.
9.  To factory reset the Sensor, long press NCONNADVSW. For the Monitor, long press FACTORYRESETSW.

    **Output Console on TPMS Monitor**
    ![](../images/tpms_monitor_nonconn.png "Output Console on TPMS Monitor (non-connectable advertising)")

**Parent topic:**[TPMS Sensor and Monitor](../topics/tpms_sensor_and_monitor.md)

