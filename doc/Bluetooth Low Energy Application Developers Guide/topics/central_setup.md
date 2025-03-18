# Central setup

The application configures the extended scanning by going through the following states:

1.  Start scanning by calling:

    ```
    bleResult_t Gap_StartScanning
    (
    const gapScanningParameters_t* pScanningParameters,
    gapScanningCallback_t scanningCallback,
    gapFilterDuplicates_t enableFilterDuplicates,
    uint16_t duration,
    uint16_t period
    )
    ```

    When using the common application structure, the application can use the following API defined in *app\_conn.h*:

    ```
    bleResult_t BluetoothLEHost_StartScanning
    (
        appScanningParams_t   *pAppScanParams,
        gapScanningCallback_t pfCallback
    );
    ```

    The API starts scanning using the given parameters, which must have the following structure:

    ```
    typedef struct appScanningParams_tag
    {
        gapScanningParameters_t *pHostScanParams;         /*!< Pointer to host scan structure */
        gapFilterDuplicates_t enableDuplicateFiltering;        /*!< Duplicate filtering mode */
        uint16_t duration;                                                           /*!< scan duration  */
        uint16_t period;                                                               /*!< scan period  */
    } appScanningParams_t;
    ```

    Application may use the default set of parameters *gGapDefaultExtScanningParameters\_d*. If the *pScanningParameters* pointer is NULL, the latest set of parameters are used. The *scanningPHYs* parameter indicates the PHYs on which the advertising packets should be received on the primary advertising channel. As a result, permitted values for the parameter are 0x01 \(scan LE 1M\), 0x04 \(scan LE Coded\) and 0x05 \(scan both LE 1M and LE Coded\). There are no strict timing rules for scanning, yet if both PHYs are enabled for scanning, the scan interval value must be large enough to accommodate two scan windows \(interval \>= 2 \* window\).

    If the advertiser uses legacy advertising PDUs, the device may actively scan by sending a SCAN\_REQ PDU to the advertiser on the LE 1M primary advertising channel \(no secondary channel in legacy advertising\). Respectively, if the advertiser uses extended advertising PDUs, the active scan operation takes place on the secondary advertising channel. After the device receives a scannable ADV\_EXT\_IND PDU on the primary advertising channel \(PHY LE 1M or Coded\), it starts listening for the AUX\_ADV\_IND PDU on the secondary advertising channel \(PHY 1M, 2M or Coded\). Once received, the device sends an AUX\_SCAN\_REQ to the advertiser. Next, an AUX\_SCAN\_RSP PDU should be received, containing the scan response data. Application should wait for a *gScanStateChanged\_c* or a *gScanCommandFailed\_c*in the Scanning Callback.

2.  Collect information by waiting for *gDeviceScanned\_c* \(legacy advertising PDUs\) or *gExtDeviceScanned\_c* \(extended advertising PDUs\) event in the Scanning Callback. The *gExtDeviceScanned\_c* event contains additional information pertaining to the extended received PDU, such as: primary PHY, secondary PHY, advertising SID, interval of the periodic advertising if enabled in the set.

    When using the common application structure, the application can use the following API defined in *app\_conn.h*, to search the contents from *pData* in an advertising element:

    ```
    bool_t BluetoothLEHost_MatchDataInAdvElementList
    (
        gapAdStructure_t *pElement,
        void             *pData,
        uint8_t          iDataLen
    );
    ```

3.  Stop scanning by calling the function below:

    ```
    bleResult_t Gap_StopScanning(void);
    ```

    Application should wait for a *gScanStateChanged\_c* or a *gScanCommandFailed\_c* in the Scanning Callback.

4.  Connect to a device by calling the function below:

    ```
    bleResult_t Gap_Connect
    (
    const gapConnectionRequestParameters_t* pParameters,
    gapConnectionCallback_t connCallback
    );
    ```

    When using the common application structure, the following API can be used:

    ```
    bleResult_t BluetoothLEHost_Connect
    (
        gapConnectionRequestParameters_t*   pParameters,
        gapConnectionCallback_t             connCallback
    );
    ```

    The *initiatingPHYs* parameter indicates the PHYs on which the advertising packets should be received on the primary advertising channel and the PHYs for which connection parameters have been specified. The parameter is a bitmask of PHYs: BIT0 = LE 1M, BIT1 = LE 2M and BIT2 = LE Coded. The Host may enable one or more initiating PHYs, but it must at least set one bit for a PHY allowed for scanning on the primary advertising channel, i.e., BIT0 for LE 1M PHY or BIT2 for LE Coded PHY.

    If the advertiser uses legacy advertising PDUs, the device may connect by sending a CONNECT\_IND PDU to the advertiser on the LE 1M primary advertising channel \(no secondary channel in legacy advertising\). On the other hand, if the advertiser uses extended advertising PDUs, the extended connect operation takes place on the secondary advertising channel. After the device receives a connectable ADV\_EXT\_IND PDU on the primary advertising channel \(PHY LE 1M or Coded\), it starts listening for the connectable AUX\_ADV\_IND PDU on the secondary advertising channel \(PHY 1M, 2M or Coded\). Once received, the device sends an AUX\_CONNECT\_REQ to the advertiser. Next, if AUX\_CONNECT\_RSP PDU is received, the device enters the Connection State in the Central role on the secondary advertising channel PHY.

    Application should wait for a *gConnEvtConnected\_c* event in the Connection Callback. If the channel selection algorithm \#2 is used for this connection, then a *gConnEvtChanSelectionAlgorithm2\_c* event is also generated.

    After the connection is successfully established, the application may choose to read the connection PHY by calling the *Gap\_LeReadPhy* API. It may also opt to change the PHY of the connection by triggering a PHY Update Procedure using the *Gap\_LeSetPhy API*. However, the Controller might not be able to perform the change if, in case the peer does not support the new requested PHY.


**Parent topic:**[Extended advertising](../topics/extended_advertising.md)

