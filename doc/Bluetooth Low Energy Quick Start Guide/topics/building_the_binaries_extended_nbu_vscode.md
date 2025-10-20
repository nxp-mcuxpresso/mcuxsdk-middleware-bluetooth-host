# Building and flashing the BLE Extended NBU software demo applications using Visual Studio Code

To build and flash the BLE software demo applications using Visual Studio Code, follow the steps listed below:

1.  Open Visual Studio Code and open the MCUXpresso for Visual Studio Code extension as shown in the figure below.

    **MCUXpresso for Visual Studio Code extension**
    ![MCUXpresso for Visual Studio Code extension](../images/image39.png "MCUXpresso for Visual Studio Code extension")

2.  Import the SDK package: click “**Import Repository**”. Then, choose the “**Local**” option \(if the SDK is archived use “**Local archive**”\), browse to the path of the SDK you want, and click “**Import**”.

    **Steps to import the SDK**
    ![Steps to import the SDK](../images/MCXW72_I/Figure22_mcxw72.png "Steps to import the SDK")

3.  Import NCP(Core1) application: After the SDK is loaded successfully, select the “**Import Example from Repository**” to add an application to your workspace. Choose the repository, toolchain \(Arm GNU\), board, example you want to add, and the location where the VS Code project would be created. Then click “**Import**”.

    **Steps to import the NCP example application**
    ![Steps to import the example application](../images/MCXW72_I/Figure23_mcxw72_core1.png "Steps to import the example application")

4.  Import Core0 application: After the SDK is loaded successfully, select the “**Import Example from Repository**” to add an application to your workspace. Choose the repository, toolchain \(Arm GNU\), board, example you want to add, and the location where the VS Code project would be created. Then click “**Import**”.

    **Steps to import the Core0 example application**
    ![Steps to import the example application](../images/MCXW72_I/Figure23_mcxw72_core0.png "Steps to import the example application")

5.  Both Extended NBU applications now appears in the “**Projects**” tab on the left. Build both applications by pressing the “**Build selected**” button on each.

    **Building the NCP FSCI Black Box ThreadX application**
    ![Building the Wireless UART Host FreeRTOS application](../images/MCXW72_I/Figure24_mcxw72_core1.png "Building the Wireless UART Host FreeRTOS application")

    **Building the Wireless UART Host Host FreeRTOS application**
    ![Building the Wireless UART Host FreeRTOS application](../images/MCXW72_I/Figure24_mcxw72_core0.png "Building the Wireless UART Host FreeRTOS application")

6.  After the build is completed successfully, first debug the NBU(Core1) application by clicking the “**Debug**” button.

    **Debug the NCP FSCI Black Box ThreadX application**
    ![Debug the Wireless UART Host FreeRTOS application](../images/MCXW72_I/Figure25_mcxw72_core1.png "Debug the Wireless UART Host FreeRTOS application")

7.  Press the run \(“**Continue**”\) button to run the application on the board.

    **Running the NCP FSCI Black Box ThreadX application**
    ![Running the NCP FSCI Black Box ThreadX application](../images/MCXW72_I/Figure26_mcxw72_core1.png "Running the NCP FSCI Black Box ThreadX application")

8.  After the build is completed successfully and the NBU application was successfully written, debug the Core0 application by clicking the “**Debug**” button.

    **Debug the Wireless UART Host FreeRTOS application**
    ![Debug the Wireless UART Host FreeRTOS application](../images/MCXW72_I/Figure25_mcxw72_core0.png "Debug the Wireless UART Host FreeRTOS application")

9.  Press the run \(“**Continue**”\) button to run the application on the board.

    **Running the Wireless UART Host FreeRTOS application**
    ![Running the Wireless UART Host FreeRTOS application](../images/MCXW72_I/Figure26_mcxw72_core0.png "Running the Wireless UART Host FreeRTOS application")


**Parent topic:**[Building the binaries](../topics/building_the_binaries.md)

