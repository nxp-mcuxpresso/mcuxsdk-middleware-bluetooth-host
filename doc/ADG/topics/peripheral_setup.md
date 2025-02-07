# Peripheral Setup

1.  First set the extended advertising parameters using Gap\_SetExtAdvertisingParameters. The extended advertising type must be set to non-connectable and non-scannable.
2.  Set the periodic advertising parameters using the same handle as in the previous command.

    ```
    bleResult_t **Gap\_SetPeriodicAdvParameters**
    (
        gapPeriodicAdvParameters_t*    pAdvertisingParameters
    );
    ```

    Wait for a *gPeriodicAdvParamSetupComplete\_c*event in the generic callback.

3.  Next, set the periodic advertising data by calling:

    ```
    bleResult_t **Gap\_SetPeriodicAdvertisingData**
    (
        uint8_t                      handle,
        gapAdvertisingData_t*  pAdvertisingData,
        bool_t bUpdateDID
    );
    ```

    *pAdvertisingData* cannot be NULL. If periodic advertising data must be empty, set *cNumAdStructures* to 0. Wait for a *gPeriodicAdvDataSetupComplete\_c*event in the generic callback.

4.  Start extended advertising using Gap\_StartExtAdvertising.
5.  Last, enable Periodic Advertising. Periodic advertising starts only after extended advertising is started.

    ```
    bleResult_t **Gap\_StartPeriodicAdvertising**
    (
        uint8_t handle,
        bool_t  bIncludeADI
    );
    ```

    Wait for a *gPeriodicAdvertisingStateChanged\_c*event in the advertising callback.


**Parent topic:**[Periodic Advertising](../topics/periodic_advertising.md)

