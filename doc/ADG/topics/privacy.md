# Privacy

To enable or disable Privacy, the following APIs may be used:

```
bleResult_t 
 BleConnManager_EnablePrivacy(void);
```

```
bleResult_t 
 BleConnManager_DisablePrivacy(void);
      
```

The function BleConnManager\_EnablePrivacy calls *BleConnManager\_ManagePrivacyInternal* after checking if the privacy is enabled.

```
 static bleResult_t 
 BleConnManager_ManagePrivacyInternal        
 (bool_t bCheckNewBond);
        
```

If the privacy feature is supported \(*gAppUsePrivacy\_d = 1*\), the Connection Manager activates Controller Privacy or Host Privacy depending on the board capabilities.

The *bCheckNewBond* is a boolean that tells the Manager whether it should check or not if a bond between the devices already exists.

In order to update the identity information after a bond is added or removed privacy should be disabled and enabled. For pairing with bonding this is done automatically in *ble\_conn\_manager*. In case the application adds or removes a bond through the GAP API, it should also disable and enable privacy.

**Parent topic:**[Bluetooth LE Connection Manager](../topics/bluetooth_le_connection_manager.md)

