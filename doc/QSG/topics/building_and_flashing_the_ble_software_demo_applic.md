# Building and flashing the BLE software demo applications using IAR Embedded Workbench

Use the following steps in order to build and flash the BLE software demo applications using the IAR Embedded Workbench:

1.  First unpack the contents of the archive to a folder on the local disk. Then, navigate to the resulting location starting from the SDK root directory.

2.  Open the IAR workspace file \(`*.eww` file format\) highlighted file in [Figure 1](#fig111).

    ![](../images/Rev1.1_figure4.png "Wireless UART IAR demo project location")

3.  Choose between Debug and Release configurations in the drop-down selector above the project tree in the workspace, as seen in [Figure 2](#fig_396da1fa-2e82-49f2-9cc2-3c07552c4c0f).

    ![](../images/Rev1.1_figure5.png "Select the desired
                            configuration (Debug or Release)")

    [Figure 3](#004) shows the Wireless UART - IAR workspace.

    ![](../images/Rev1.1_figure6.png "Wireless UART - IAR workspace")

4.  Build the Wireless UART project using the options shown in [Figure 4](#fig_c01a0c2d-8da1-42ff-8da5-182c5d5f55bd).

    ![](../images/MCXW72_I/Figure8.png "Build Wireless UART
                            application")

5.  Make the appropriate debugger settings in the project options window, as seen in [Figure 5](#fig_cf06f31e-1fb3-482f-b980-41522d06733c).

    **Project \> Options \(Alt+F7\) \> Debugger \> Setup \(tab\) \> Driver \> J-Link/J-Trace**

    ![](../images/MCXW72_I/Figure9.png "Debugger Settings for
                            the Wireless UART project")

6.  Click the “**Download and Debug**” button \(or **CTRL+D**\) to flash the executable onto the board, as seen in [Figure 6](#fig_9393b5f9-99a1-4e48-9821-f4237f7d7558).

    ![](../images/image6.png "Download and Debug the
                            Wireless UART application")

7.  Press **Go** \(**F5**\). At this moment, the board starts running the application as shown in [Figure 7](#fig_a3w_4nf_2zb).

    ![](../images/MCXW72_I/Figure11.png "Running the code on IAR")


**Parent topic:**[Building the binaries](../topics/building_the_binaries.md)

