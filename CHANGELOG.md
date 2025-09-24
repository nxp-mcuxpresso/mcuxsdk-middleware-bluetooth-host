# Changelog

All notable changes to NXP Bluetooth LE Host will be documented in this file.

**NXP Bluetooth LE Host Stack** is certified **Bluetooth 6.0**

## [1.10.12] - mcux v2025-12-00-pvw1

### Added

- **Gap_LoadCustomBondedDeviceInformation API** to retrieve custom peer information using NVM index

### Improved

- Miscellaneous application updates

### Fixed

- **Updated privacy timeout** mechanism affected by LE Set Extended Advertising Enable Command

### Changed

- Merged **Gap_SetPeriodicAdvParameters** and Gap_SetPeriodicAdvParametersV2 into Gap_SetPeriodicAdvParameters

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.11] - mcux v2025-09-00

### Added

- CCC sample applications updated to **CCC Digital Key v4.0.0** Specification
- **RAS/RAP PTS** 8.7.4 test support added in Localization Sample applications
- Support for CS start procedure while the previous procedure is not completed; old procedure replaced with the new one
- Support for arm gcc for ncp_loc_reader__cm33_core1

### Improved

- Localization Sample Applications Ram partition
- **RAS/RAP** profile and service
- Various sample applications have been updated


### Fixed

- Privacy setting issue on ncp_loc_reader
- Extended NBU FSCI message handling issue
- Ble_shell updated to set the Random Static Address properly
- Always set the Advertising Legacy Set handle if the legacy API was used
- fsci_bridge and w_uart_host memory leak
- PAWR parameters in PeriodicSyncTransferReceived are now parsed correctly
- Ensure an RPA/NRPA is properly set from the application to enable a central using Controller Privacy to connect to unbonded peripherals
- CS algorithm buffer overwrite issue during Connection Handover application
- Various sample applications bug fixes applied
- Pass correct Codded PHY (S2) to Channel Sounding Set Procedure Parameters

### Changed

- Merged Gap_SetExtAdvertisingParameters and Gap_SetExtAdvertisingParametersV2 into **Gap_SetExtAdvertisingParameters**
- ce_status_buffer type changed to int32_t
- BLE_Shell prints Random Static address as identity address instead of the Public Device Address

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.10] - mcux v2025-09-00-pvw2

### Improved

- **CS Event Handling**: CS (Channel Sounding) events are now sent to the application task for processing, 
	rather than being handled directly in the Host task
- Various sample applications have been updated

### Fixed

- Bluetooth Advertising Sets: Now supports **4 advertising** sets in the Bluetooth host libraries
- Various sample applications bug fixes applied

### Changed

- Bluetooth Address Type: The default address type has been changed from **Public to Random Static**

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.9] - mcux v2025-09-00-pvw1

### Added

- **IDS event** trigger when incoming ATT packets exceed agreed MTU
- **IDS event** trigger when Unexpected SMP Messages received in idle states (before pairing starts)
- Average RSSI reporting in Anchor Monitor event
- Support for gAppDeferAlgoRun_d in btcs_client.c
- **Multi-connection monitoring** in Handover/Monitor Mode

### Improved

- Updated **NBU channel sounding** applications to support **64MHz** clock speed
- Cleanup of commComplete structures that only contain status from hci_interface.h

### Fixed

- CCC Application handover monitoring RSSI issue
- Intrusion Detection System not reporting event type
- Extended **NBU armgcc projects** stability
- Advertising Extended Applications when Gap_PeriodicAdvCreateSync fails

### Changed

- Updated digital_key_car_anchor applications to configure coding scheme via Host API
- Enhanced RAS handling of ACK Ranging Data in invalid conditions 

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.8] - mcux v2025-06-00

### Added

- **Gap_SetBondedDeviceName()** to set device name using NVM index
- **RAS** queue for GATT indications sent
- **gHciStatusBase_c** to **csError** status
- Option to use statically allocated **memory** for dynamic **GATT database** (prevents heap fragmentation)
- Checks for **controller** supported features and setting **PAST bits** accordingly
- **Anchor** support to **export** device data via RAS using gAppHciDataLogExport_d = 2
- **Anchor** support to **export** device local HCI data using gAppHciDataLogExport_d = 1
- **Shell commands** to list peer devices and trigger connection handover
- Define for **enabling** optional **CCC** LE Coded **PHY** advertising
- **cs_sync_phy** parameter to mDefaultRangeSettings (**renamed** from outdated RTTPhy)

### Improved

- **Stack Host** now saves the most recently set **random address** after successful controller response
- Miscellaneous **minor** application **updates**

### Fixed

- Compilation issue in **loc_reader app** with real-time RAS transfer
- **CCC** application **handover state machine** race condition
- **CCC** resets **gCurrentAdvHandle** upon connection
- **RAS** uses correct bit for data overwrite preference

### Changed

- Updated **Bluetooth LE Host Documentation**.
- **BLE_Shell** Tx timer interval adjusted for **max throughput** on 1M PHY
- **CS_ConfigVendorCommand** updated with **Inline Phase Return** field
- Renamed **tx_pwr_phy** to **phy** and removed obsolete rtt_phy field
- Updated **documentation** to clarify **Controller Privacy** restrictions 

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.7] - mcux v2025-06-00-pvw2

### Added
- Support for **Bluetooth LE Debug Keys** 
- Support for **pairing failure** reason 0x10 (Busy)
- Extended NBU **Wireless_uart_host battery service**
- **Channel Sounding RSSI Information** at application level
- Application support for **4-antenna configuration**

### Improved
- **Clean** include directives
- Allow central-only devices to use **Gap_EncryptAdvertisingData**
- **CSTACK** size optimization for **RADE** using baremetal apps

### Fixed
- Extended NBU **memory leak** issues
- Issues when **gAppOtaASyncFlashTransactions_c** is set to 0
- Issue with **CS procedure** affected by the CS data export
- Extended NBU **Wireless_uart_host privacy**

### Changed
- Updated **Bluetooth LE Host Documentation**. 

### Supported Platforms
- KW45, KW47, MCXW71, MCXW72, K32W1

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
