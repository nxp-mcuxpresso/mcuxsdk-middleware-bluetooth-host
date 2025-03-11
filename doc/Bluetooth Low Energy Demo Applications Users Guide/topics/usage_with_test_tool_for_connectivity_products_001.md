# Usage with Test Tool for connectivity products

The Bluetooth LE FSCI Black Box demo application is designed to be used via serial interface. This can be done using the TEST Tool for Connectivity Products – Command Console application as described below.

1.  Download the demo application onto a supported board.
2.  Connect the board to a USB port of the PC. The UASB COM port drivers must be installed properly and a COM port corresponding to the board should be available.
3.  Open the Test Tool application and connect to the serial port corresponding to the board on which the Bluetooth LE FSCI Black Box application runs. See [Figure 1](#fir3g3r22234). The serial communication parameters are: baud rate 115200, 8N1, and no flow control.

    ![](../images/image_25_1.png "Test tool command console serial port
                                                selection")

4.  Select the appropriate Test Tool XML file from the drop-down list for the release being used and send commands to the application. An example is shown in [Figure 2](#fig_9451593r22234).

    ![](../images/image_25_2.png "FSCI black box command example")


**Parent topic:**[Bluetooth LE FSCI Black Box](../topics/bluetooth_le_fsci_black_box.md)

