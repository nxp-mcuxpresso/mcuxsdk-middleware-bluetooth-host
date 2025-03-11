# Enabling DBAF

To enable DBAF, modify the Digital Key applications as per the steps listed below:

-   \(Required only for KW45B41Z-EVK\): Flash the `kw45b41_nbu_ble_xp_hosted` NBU found in `middleware/wireless/ble_controller/bin/experimental/`.
-   Update the application project so that it uses the experimental Bluetooth LE Host library path: `middleware/wireless/bluetooth/host/lib_exp/`:
    -   Use `lib_ble_OPT_host_cm33_iar.a` for the Digital Key Car Anchor application.
    -   Use `lib_ble_OPT_host_central_cm33_iar.a` for the Digital Key Device application.
-   In `app_preinclude.h` file, set `gBLE60_DecisionBasedAdvertisingFilteringSupport_d` to `TRUE`.
-   Make the following updates in the `digital_key_device.c` \(for the Digital Key Device application\) and in `app_digital_key_car_anchor.c` \(for the Digital Key Car Anchor application\):

    ```
    extern uint8_t gHostInitExpmFeatures;
    #define gExpmDecisionBasedAdvertisingFilteringBit_d BIT1;
    ```

-   In the function `BluetoothLEHost_AppInit()`, add the below line prior to the `BluetoothLEHost_Init()` function call:

    ```
    gHostInitExpmFeatures |= gExpmDecisionBasedAdvertisingFilteringBit_d;
    ```


**Parent topic:**[Running Passive Entry Scenario with Decision Based Advertising Filtering \(DBAF\)](../topics/running_passive_entry_scenario_with_decision_based.md)

