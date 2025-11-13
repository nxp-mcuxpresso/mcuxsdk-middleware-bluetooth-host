# Testing RAS and RAP using Profile Tuning Suite (PTS)

When testing RAS and RAP using the PTS, the loc_reader and loc_user_device need to be used.

The test code support for testing RAS and RAP against PTS is located in the files:
 - application/common/lcl/app_localization.c
 - application/common/lcl/pts_test_vectors.c
 - examples/loc_reader/loc_reader.c
 - examples/loc_user_device/loc_user_device.c

This configuration must be enabled in the app_preinclude.h of either of the examples:
 - gRasRapPtsTest_d is set to 1
 - gAppIsPeripheral_d is set to 1
 - gAppUsePrivacy_d is set to 0
 - gRandomStaticAddress_d is set to 0
 - gAppMaxConnections_c is set to 2 (any value higher than 1)

The testing method for RAS and RAP in PTS is based on a test vector approach, as PTS does not support Channel Sounding (CS).

## Usage

The PTS tool implements tests using test steps. These steps could vary from connects and disconnects, to generation and injection of CS data in the Implementation Under Test (IUT).

The test steps require user interaction with the IUT, as PTS will guide the user with actions necessary for the user to execute to complete the test.

Primarily, the user will use the:
 - `sb` command to start Bluetooth LE advertising for the PTS to initiate a connection to the IUT.
 - `dcnt` command to disconnect the IUT from the PTS.
 - `ptstest` command, followed by the `test_id` that is the id of the test running, usage example: `ptstest RAS/SR/RCO/BV-01-C`.
 - `filter` command set a filter. (used in reduced number of test)

A test could require no use of the `ptstest`, one use of the `ptstest` command or multiple uses of the `ptstest` command. When a test requires a user to perform several actions, `ptstest test_id` should be issued with the same test ID as the test being run. The test step logic can be found in the `loc_reader.c` and `loc_user_device.c`, under the `BleApp_RunPtsTest` function.
