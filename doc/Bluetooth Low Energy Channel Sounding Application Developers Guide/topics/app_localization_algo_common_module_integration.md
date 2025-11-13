# Integration

The app_localization_algo is only required on the application that has the role of Ranging Requester and computes the actual distance. Perform the following steps to integrate app_localization_algo common module:

1. Add the following component to the application project via Kconfig:
   - `CONFIG_MCUX_COMPONENT_middleware.wireless.lcl_algo=y`
2. Pass a callback of type `pfAppDisplayResult_t` as the third parameter of `AppLocalization_Init`. This callback receives the distance measurement result, which the application displays.
3. Call the function `AppLocalizationAlgo_ResetPeer` upon peer disconnection to clean up algorithm-related data structures.
4. There is no other direct interaction between the application and this module. Its APIs are mostly used by the app_localization module.