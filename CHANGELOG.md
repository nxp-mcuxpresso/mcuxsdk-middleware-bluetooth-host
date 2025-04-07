# Changelog

All notable changes to NXP Bluetooth Le Host will be documented in this file.

## [1.10.6] - mcux v2025-06-00-pvw1

### Added
- **Encypted Advertising Data** support in Extendded Advertising Applications.
- Support for **disable UART** for **CS applications** for **low power measurements**.
- Support for **LCE (DSPV) non-blocking API** integration to **RADE**.
- **Intrusion Detection System** as **Experimental**.

### Improved
- **L2CAP command length validation** to cover all signaling commands.
- **Extended NBU Wireless_uart_host  multiple connections** support. 

### Supported Platforms
- KW45, KW47, MCXW71, MCXW72, K32W1


## [1.10.5] - mcux v2025-03-00

### Added
- MCXW72 **Extended NBU** support and **w_uart_host**, **fsci_bridge** and **ncp_fscibb** sample applications.
- **L2CAP support** for Channel Sounding **IQ Sample Transfer** in CCC CS sample applications.
- Bluetooth LE Sample applications for **MCX-W71-EVK** board.

### Changed
- Updated **FSCI XML file**.
- Updated **Bluetooth LE Host Documentation**. 

### Fixed
- Cleared the **mpRemoteCachedCaps** entry when the peer disconnects (CS sample applications).
- Transfer **RAS subscription** data during connection handover (CCC CS sample applications).
- **EAD** - Updated advertising data length check to ensure encrypted data fits inside one AD.
- Updated **digital_key_car_anchor** and **digital_key_car_anchor_cs** applications to manage **Random Static address** from the application layer.

### Supported Platforms
- KW45, KW47, MCXW71, MCXW72, K32W1



## [1.10.4] - mcux v2025-03-00-pvw2

### Added
- PAWR support in **BLE Shell** sample application.
- PAWR support in **adv_ext_peripheral** and **adv_ext_central** sample applications.
- New sample applications for **FRDM-MCXW72**.
- **Gap_SetScanningCallback** API.
- Support for handover connection interval update command.

### Changed
- Updated HID Device for **Windows 11** compatibility.
- Updated CCC demos to **Digital Key R4 spec version 1.0.0**.
- Improved RPA resolution at the **Host level**, now performed synchronously.
- Enhanced parsing of the **CS procedure** in Ranging Service.

### Fixed
- Corrected parsing of the **PAST command** in FSCI GAP.
- Fixed **scan event reporting** in PAST scenario.
- Added an error case for `Gap_SetChannelMap` in the generic event handler.

### Supported Platforms
- KW45, KW47, MCXW71, MCXW72, K32W1
