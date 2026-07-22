# Changelog

All notable changes to NXP Bluetooth LE Host will be documented in this file.

**NXP Bluetooth LE Host Stack** is certified **Bluetooth 6.0**


## [1.10.22] - mcux v2026-09-00-pvw2

### Added

- Added `gSecEvt_SameConfirmValue_c` IDS event to detect Legacy Pairing Confirm Value replay attacks

### Improved

- Replaced mode assert with graceful drop and cleanup in localization applications
- RAS/RAP: handle case where MTU size reduction forces use of more segments than predefined `gRASMaxNoOfSegments_c`
- Advertising-set-related BLE Host storage is now allocated by the application and scales with the configurable `gMaxAdvSets_c`

### Fixed
- Fixed CS measurement not starting after handover in digital_key_car_anchor_cs
- Fixed Insufficient Encryption returned instead of Insufficient Authentication
- Fixed RSSI filter not excluding invalid RSSI values in CCC monitoring
- Fixed missing CCC characteristics in digital_key_car_anchor_cs example
- Fixed mismatch between ExtendedFeatures from GAPInit and HCI LE Read All Remote Features Complete event
- Miscellaneous Sample Applications fixes
- Miscellaneous Coverity fixes
- Miscellaneous MISRA fixes

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23, KW43, MCXW70


## [1.10.21] - mcux v2026-09-00-pvw1

### Added
- RSSI-based adaptive CS procedure interval for localization applications (experimental; disabled by default)
- Added BTCS timer mechanism to ensure CS procedure loop restarts are not blocked by failed L2CAP transfers

### Improved
- CS procedure parameters are now updated on connection interval change to avoid CS request rejection
- Aligned wireless_uart preinclude settings across all platforms for consistency
- Updated ble_shell periodic advertising default interval set to 1 second for improved discoverability
- Increased host task stack size across multiple applications
- Extended ble_shell periodic advertising handling to support gPeriodicDeviceScannedV2_c event
- RADE component naming updated in documentation
- Unified CS results complete event handling; real-time transfer moved to AppLocalization_HandleCompleteResults()

### Fixed
- Fixed out-of-bounds access in gap.c
- HCI LE Read All Remote Features command is now issued if supported by the controller
- Power Control APIs now return gBleFeatureNotSupported_c if the controller does not indicate support
- Fixed array index overflow in application ranging_client.c
- Fixed local RSSI parsing: correct byte is now collected before advancing parse pointer
- Fixed bounds check for gaAntPermNAp in processMode2Data and processMode3Data
- Fixed localization index overflow; added sanity check
- Corrected bounds check for deviceId
- Fixed wireless_uart mAdvPending flag to prevent multiple advertising starts during multi-peer disconnect
- Fixed missing RSSI variables causing compilation issues in app_localization_utils.c
- TPMS sensor Signing Key and sequence number now saved in NVM to prevent MAC check failures
- CS IPT mode now configured with only 1 antenna path to fix Samsung phone CS request rejection
- FSCI Extended NBU: Added LL_API_NotifyWakeUp call in idle_task to detect NBU stuck condition; added leExtendedFeatures to NCP GAP init complete event
- Fixed systick drift when PWR_DisallowDeviceToSleep() is active
- CS: Added check for valid numAntennaPaths in processCsResultsEvent
- Miscellaneous MISRA fixes
- Miscellaneous Coverity fixes

### Changed
- Reverted incorrect connection handle handling for non-connection enhanced notification events

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23, KW43, MCXW70


## [1.10.20] - mcux v2026-06-00

### Added
- Tire Pressure Monitoring System (TPMS) sample demo applications (KW4x)
- Tire Pressure Monitoring System (TPMS) documentation added to DAUG
- Localization accelerator LCE support for KW43/MCXW70
- Added LE Power Control support in BLE Shell application
- LE Read All Remote Features (LLEFS) support in HOST-GAP
- LE Channel Assessment (CHAS) Config HCI command support in HOST-GAP
- Sensor support enabled on KW43/MCXW70

### Improved
- CS procedure auto-loop shell command; improved RAS data drop mechanism
- Allow 'setnumprocs' 0 for infinite CS procedure repeats
- Synchronize code between loc_reader and ncp_loc_reader application files
- Subevent interval added to RADE algoritm API
- Updated connection handover setup info for FRDM-KW43 in CS application documentations
- Documentation updates

### Fixed
- L2CAP data fragmentation fix causing inccorect fragmentation over the air
- Fix for no valid subevents in localization
- Allow sub_mode_type 255 in CS config params
- Incorrect connection handle (0 instead of 0xFFFF) for non-connection enhanced notification events
- Channel Sounding subevent abort (No CS_SYNC mode0) after rebond
- Set 'preferredPeerAntenna' based on peer capabilities
- Miscellaneous minor application bug fixes

### Changed
- Reduced cyclomatic complexity (CCM) in multiple localization functions
- Decoupled buttons from LEDs in Bluetooth applications

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23, KW43, MCXW70


## [1.10.19] - mcux v2026-06-00-pvw2

### Added
- Support for LE Read All Remote Features after connection is established
- Experimental Inline PCT Transfer support for localization sample applications
- Support for configurable limits for concurrent Channel Sounding connections/procedures

### Improved
- Optimized memory allocation in isp_mciq_ranging_compute for localization applications
- Updated RAS loc_reader application for multiple CS connections
- Updated handover documentation with broadcast time synchronization details
- Documentation updates

### Fixed
- Fixed double free issue in L2ca_SendAclDataWithSegmentation
- Fixed consecutive CS procedures sustainability issue
- Miscellaneous minor application bug fixes

### Changed
- CS procedure state management: do not allow new procedure to start before previous one completes

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23, KW43


## [1.10.18] - mcux v2026-06-00-pvw1

### Added
- Support for RTT 0 m calibration in applications.
- Support for CS Enhancements at Host level: 
	- Inline PCT Transfer
	- RTT 2M PHY
- New connection event 'gConnEvtRemoteFeaturesRead_c', providing the peer feature bitmask read via the Read Remote Features procedure
- Support for 5 advertising sets in the Bluetooth LE Host
- Method to specify GATT handles on the client, avoiding repeated service discovery
- Slope calibration algorithm for CS (experimental, disabled by default). Computes distance and quality indicators per antenna path
- Handover broadcast time synchronization, allowing one connected anchor to synchronize multiple target anchors simultaneously
- Added common configuration header 'app_localization_config.h', overridable by the user

### Improved
- Updated handling of 'Procedure_Results_Start' to correctly process multiple subevents in a single message
- Improved CS temperature polling mechanism
- BTCS Server now explicitly notifies the PSM channel via 'AppLocalization_SetPsmChannelId' upon handover connection
- RAS/BTCS clients can disable algorithm execution via 'gRunAlgo_d'
- Updated and cleaned 'app_preinclude.h' for sample applications
- Documentation and configuration updates for LCE enable/disable
- Documentation updates
- Miscellaneous minor application bug fixes

### Fixed
- If the LE Set Periodic Adv Subevent Data command finishes with an error, use 'gInternalError_c' with 'gLeSetPeriodicAdvSubeventData_c' as the source and the command complete status as the error code
- Correct handling of AddrType values in MonAdvReport generated from XML

### Changed
- Removed the Bluetooth LE Host library from the wireless_uart_host project
- Disabled use of Random Static Address for all applications except: digital_key_car_anchor and digital_key_car_anchor_cs
- Set maximum CS procedure duration to: (procedure interval x connection interval x 2 - 1) slots

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23


## [1.10.17] - mcux v2026-03-00

### Added
- Test mode functionality for the Intrusion Detection System
- GAP API for HCI LE_Set_Local_Slave_Latency_Enable vendor command
- `gRasUseNotifOrInd_c` to allow configuration of Indication or Notification on RAS
- `cs_sync_phy` argument to `setcsconfig` shell command
- `snr_control_init` and `snr_control_refl` arguments to `setcsproc` shell command
- Detailed description for `setcsconfig` and `setcsproc` command arguments to the CCC w/ CS and Localization applications notes
- Mode0 step information parsing for both RAS and BTCS, made available for the export module

### Improved
- ATT Write Commands can now be sent at any time, as per specification
- Enhanced CCC and RAS Channel Sounding by incorporating real-time local and remote data decompression
- Localization applications now record the PHY used for establishing the connection and incorporate it into the CS procedure parameters configuration
- Localization applications keep the `subevtStopIdx` for the remote data and provide it to the algorithm
- Localization applications now pass remote mode map to algorithm
- Eliminated the dependency on `hci_interface.h` in localization files
- Documentation updates
- Miscellaneous minor application bug fixes

### Fixed
- Wrong data field type in `GAP2GenericEventVendorSpecificDebugFatalError`
- Localization application now processes CS results only when local data is available
- Digital key car anchor application issue related to setting the GAP role prior to any use
- Error in the definition of a CCC PSM VDBT Version GATT characteristic
- Issue with PACKET_NADM byte not being packed correctly into the TOF buffer for mode-3 steps
- MISRA fixes

### Changed
- Modified linker section names, comments, and RAM layout in the `connectivity.icf` and `connectivity_ble_loc.ld` files for every KW47 and MCXW72 board
- Set the default CS submode and default `rtt_type` to 3
- Set the default radio core speed for the Channel Sounding application to 48 MHz
- Localization applications configured to use the extended heap as the final segment of DATA1
- Deleted the `gcGapMaximumActiveConnections_c` definition from applications
- Eliminated unused fields related to CS Test Mode
- Deleted initializations of range settings that were subsequently overwritten

### Supported Platforms

- KW45, KW47, MCXW71, MCXW72, MCXW23

## [1.10.16] - mcux v2026-03-00-pvw2

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
