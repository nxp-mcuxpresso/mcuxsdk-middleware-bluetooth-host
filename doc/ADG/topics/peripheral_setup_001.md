# Peripheral setup

This section describes the extended advertising GAP API. The application should not use both the extended and legacy API \(described in section 4.2.1\). If this requirement cannot be met, the application should at least wait for the generated events in the Advertising Callback prior to using the other API. That is, it is advisable to call legacy functions only after the event pertaining to an extended API is received, and vice versa. This GAP constraint can be considered an extension of the HCI constraint from the Bluetooth 5 specification: "A Host should not issue legacy commands to a Controller that supports the LE Feature \(Extended Advertising\)".

The application configures extended advertising by going through the following states:

1.  Set the extended advertising parameters by calling:

    ```
    bleResult_t **Gap\_SetExtAdvertisingParameters**
    (
    gapExtAdvertisingParameters_t* pAdvertisingParameters
    );
    ```

    It may use the default set of parameters *gGapDefaultExtAdvertisingParameters\_d*. The application should wait for a *gExtAdvertisingParametersSetupComplete\_c event* in the Generic Callback. Only one advertising set can be configured at a time. Comparing with the legacy *Gap\_SetAdvertisingParameters* command, the new set of parameters is as follows.

    |Parameter|Description|
    |---------|-----------|
    |SID|Value of the Advertising SID subfield in the ADI field of the PDU.|
    |handle|Used to identify an advertising set. Possible values are 0x00 or 0x01 since the current implementation supports two advertising sets.|
    |extAdvProperties|BIT0 - Connectable advertising

BIT1 - Scannable advertising

BIT2 - Directed advertising

BIT3 - High Duty Cycle Directed Connectable advertising \(≤3.75 ms Advertising Interval\)

BIT4 - Use legacy advertising PDUs

BIT5 - Omit advertiser's address from all PDUs

\("anonymous advertising"\)

BIT6 - Include TxPower in the extended header of the advertising PDU.

If legacy advertising PDU types are being used \(BIT4 = 1\),

permitted properties values are presented in the next table. If the advertising set already contains data, the type shall be one that supports advertising data and the amount of data shall not exceed 31 octets.

If extended advertising PDU types are being used \(BIT4 =0\), then the advertisement shall not be both connectable and scannable. While high duty cycle directed connectable advertising \(≤ 3.75 ms advertising interval\) shall not be used \(BIT3 = 0\).

|
    |txPower|Maximum power level at which the advertising packets are to be transmitted, the Controller can choose any power level <= txPower. Value 127 to be used if Host has no preference.|
    |primaryPHY|PHY for ADV\_EXT\_IND: LE 1 M or LE Coded|
    |secondaryPHY|PHY for AUX\_ADV\_IND and periodic advertising: LE 1 M, LE 2 M or LE Coded. Ignored for legacy advertising|
    |secondaryAdvMaxSkip|Maximum advertising events that the Controller can skip before sending the AUX\_ADV\_IND packets on the secondary advertising channel. Higher values may result in lower power consumption. Ignored for legacy advertising|
    |enableScanReqNotification|Whether to enable notifications when scanning PDUs \(SCAN\_REQ, AUX\_SCAN\_REQ\) are received. If enabled, the application is notified upon scan requests by gExtScanNotification\_c events in the Advertising Callback|

    When using LE Coded PHY for advertising, the default coding scheme chosen by link layer is S=8 \(125 kb/s data rate\). To change the default coding scheme, the user has two options:

    -   At compile time by defining *mLongRangeAdvCodingScheme\_c*, or
    -   At run time by calling the API *Controller\_ConfigureAdvCodingScheme\(\)*.
    In both cases, the value of the define or the parameter of the API has to be an appropriate value for primary and secondary PHYs as defined by the enumeration *advCodingScheme\_tag* found in *controller\_interface.h*.

    |EventType|PDU Type|Advertising Event Properties|
    |---------|--------|----------------------------|
    |Connectable and scannable undirected|ADV\_IND|00010011b|
    |Connectable directed \(low duty cycle\)

|ADV\_DIRECT\_IND|00010101b|
    |Connectable directed \(high duty cycle\)

|ADV\_DIRECT\_IND|00011101b|
    |Scannable undirected|ADV\_SCAN\_IND|00010010b|
    |Non-connectable and

Nonscannable undirected

|ADV\_NONCONN\_IND|00010000b|

2.  Set the advertising data and/or scan response data by calling:

    ```
    bleResult_t **Gap\_SetExtAdvertisingData**
    (
    uint8_t handle,
    gapAdvertisingData_t* pAdvertisingData,
    gapScanResponseData_t* pScanResponseData
    );
    ```

    Either of the pAdvertisingData or pScanResponseData parameters can be NULL, but not both. For extended advertising \(BIT4 = 0\) only one must be different than NULL – the scannable advertising bit \(BIT1\) indicates whether pAdvertisingData \(BIT1 = 0\) orpScanResponseData \(BIT1 = 1\) is accepted. The total amount of Advertising Data shall not exceed 1650 bytes. Application should wait for a *gExtAdvertisingDataSetupComplete\_c* event in the Generic Callback.

3.  Enable extended advertising by calling:

    ```
    bleResult_t **Gap\_StartExtAdvertising**
    (
    gapAdvertisingCallback_t advertisingCallback,
    gapConnectionCallback_t connectionCallback,
    uint8_t handle,
    uint16_t duration,
    uint8_t maxExtAdvEvents
    );
    ```

    When using the common application structure, the application can use the following API defined in *app\_conn.h*:

    ```
    bleResult_t **BluetoothLEHost\_StartExtAdvertising**
    (
        appExtAdvertisingParams_t *pExtAdvParams,
        gapAdvertisingCallback_t  pfAdvertisingCallback,
        gapConnectionCallback_t   pfConnectionCallback
    );
    ```

    The API goes through the steps of setting the advertising data and parameters. Events from the Host task are treated in the *App\_AdvertiserHandler\(\)* function, implemented in *app\_advertiser.c*. To set the extended advertising parameters and data *BluetoothLEHost\_StartExtAdvertising* a parameter of the following type:

    ```
    typedef struct **appExtAdvertisingParams\_tag**
    {
        gapExtAdvertisingParameters_t *pGapExtAdvParams;
        gapAdvertisingData_t *pGapAdvData;
        gapScanResponseData_t *pScanResponseData;
        uint8_t                     handle;
        uint16_t                    duration;
        uint8_t                     maxExtAdvEvents;
    } appExtAdvertisingParams_t;
    ```

    Advertising may be enabled for each previously configured advertising set, identified by the handle parameter. If duration is set to 0, advertising continues until the Host disables it, otherwise advertising is only enabled for this period \(multiple of 10 ms\). *maxExtAdvEvents* represent the maximum number of extended advertising events the Controller shall attempt to send prior to terminating the extended advertising, ignored if set to 0. Application should wait for a *gExtAdvertisingStateChanged\_c* or a *gAdvertisingCommandFailed\_c* event in the Advertising Callback.

4.  Disable advertising by calling:

    ```
    bleResult_t **Gap\_StopExtAdvertising**
    (
    uint8_t handle
    );
    ```

    Application should wait for a *gExtAdvertisingStateChanged\_c* or a *gAdvertisingCommandFailed\_c* event in the Advertising Callback.

5.  Remove the advertising set by calling:

    ```
    bleResult_t** Gap\_RemoveAdvSet**
    (
    uint8_t handle
    );
    ```

    Application should wait for a *gExtAdvertisingSetRemoveComplete\_c* event in the Generic Callback.


**Parent topic:**[Extended advertising](../topics/extended_advertising.md)

