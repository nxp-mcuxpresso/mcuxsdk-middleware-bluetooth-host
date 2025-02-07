# Scanning

The most basic setup for a Central device begins with scanning, which is performed by the following function from *gap\_interface.h*:

```
bleResult_t **Gap\_StartScanning**
(
  const gapScanningParameters_t* pScanningParameters,
  gapScanningCallback_t scanningCallback,
  gapFilterDuplicates_t enableFilterDuplicates,
  uint16_t duration,
  uint16_t period
);
```

If the *pScanningParameters* pointer is NULL, the currently set parameters are used. If no parameters have been set after a device power-up, the standard default values are used:

```
**\#define** gGapDefaultScanningParameters_d \
{ \
    /* type */             gGapScanTypePassive_c, \
    /* interval */         gGapScanIntervalDefault_d, \
    /* window */           gGapScanWindowDefault_d, \
    /* ownAddressType */   gBleAddrTypePublic_c, \
    /* filterPolicy */     gScanAll_c \
    /* scanning PHY */     gLePhylMFlag_c\
}
```

The easiest way to define non-default scanning parameters is to initialize a *gapScanningParameters\_t* structure with the above default and change only the required fields.

For example, to perform active scanning and only scan for devices in the Filter Accept List, the following code can be used:

```
gapScanningParameters_t scanningParameters = gGapDefaultScanningParameters_d;
scanningParameters.type = gGapScanTypeActive_c;
scanningParameters.filterPolicy = gScanWithFilterAcceptList_c;
Gap_StartScanning(&scanningParamters, scanningCallback, enableFilterDuplicates, duration, period);
```

When using the common application structure, the application can use the following API defined in *app\_conn.h*:

```
bleResult_t **BluetoothLEHost\_StartScanning**
(
    appScanningParams_t   *pAppScanParams,
    gapScanningCallback_t pfCallback
);
```

The API uses the *appScanningParams\_t* structures, which is defined as follows:

```
typedef struct appScanningParams_tag
{
    gapScanningParameters_t *pHostScanParams;         /*!< Pointer to host scan structure */
    gapFilterDuplicates_t enableDuplicateFiltering;   /*!< Duplicate filtering mode */
    uint16_t duration;                                /*!< scan duration  */
    uint16_t period;                                  /*!< scan period  */
} appScanningParams_t;
```

The *scanningCallback* is triggered by the GAP layer to signal events related to scanning.

The most important event is the *gDeviceScanned\_c* event, which is triggered each time an advertising device is scanned. This event data contains information about the advertiser:

```
typedef struct
{
    bleAddressType_t                  addressType ;
    bleDeviceAddress_t                aAddress ;
    int8_t                            rssi ;
    uint8_t                           dataLength ;
    uint8_t*                          data ;
    bleAdvertisingReportEventType_t   advEventType ;
    bool_t                            directRpaUsed;
    bleDeviceAddress_t                directRpa;
    bool_t                            advertisingAddressResolved;
} gapScannedDevice_t;
```

If this information signals a known Peripheral that the Central wants to connect to, the latter must stop scanning and connect to the Peripheral.

To stop scanning, call this function:

```
bleResult_t **Gap\_StopScanning** (**void**);
```

By default, the GAP layer is configured to report all scanned devices to the application using the *gDeviceScanned\_c* event type. However, some use cases might require to perform specific GAP Discovery Procedures. In such use cases the advertising reports might require the filtering of Flags AD value from the advertising data. Other use cases require the Bluetooth LE Host Stack to automatically initiate a connection when a specific device has been scanned.

To enable filtering based on the Flags AD value or to set device addresses for automatic connections, the following function must be called before the scanning is started:

```
bleResult_t **Gap\_SetScanMode**
(
    gapScanMode_t           scanMode,
    gapAutoConnectParams_t* pAutoConnectParams,
    gapConnectionCallback_t connCallback
);
```

The default value for the scan mode is *gDefaultScan\_c,* which reports all packets regardless of their content and does not perform any automatic connection.

To enable Limited Discovery, the *gLimitedDiscovery\_c* value must be used, while the *gGeneralDiscovery\_c* value activates General Discovery.

To enable automatic connection when specific devices are scanned, the *gAutoConnect\_c* value must be set, in which case the *pAutoConnectParams* parameter must point to the structure that holds the target device addresses and the connection parameters to be used by the Host for these devices.

If `scanMode` is set to *gAutoConnect\_c*, connCallback must be set and is triggered by GAP to send the events related to the connection.

**Parent topic:**[Central setup](../topics/central_setup_001.md)

