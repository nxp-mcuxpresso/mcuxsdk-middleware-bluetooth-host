# GATT client interactions

The client side of the service, which includes the service discovery, notification configuration, attribute reads and others are left to be handled by the application. The application calls the GATT client APIs and reacts accordingly. The only exception for this rule is that the service interface declares the client configuration structure. This structure usually contains the service handle and the handles of all the characteristic values and descriptors discovered. Additionally it can contain values that the client can use to interact with the server. For the Temperature Service client, the structure is as follows:

```
/*! Temperature Client - Configuration */
typedef struct tmcConfig_tag
{
    uint16_t                 hService;
    uint16_t                 hTemperature ;
    uint16_t                 hTempCccd ;
    uint16_t                 hTempDesc ;
    gattDbCharPresFormat_t   tempFormat ;
} tmcConfig_t ;
```

**Parent topic:**[Creating a Custom Profile](../topics/creating_a_custom_profile.md)

