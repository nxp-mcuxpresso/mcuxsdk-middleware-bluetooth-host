# Changelog

All notable changes to NXP Bluetooth Le Host will be documented in this file.

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
