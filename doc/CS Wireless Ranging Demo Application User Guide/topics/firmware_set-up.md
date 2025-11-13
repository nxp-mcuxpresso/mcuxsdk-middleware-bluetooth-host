# Firmware setup

To install the firmware on the boards, perform the steps below:

**Note:** The firmware must be installed on the two boards following those two steps.

1.  Update your NBU image with the provided binaries in the following folder of the SDK: `../middleware/wireless/ble_controller/bin` (\*\_hadm\_\* for KW45 family).
2.  Compile the application `wireless_ranging_bluetooth_bm` by referring to the quick start guide specific to your platform. The application can be found in the directory:

    `\\boards\<board>\wireless_examples\bluetooth\wireless_ranging`

3.  Flash the binary on both devices. Flash the application using your preferred IDE.


**Parent topic:**[Wireless ranging demo application setup](../topics/wireless_ranging_demo_application_setup.md)

