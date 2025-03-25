# Host API Python Bindings


## Prerequisites

Python 3.10.x/32b is necessary to run the Python bindings. The bindings use the Host API C libraries. On Linux and OS X operating systems, these are called from the installation location which is `/usr/local/lib`, while on Windows OS the library file is loaded in `<Python Install Directory>\DLLs`.


## Platform setup

To run scripts from the command line, the PYTHONPATH must be set accordingly, so that the interpreter can find the imported modules.

**Linux OS**

Adding the source folder to the PYTHONPATH can be done by editing `~/.bashrc` and adding the following line:
```bash
export PYTHONPATH=\$PYTHONPATH:/home/.../hsdk-python/src
```

Most of the Python scripts operate on boards connected on a serial bus and superuser privileges must be employed to access the ports. After running a command prefixed with sudo, the environment paths become those of root, so the locally set PYTHONPATH is not visible anymore. That is why `/etc/sudoers` is modified to keep the environment variable when changing user.

Edit `/etc/sudoers` with your favorite text editor. Modify:
```
Defaults env_reset -> Defaults env_keep="PYTHONPATH"
```

As an alternative to avoid modifying the `:sudoers` file, the PYTHONPATH can be adjusted programmatically, as in the example below:
```py
import sys

if sys.platform.startswith('linux'):
    sys.path.append('/home/user/hsdk-python/src')
```

**Windows OS**

Add the source folder to the PYTHONPATH by following these steps:

1. Navigate to My Computer > Properties > Advanced System Settings > Environment Variables > System Variables.

2. Modify existing or create new variable named PYTHONPATH, with the absolute path of `tools\wireless\host_sdk\hsdk-python\src`.


## Directory Tree

```
lib                                         Compiled host SDK libraries for Windows.
├── README.md
├── x64
│   └── HSDK.dll
└── x86
    └── HSDK.dll
src
├── com
│   ├── nxp
│   │   ├── wireless_connectivity
│   │   │   ├── commands
│   │   │   │   ├── ble                     Generated files for Bluetooth LE support.
│   │   │   │   │   ├── ble_sig_defines.py
│   │   │   │   │   ├── enums.py
│   │   │   │   │   ├── events.py
│   │   │   │   │   ├── frames.py
│   │   │   │   │   ├── gatt_database_dynamic.py
│   │   │   │   │   ├── heart_rate_interface.py
│   │   │   │   │   ├── __init__.py
│   │   │   │   │   ├── operations.py
│   │   │   │   │   ├── spec.py
│   │   │   │   │   └── sync_requests.py
│   │   │   │   ├── comm.py
│   │   │   │   ├── firmware                Generated files for OTA/FSCI bootloader support.
│   │   │   │   │   ├── enums.py
│   │   │   │   │   ├── events.py
│   │   │   │   │   ├── frames.py
│   │   │   │   │   ├── __init__.py
│   │   │   │   │   ├── operations.py
│   │   │   │   │   ├── spec.py
│   │   │   │   │   └── sync_requests.py
│   │   │   │   ├── fsci_data_packet.py
│   │   │   │   ├── fsci_frame_description.py
│   │   │   │   ├── fsci_operation.py
│   │   │   │   ├── fsci_parameter.py
│   │   │   │   └── __init__.py
│   │   │   ├── hsdk
│   │   │   │   ├── CFsciLibrary.py
│   |   |   │   ├── config.py               Configuration file for the Python Host SDK subsystem.
│   │   │   │   ├── CUartLibrary.py
│   │   │   │   ├── device
│   │   │   │   │   ├── device_manager.py
│   │   │   │   │   ├── __init__.py
│   │   │   │   │   └── physical_device.py
│   │   │   │   ├── framing
│   │   │   │   │   ├── fsci_command.py
│   │   │   │   │   ├── fsci_framer.py
│   │   │   │   │   └── __init__.py
│   │   │   │   ├── __init__.py
│   │   │   │   ├── library_loader.py
│   │   │   │   ├── ota_server.py
│   │   │   │   ├── singleton.py
│   │   │   │   ├── sniffer.py
│   │   │   │   └── utils.py
│   │   │   ├── __init__.py
│   │   │   └── test                        Test and proof of concept scripts.
│   │   │       ├── hrs.py                  Script implementing a Bluetooth LE Heart Sensor profile .
│   │   │       └── __init__.py
│   │   └── __init__.py
│   └── __init__.py
└── __init__.py
```


## Functional description

The interaction between Python and the C libraries is made by the ctypes module. Ctypes provides C compatible data types, and allows calling functions in DLLs or shared libraries. It can be used to wrap these libraries in pure Python. Because the use of shared libraries is a requirement, the LIB_OPTION variable must remain set on "dynamic" in hsdk/Makefile. Ctypes made into mainline Python starting with version 2.5.

The Python Bindings expose Bluetooth LE familiar API in the `com.nxp.wireless_connectivity.commands` package. Such a package contains the following modules:
```
bluetooth le
├── enums.py            – Classes that resemble enums, constants are generated here.
├── events.py           – Observer classes that can build an object from a byte array and deliver it to the user.
├── frames.py           – Classes that map on Bluetooth LE/Firmware FSCI messages.
├── operations.py       – Each Operation class encapsulates a request and one or multiple events that are to be generated by the request.
├── spec.py             – This file describes the name, size, order and relationship between the command parameters.
└── sync_requests.py    – Each Synchronous Request is a method which sends a request and returns the triggered event.
```

Tests and examples are placed in the package `com.nxp.wireless_connectivity.test`.


## Bluetooth LE Heart Rate Service use case

The Heart Rate Service is presented as use case for using the API of a Bluetooth LE _black box_, located in the example `hsdk-python/src/com/nxp/wireless_connectivity/test/hrs.py`.

The example populates the GATT Database dynamically with the GATT, GAP, heart rate, battery and device information services. It then configures the Bluetooth LE stack and starts advertising. There are also two connect and disconnect observers to handle specific events.

The user needs to connect to the Bluetooth LE or hybrid _black box_ through a serial bus port that is passed as a command line argument, for example, `/dev/ttyACM0`.

```bash
# python hrs.py -h
usage: hrs.py [-h] [-p] serial_port

Bluetooth LE demo app which implements a ble_fsci_heart_rate_sensor.

positional arguments:
    serial_port Kinetis-W system device node.
optional arguments:
    -h, --help  show this help message and exit
    -p, --pair  Use pairing
```

It is important to first execute a CPU reset request to the Bluetooth LE _black box_ before performing any other configuration to reset the Buetooth LE stack. This is done by the following command:
```py
FSCICPUReset(serial_port, protocol=Protocol.BLE)
```

**User sync request example**

It is recommended for the user to access the Bluetooth LE API through sync requests. `GATTDBDynamicAddCharacteristicDeclarationAndValue` API is used as an example:
```py
def gattdb_dynamic_add_cdv(self, char_uuid, char_prop, maxval_len, initval, val_perm):
    '''
    Declare a characteristic and assign it a value.

    @param char_uuid: UUID of the characteristic
    @param char_prop: properties of the characteristic
    @param maxval_len: maximum length of the value
    @param initval: initial value
    @param val_perm: access permissions on the value
    @return: handle of the characteristic
    '''
    ind = GATTDBDynamicAddCharacteristicDeclarationAndValue(
        self.serial_port,
        UuidType=UuidType.Uuid16Bits,
        Uuid=char_uuid,
        CharacteristicProperties=char_prop,
        MaxValueLength=maxval_len,
        InitialValueLength=len(initval),
        InitialValue=initval,
        ValueAccessPermissions=val_perm,
        protocol=self.protocol
    )
    if ind is None:
        return self.gattdb_dynamic_add_cdv(char_uuid, char_prop, maxval_len, initval, val_perm)

    print '\tCharacteristic Handle for UUID 0x%04X ->' % char_uuid, ind.CharacteristicHandle
    self.handles[char_uuid] = ind.CharacteristicHandle
    return ind.CharacteristicHandle
```

**Sync request internal implementation**

As an example, for the `GATTDBDynamicAddCharacteristicDeclarationAndValue` API, the command is executed through a synchronous request. The sync request code creates an object of the following class:
```py
class GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(object):

    def __init__(self,
UuidType=GATTDBDynamicAddCharacteristicDeclarationAndValueRequestUuidType.Uuid16Bits, Uuid=[],
CharacteristicProperties=GATTDBDynamicAddCharacteristicDeclarationAndValueRequestCharacteristicProperties.gNone_c,
MaxValueLength=bytearray(2), InitialValueLength=bytearray(2), InitialValue=[],
ValueAccessPermissions=GATTDBDynamicAddCharacteristicDeclarationAndValueRequestValueAccessPermissions.gPermissionNone_c):
    '''
    @param UuidType: UUID type
    @param Uuid: UUID value
    @param CharacteristicProperties: Characteristic properties
    @param MaxValueLength: If the Characteristic Value length is variable, this is the maximum length; for fixed lengths this must be set to 0
    @param InitialValueLength: Value length at initialization; remains fixed if maxValueLength is set to 0, otherwise cannot be greater than maxValueLength
    @param InitialValue: Contains the initial value of the Characteristic
    @param ValueAccessPermissions: Access permissions for the value attribute
    '''
    self.UuidType = UuidType
    self.Uuid = Uuid
    self.CharacteristicProperties = CharacteristicProperties
    self.MaxValueLength = MaxValueLength
    self.InitialValueLength = InitialValueLength
    self.InitialValue = InitialValue
    self.ValueAccessPermissions = ValueAccessPermissions
```

An operation is represented by an object of the following class:
```py
class GATTDBDynamicAddCharacteristicDescriptorOperation(FsciOperation):

    def subscribeToEvents(self):
        self.spec = Spec.GATTDBDynamicAddCharacteristicDescriptorRequestFrame
        self.observers =[GATTDBDynamicAddCharacteristicDescriptorIndicationObserver('GATTDBDynamicAddCharacteristicDescriptorIndication'), ]
        super(GATTDBDynamicAddCharacteristicDescriptorOperation, self).subscribeToEvents()
```

The Spec object is initialized and set to zero in the FSCI packet any parameter not passed through the object of a class, depending on its length. Also, when defining such an object, the parameters may take simple integer, boolean or even list values instead of byte arrays, the values are serialized as a byte stream.

The observer is an object of the following class:
```py
class GATTDBDynamicAddCharacteristicDeclarationAndValueIndicationObserver(Observer):
    opGroup = Spec.GATTDBDynamicAddCharacteristicDeclarationAndValueIndicationFrame.opGroup
    opCode = Spec.GATTDBDynamicAddCharacteristicDeclarationAndValueIndicationFrame.opCode

    @overrides(Observer)
    def observeEvent(self, framer, event, callback, sync_request): 
    # Call super, print common information
    Observer.observeEvent(self, framer, event, callback, sync_request)
    #Get payload
    fsciFrame = cast(event, POINTER(FsciFrame))
    data = cast(fsciFrame.contents.data, POINTER(fsciFrame.contents.length * c_uint8))
    packet = Spec.GATTDBDynamicAddCharacteristicDeclarationAndValueIndicationFrame.getFsciPacketFromByteArray(data.contents, fsciFrame.contents.length)
    # Create frame object
    frame = GATTDBDynamicAddCharacteristicDeclarationAndValueIndication()
    frame.CharacteristicHandle = packet.getParamValueAsNumber("CharacteristicHandle")
    framer.event_queue.put(frame) if sync_request else None

    if callback is not None:
        callback(frame)
    else:
        print_event(self.deviceName, frame)
    fsciLibrary.DestroyFSCIFrame(event)
```

The status of the request is printed at the console by the following general status handler:
```py
def subscribe_to_async_ble_events_from(device, ack_policy=FsciAckPolicy.GLOBAL):
    ble_events = [
        L2CAPConfirmObserver('L2CAPConfirm'),
        GAPConfirmObserver('GAPConfirm'),
        GATTConfirmObserver('GATTConfirm'),
        GATTDBConfirmObserver('GATTDBConfirm'),
        GAPGenericEventInitializationCompleteIndicationObserver('GAPGenericEventInitializationCompleteIndication'),
        GAPAdvertisingEventCommandFailedIndicationObserver('GAPAdvertisingEventCommandFailedIndication'),
        GATTServerErrorIndicationObserver('GATTServerErrorIndication'),
        GATTServerCharacteristicCccdWrittenIndicationObserver('GATTServerCharacteristicCccdWrittenIndication')
    ]

    for ble_event in ble_events:
        FsciFramer(device, FsciAckPolicy.GLOBAL, Protocol.BLE, Baudrate.BR115200).addObserver(ble_event)
```

**Connect and disconnect observers**

The following code adds observers for the connect and disconnect events in the user class:
```py
class BLEDevice(object):
    '''
    Class which defines the actions performed on a generic Bluetooth LE device.
    Services implemented: GATT, GAP, Device Info.
    '''
    self.framer.addObserver(
        GAPConnectionEventConnectedIndicationObserver('GAPConnectionEventConnectedIndication'),
        self.cb_gap_conn_event_connected_cb)
    self.framer.addObserver(GAPConnectionEventDisconnectedIndicationObserver('GAPConnectionEventDisconnectedIndication'),
    self.cb_gap_conn_event_disconnected_cb)
```

where the callbacks are:
```py
def cb_gap_conn_event_connected_cb(self, event):
    '''
    Callback executed when a smartphone connects to this device.
    @param event: GAPConnectionEventConnectedIndication
    '''
    print_event(self.serial_port, event)
    self.client_device_id = event.DeviceId
    self.gap_event_connected.set()

def cb_gap_conn_event_disconnected_cb(self, event):
    '''
    Callback executed when a smartphone disconnects from this device.
    @param event: GAPConnectionEventdisConnectedIndication
    '''
    print_event(self.serial_port, event)
    self.gap_event_connected.clear()
```

From an Android™ or iOS-based smartphone, the user can use the Bluetooth LE Toolbox application in the Heart Rate profile. Random heart rate measurements in the range 60-100 are displayed every second, while battery values change every 10 seconds.


**Parent topic:** [Bluetooth Low Energy Host Stack FSCI Application Programming](../Bluetooth%20Low%20Energy%20Host%20Stack%20FSCI%20Application%20Programming.md)
