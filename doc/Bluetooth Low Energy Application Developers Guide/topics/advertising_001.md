# Advertising

Before starting advertising, the advertising parameters should be configured. Otherwise, the following defaults are used.

```
#define gGapDefaultAdvertisingParameters_d \
{
   /* minInterval */     gGapAdvertisingIntervalDefault_c, \
   /* maxInterval */     gGapAdvertisingIntervalDefault_c, \
   /* advertisingType */ gConnectableUndirectedAdv_c, \
   /* addressType */     gBleAddrTypePublic_c, \
   /* peerAddressType */  gBleAddrTypePublic_c, \
   /* peerAddress */     {0U, 0U, 0U, 0U, 0U, 0U}, \
   /* channelMap */      (gapAdvertisingChannelMapFlags_t)gGapAdvertisingChannelMapDefault_c,  \
   /* filterPolicy */    gProcessAll_c \
}
```

To set different advertising parameters, a *gapAdvertisingParameters\_t* structure should be allocated and initialized with defaults. Then, the necessary fields may be modified.

After that, the following function should be called:

```
bleResult_t Gap_SetAdvertisingParameters
(
    const gapAdvertisingParameters_t *     pAdvertisingParameters
);
```

The application should listen to the *gAdvertisingParametersSetupComplete\_c* generic event.

Next, the advertising data should be configured and, if the advertising type supports active scanning, the scan response data should also be configured. If either of these is not configured, they are defaulted to empty data.

The function used to configure the advertising and/or scan response data is shown here:

```
bleResult_t Gap_SetAdvertisingData
(
  const gapAdvertisingData_t *     pAdvertisingData,
  const gapScanResponseData_t *    pScanResponseData
);
```

Either of the two pointers may be *NULL*, in which case they are ignored \(the corresponding data is left as it was previously configured, or empty if it has never been set\), but not both at the same time.

The application should listen to the *gAdvertisingDataSetupComplete\_c* generic event.

After all the necessary setup is done, advertising may be started with this function:

```
bleResult_t Gap_StartAdvertising
(
    gapAdvertisingCallback_t advertisingCallback,
    gapConnectionCallback_t connectionCallback
);
```

The advertising callback is used to receive advertising events \(advertising state changed or advertising command failed\), while the connection callback is only used if a connection is established during advertising.

The connection callback is the same as the callback used by the Central when calling the *Gap\_Connect* function.

When using the common application structure, the application can use the following API defined in *app\_conn.h*:

```
bleResult_t BluetoothLEHost_StartAdvertising
(
    appAdvertisingParams_t   *pAdvParams,
    gapAdvertisingCallback_t pfAdvertisingCallback,
    gapConnectionCallback_t  pfConnectionCallback
);
```

The API goes through the steps of setting the advertising data and parameters. Events from the Host task are treated in the *App\_AdvertiserHandler\(\)* function, implemented in *app\_advertiser.c*. To set the advertising parameters and data *BluetoothLEHost\_StartAdvertising* requires a parameter of the following type:

```
typedef struct 
appAdvertisingParams_tag 
{ 
 gapAdvertisingParameters_t *pGapAdvParams; /*!< Pointer to the GAP advertising parameters */
 const gapAdvertisingData_t *pGapAdvData; /*!< Pointer to the GAP advertising data */
 const gapScanResponseData_t *pScanResponseData; 
 /*!< Pointer to the scan response data */ } appAdvertisingParams_t;
```

If a Central initiates a connection to this Peripheral, the *gConnEvtConnected\_c* connection event is triggered.

To stop advertising while the Peripheral has not yet received any connection requests, use this function:

```
bleResult_t Gap_StopAdvertising (void);
```

This function should not be called after the Peripheral enters a connection, as the advertising automatically stops in this case.

**Parent topic:**[Peripheral setup](../topics/peripheral_setup_002.md)

