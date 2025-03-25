# Windows OS Host Software Installation Guide


## Libraries

**Prerequisites**

Microsoft Visual Studio® is required to build the host software. Open the solution file ConnectivityLibrary.sln and build it for either Win32 or x64, depending on your setup requirements. The output directory contains a file named HSDK.dll, which can be thought of as a bundle of all the shared libraries from Linux OS, except for SPI (libspi.so). Currently, SPI interface to the board is not supported by the Windows host software.

Prebuilt HSDK.dll files are available under directory hsdk-python\lib.

**Installation**

The host software for the Windows OS is designed to work in a Python environment by contrast to the Linux OS where standalone C demos also exist.

Download and install the Python 3.10.x/32b package from [Python Downloads](https://www.python.org/downloads/). When customizing the installation options, check Add python.exe to Path.

**Using prebuilt library**

1. Depending on your Python environment architure (not Windows architecture) copy the appropriate HSDK.dll from `hsdk-python\lib\[x86|x64]`to `\<Python Directory\>\DLLs`.

2. Download and install Visual C++ Redistributable Packages for Microsoft Visual Studio, depending on the Windows architecture of your system (vcredist_x86.exe or vcredist_x64.exe) from [Microsoft Visual C++ Redistributable latest supported downloads](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170).

3. Download and install the Microsoft Visual C++ Compiler for Python as described in [the python Windows Complilers](https://wiki.python.org/moin/WindowsCompilers).

**Using local built library**

1. Depending on your Python environment architure (not Windows architecture), build the appropriate Microsoft Visual Studio solution configuration and then copy HSDK.dll to `\<Python Directory\>\DLLs`.

2. Download and install the Microsoft Visual C++ Compiler for Python as described in [the python Windows Complilers](https://wiki.python.org/moin/WindowsCompilers).

Optionally, copy the `hsdk\res\hsdk.conf` to `\<Python Directory\>\DLLs` to control the behavior of the FSCI-ACK synchronization mechanism.


## Demos

See [Host API Python Bindings](../topics/host_api_python_bindings.md).


**Parent topic:** [Bluetooth Low Energy Host Stack FSCI Application Programming](../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md)
