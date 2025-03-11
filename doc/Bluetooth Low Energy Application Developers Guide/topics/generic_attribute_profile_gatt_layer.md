# Generic Attribute Profile \(GATT\) Layer

The GATT layer contains the APIs for discovering services and characteristics and transferring data between devices and is built on top of the Attribute Protocol \(ATT\).

The Attribute Protocol \(ATT\) transfers data between Bluetooth Low Energy devices on a dedicated L2CAP channel \(channel ID 0x04\).

As soon as a connection is established between devices, the GATT APIs are readily available. No initialization is required because the L2CAP channel is automatically created.

To identify the GATT peer instance, the same *deviceId* value from the GAP layer \(obtained in the *gConnEvtConnected\_c*connection event\) is used.

There are two GATT roles that define the two devices exchanging data over ATT:

-   GATT Server – the device that contains a GATT Database, which is a collection of services and characteristics exposing meaningful data. Usually, the Server responds to *requests* and *commands* sent by the Client. However, it can be configured to send data on its own through *notifications*and *indications*.
-   GATT Client – the “active” device that usually sends *requests* and *commands* to the Server to *discover* Services and Characteristics on the Server’s Database and to exchange data.

There is no fixed rule deciding which device is the Client and which one is the Server. Any device may initiate a request at any moment. Therefore, it temporarily acts as a Client, at which the peer device may respond, provided it has the Server support and a GATT Database.

Often, a GAP Central acts as a GATT Client to discover Services and Characteristics and obtain data from the GAP Peripheral, which usually has a GATT database. Many standard Bluetooth Low Energy profiles assume that the Peripheral has a database and must act as a Server. However, this is by no means a general rule.


```{include} ../topics/client_apis.md
:heading-offset: 1
```

```{include} ../topics/server_apis.md
:heading-offset: 1
```

