# Linux OS Host Software Installation Guide


## Libraries

**Prerequisites**

Packages: build-essential, libudev, libudev-dev, libpcap, libpcap-dev. Use apt-get install on Debian-based distributions. The Linux OS kernel version must be greater than 3.2.

**Installation**

```bash
$ pwd
/home/user/hsdk
$ make
$ find build/ -name "\*.so"
build/libframer.so
build/libsys.so
build/libphysical.so
build/libuart.so
build/libfsci.so
build/libspi.so
$ sudo make install
```

By default, make generates `sharedlibraries` (having `.so` extension). The step make install (superuser privileges required) copies these libraries to /usr/local/lib, which is part of the default Linux OS library path. The installation prefix may be changed by passing the variable PREFIX, e.g. make install PREFIX=/usr/lib. The user is responsible for making sure that PREFIX is part of the system's LD_LIBRARY_PATH.

Static libraries can be generated instead, by modifying the LIB_OPTION variable in the Makefile from `dynamic` to `static` (.a extension).

`make install` also disables the [ModemManager](http://www.freedesktop.org/wiki/Software/ModemManager/) control for the connected devices. Otherwise, the daemon starts sending AT commands that affect the responsiveness of the afore-mentioned devices in the first 20 seconds after plug in.


## Demos

**Installation**

```bash
$ pwd
/home/user/hsdk/demo
$ make
$ ls bin/
GetKinetisDevices
```

These demos are provided in this package:
- GetKinetisDevices: this program detects the boards connected to the serial line and outputs the path to the console:
    ```bash
    $ ./GetKinetisDevices
    NXP Kinetis-W device on /dev/ttyACM0.
    NXP Kinetis-W device on /dev/ttyACM1.
    ```

**Parent topic:** [Bluetooth Low Energy Host Stack FSCI Application Programming](../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md)
