# LE Power Control feature

The Bluetooth LE Shell application supports the LE Power Control feature, which allows monitoring and control of transmit power levels and path loss reporting on active connections. This feature is available on platforms supporting Bluetooth Core Specification 5.2 or later.

- In `app_preinclude.h` file, ensure the following defines are set:
  - `gBLE52_d` set to `1` (BLE 5.2 support)
  - `gBLE52_LePowerControlSupport_d` set to `TRUE`
  - `gHostInitEnableExpmFeatures_c` set to `1`

## Overview

LE Power Control provides the following capabilities:
- Read local and remote transmit power levels for different PHY types
- Monitor path loss between devices with configurable thresholds
- Receive notifications when power levels or path loss changes
- Configure path loss reporting parameters

This feature requires an active connection between two BLE devices.

## Shell Commands

### Enhanced Read Transmit Power Level

Reads the current and maximum transmit power levels of the local Controller on the ACL connection identified by the Connection_Handle parameter and the PHY indicated by the PHY parameter.

**Command:** `gap pwrread <peerID> <phy>`

**Parameters:**
- `peerID`: The peer device ID (0-based index)
- `phy`: PHY type:
  - `1` = LE 1M PHY (gPowerControlLePhy1M_c)
  - `2` = LE 2M PHY (gPowerControlLePhy2M_c)
  - `3` = LE Coded PHY with S=8 data coding (gPowerControlLePhyCodedS8_c)
  - `4` = LE Coded PHY with S=2 data coding (gPowerControlLePhyCodedS2_c)

**Example:**
```
BLE Shell>gap pwrread 0 1
BLE Shell>
-->  GAP Event: Enhanced Read Transmit Power Level
     PHY: LE 1M PHY
     Current TX Power: 0 dBm
     Max TX Power: 10 dBm
BLE Shell>
```

### Read Remote Transmit Power Level

Reads the transmit power level used by the remote Controller on a specified PHY on an ACL connection.

**Command:** `gap pwrremote <deviceId> <phy>`

**Parameters:**
- `deviceId`: The peer device ID (0-based index)
- `phy`: PHY type (same values as above)

**Example:**
```
BLE Shell>gap pwrremote 0 1
BLE Shell>
-->  GAP Event: Transmit Power Reporting
     Reason: Read Remote TX Power completed
     PHY: LE 1M PHY
     TX Power Level: -11 dBm
     Delta: 0 dB
     Flags: [Normal Range]
BLE Shell>gap pwrremote 0 1
BLE Shell>
-->  GAP Event: Transmit Power Reporting
     Reason: Read Remote TX Power completed
     PHY: LE 1M PHY
     TX Power Level: 10 dBm
     Delta: 0 dB
     Flags: [At Maximum]
```

### Set Path Loss Reporting Parameters

Configures the thresholds and parameters for path loss monitoring.

**Command:** `gap pathlossparams <deviceId> <highThreshold> <highHysteresis> <lowThreshold> <lowHysteresis> <minTimeSpent>`

**Parameters:**
- `deviceId`: The peer device ID (0-based index)
- `highThreshold`: High path loss threshold in dB
- `highHysteresis`: Hysteresis value for high threshold in dB
- `lowThreshold`: Low path loss threshold in dB
- `lowHysteresis`: Hysteresis value for low threshold in dB
- `minTimeSpent`: Minimum time to spend in a zone before reporting, in connection events

**Example:**
```
BLE Shell>gap pathlossparams 0 40 10 20 10 10
BLE Shell>
-->  GAP Event: Path Loss Reporting Parameters Setup Complete
     Parameters configured successfully!
BLE Shell>
```

### Enable Path Loss Reporting

Enables or disables path loss reporting on an active connection.

**Command:** `gap pathlossenable <deviceId> <enable>`

**Parameters:**
- `deviceId`: The peer device ID (0-based index)
- `enable`: 
  - `0` = Disable path loss reporting
  - `1` = Enable path loss reporting

**Example:**
```
BLE Shell>gap pathlossenable 0 1
BLE Shell>
-->  GAP Event: Path Loss Reporting State Changed
     Path loss reporting is now enabled!
BLE Shell>
```

Once enabled, path loss threshold events will be reported:
```
BLE Shell>
-->  GAP Event: Path Loss Threshold HIGH zone entered
     Current Path Loss: 48 dB
BLE Shell>
```

### Enable Transmit Power Reporting

Enables or disables transmit power change reporting.

**Command:** `gap pwrenable <deviceId> <localEnable> <remoteEnable>`

**Parameters:**
- `deviceId`: The peer device ID (0-based index)
- `localEnable`:
  - `0` = Disable local TX power reporting
  - `1` = Enable local TX power reporting
- `remoteEnable`:
  - `0` = Disable remote TX power reporting
  - `1` = Enable remote TX power reporting

**Example:**
```
BLE Shell>gap pwrenable 0 1 1
BLE Shell>
-->  GAP Event: Transmit Power Reporting State Changed
     Transmit power reporting enabled!
BLE Shell>
```

When enabled, power change events will be reported:
```
BLE Shell>
-->  GAP Event: Transmit Power Reporting
     Reason: Remote TX power changed
     PHY: LE 1M PHY
     TX Power Level: -5 dBm
     Delta: +3 dB
```

## Events

**Enhanced Read Transmit Power Level**

Returned in response to the `gap pwrread` command.

**Event fields:**
- PHY: The PHY type (1M, 2M, Coded S=8, Coded S=2)
- Current TX Power: Current transmit power level in dBm (-127 to 20, or "Unavailable")
- Max TX Power: Maximum transmit power level in dBm (-127 to 20)

**Path Loss Reporting Parameters Setup Complete**

Confirms that path loss reporting parameters have been configured successfully.

**Path Loss Reporting State Changed**

Confirms that path loss reporting has been enabled or disabled.

**Path Loss Threshold**

Generated when path loss crosses a configured threshold and stays in the new zone for the minimum time.

**Event fields:**
- Current Path Loss: The measured path loss in dB
- Zone Entered:
  - `LOW`: Path loss is below low threshold (good signal)
  - `MID`: Path loss is between thresholds (moderate signal)
  - `HIGH`: Path loss is above high threshold (weak signal)

**Transmit Power Reporting State Changed**

Confirms that transmit power reporting has been enabled or disabled.

**Transmit Power Reporting**

Generated when local or remote transmit power changes, or in response to a remote power level read request.

**Event fields:**
- Reason:
  - `Local TX power changed`: Local controller changed TX power
  - `Remote TX power changed`: Remote device changed TX power
  - `Read Remote TX Power completed`: Response to `gap pwrremote` command
- PHY: The PHY type
- TX Power Level: Power level in dBm (-127 to 20)
- Delta: Change in power level in dB since last report (with +/- sign)
- Flags (for remote read only):
  - `[At Minimum]`: Displayed when at minimum power level
  - `[At Maximum]`: Displayed when at maximum power level
  - `[Normal Range]`: Displayed when between min and max

## Usage Example

The following example demonstrates path loss monitoring between two devices:
First we create the connection between the devices.
**On Device 1 (Peripheral):**

1. Start advertising:
   ```
   BLE Shell>gap devicename HRS
   -->  GATTDB Event: Attribute Written
   HRS>gap advdata 1 6
   HRS>gap advdata 8 HRS
   HRS>gap advstart
   HRS>
   -->  GAP Event: Advertising parameters successfully set.
   HRS>
   -->  GAP Event: Advertising data successfully set.
   HRS>
   -->  GAP Event: Advertising state changed successfully!
   HRS>gattdb addservice 0x180D
   --> Heart Rate
        - Heart Rate Measurement  Value Handle: 14
           - CCCD Handle: 15
        - Body Sensor Location  Value Handle: 17
        - Heart Rate Control Point  Value Handle: 19
   -->  GATTDB Event: Service Added in database.
   ```

**On Device 2 (Central):**

2. Scan and connect:
   ```
   BLE Shell>gap devicename Collector
   -->  GATTDB Event: Attribute Written
   Collector>gap scanstart filter
   Collector>
   -->  GAP Event: Scan started.
   Collector>
   -->  GAP Event: Found device 0 : F697CFBF6A17 -93 dBm
   -->  GAP Event: Found device 1 : HRS 006037CFEAE9 -29 dBm
   Collector>gap connect 1
   Collector>
   -->  GAP Event: Scan stopped.
   Collector>
   -->  GAP Event: Connected to peer 0
   ```

3. Configure path loss parameters:
   ```
   Collector>gap pathlossparams 0 40 10 20 10 10
   Collector>
   -->  GAP Event: Path Loss Reporting Parameters Setup Complete
        Parameters configured successfully!
   ```

4. Enable path loss reporting:
   ```
   Collector>gap pathlossenable 0 1
   Collector>
   -->  GAP Event: Path Loss Reporting State Changed
        Path loss reporting is now enabled!
   ```

5. Monitor path loss (move devices to change signal strength, you will get a new notification when the path loss crosses a threshold and stays in the new zone for the minimum time):
   ```
   Collector>
   -->  GAP Event: Path Loss Threshold MID zone entered
        Current Path Loss: 30 dB
   Collector>
   -->  GAP Event: Path Loss Threshold LOW zone entered
        Current Path Loss: 18 dB
   Collector>
   -->  GAP Event: Path Loss Threshold HIGH zone entered
        Current Path Loss: 42 dB
   ```

6. Read current transmit power:
   ```
   Collector>gap pwrread 0 1
   Collector>
   -->  GAP Event: Enhanced Read Transmit Power Level
        PHY: LE 1M PHY
        Current TX Power: -20 dBm
        Max TX Power: 10 dBm
   ```

7. Enable transmit power reporting:
   ```
   Collector>gap pwrenable 0 1 1
   Collector>
   -->  GAP Event: Transmit Power Reporting State Changed
        Transmit power reporting enabled!
   ```

8.  Monitor transmit power changes (move devices or wait for controller to adjust power):
   ```
   Collector>
   -->  GAP Event: Transmit Power Reporting
        Reason: Remote TX power changed
        PHY: LE 1M PHY
        TX Power Level: 1 dBm
        Delta: -3 dB
   Collector>
   -->  GAP Event: Transmit Power Reporting
        Reason: Remote TX power changed
        PHY: LE 1M PHY
        TX Power Level: -2 dBm
        Delta: -3 dB
   Collector>
   -->  GAP Event: Transmit Power Reporting
        Reason: Local TX power changed
        PHY: LE 1M PHY
        TX Power Level: -20 dBm
        Delta: -3 dB
   ```
## Notes

- LE Power Control requires Bluetooth 5.2 or later support in both the host stack and the controller firmware
- The path loss zones (LOW/MIDDLE/HIGH) help applications make link adaptation decisions

**Parent topic:** [Bluetooth LE Shell](../topics/bluetooth_le_shell_513.md)
