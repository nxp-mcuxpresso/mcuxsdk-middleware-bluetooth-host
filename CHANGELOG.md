# Changelog

All notable changes to NXP Bluetooth LE Host will be documented in this file.

**NXP Bluetooth LE Host Stack** is certified **Bluetooth 6.0**

## [1.10.16] – mcux v2026-03-00-pvw2

### Added
- Implemented Bluetooth LE Host Transient Application Key feature (experimental). in ble_shell, loc_reader and loc_user_device
- Added Bluetooth LE Host Transient Application Key in ble_shell, loc_reader and loc_user_device
- Added Channel Sounding phase calibration, with 0.5m compensation for all boards
- Added Vendor Specific Command to enable Link Layer experimental features
- Added `gAppCsTimeInfo_d` define in app_preinclude.h for digital_key_car_anchor_cs on KW47EVK
- Added Channel Sounding **phase calibration** logic and updates in RAS CS Procedure Timing Measurement

### Improved
- Updated RADE algorithm source files license description with NXP proprietary license
- Improved data logging for digital key application
- Updated CCC enumeration values to align with CCC v4.0.0 specification
- Updated Intrusion Detection System documentation with event details
- Documentation miscellaneous updates

### Fixed
- Fixed ranging data body parser on the client side — aborted steps are now counted correctly to prevent subevent misalignment
- Fixed handling of CS Procedure Disable at Host and application level
- Fixed app_localization timer blocking when sensors were disabled
- Fixed setting of `gCsTimeInfo.transferStart` for BTCS Client L2CAP transfer
- Removed unused `gAppRealTimeDataTransfer_c` define from digital_key_car_anchor_cs
- Removed pin_mux files from core1 application projects
- Fixed issues in data transfer and event updates related to timing measurement

### Changed
- Updated `(SHELL_MAX_ARGS = 20)` in `app_preinclude.h` for localization applications.

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.15] – mcux v2026-03-00-pvw1

### Added

- Added an API to update local synchronization parameters after PAwR is already established on the scanner side
- Added Connection Subrating feature (experimental) in the Bluetooth LE Host
- Added Connection Subrating feature in ble_shell
- CCC v4.1.0: renamed DK_VERSION to VDBT_VERSION as per updated specification

### Improved

- Improved localization applications timer mechanism to reduce the number of wakeups
- Updated localization timeout values to be overridable by the application
- Retain the subevent_done_status local and remote values for all subevents and pass them to the algorithm
- Documentation miscellaneous updates

### Fixed

- Fixed L2CAP credit-based channel disconnection where the channel's timer ID would be set to 0 instead of gTmrInvalidTimerID_c
- Fixed Channel Sounding applications issue: clear the Indication Pending flag for a peer device on disconnect
- ble_shell: Set maximum arguments in command (SHELL_MAX_ARGS = 20) in app_preinclude.h

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23


## [1.10.14] - mcux v2025-12-00

### Added

- KW47/MCXW72 CCC and IOT Channel Sounding Localization applications on GitHub repository
- Monitoring Advertisers support in fsci_black_box and BleShell applications
- Local average and remote average RSSI values to the CS measurement report
- The "-Os" optimization flag to the ARMGCC release configuration for NCP applications

### Improved

- Enabled low power support in loc_reader_host application

### Fixed

- loc_reader_host application event set issue
- Missing handler for Version2 of the Set RPA Timeout command

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.13] - mcux v2025-12-00-pvw2

### Added

- Threshold for the invalid number of Anchor Monitor events received by the target anchor
- **Experimental Monitoring Advertisers** feature in Bluetooth LE Host
- **Experimental Randomized RPA** feature in Bluetooth LE Host
- Application defines for default connection and default advertising tx power

### Improved

- Miscellaneous applications updates
- Central applications now wait for status of Encrypt procedure in case of bonded device
- Logging data on localization applications
- NBU Low Power Mode enabled by default for Localization Applications
- PCT rotation calibration added to localization apps
- Configured CS Reflector to start the CS procedure with the tdm command and updated the documentation
- Populated the optionalSubfeaturesSupported field correctly
- Prevented CORE 0 from entering deep sleep while LCE is computing by setting the low power mode constraint to PWR_WFI during LCE computation and releasing it afterwards
- Implemented in CCC_CS, Channel Sounding data transfer from the anchor to the device
- Documentation miscellaneous updates
- Updated all kw47 and kw45 armgcc ld linker files to take gUseInternalStorageLink_d flag value into consideration

### Fixed

- Memory issue when setting scan response data would return an error status from the LL
- Set advertises with the public address, overwritten by a previously used random address on receiving the Advertising Set Terminated event

### Changed

- Updated memory configuration: replaced the extended heap area in the available SMU2 memory with a 24KB array in the data1
- Removed redundant cached remote capabilities write on reflectors, as the initiator will always trigger a capabilities exchange and trying to write cached capabilities afterwards results in an HCI error

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

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
