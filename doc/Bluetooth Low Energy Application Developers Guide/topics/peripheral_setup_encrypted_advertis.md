# Peripheral Setup

Use the `Gap_EncryptAdvertisingData` API to obtain the encrypted advertising data, which can then be placed inside the “Encrypted Advertising Data” \(0x31\) AD type.

```
bleResult_t Gap_EncryptAdvertisingData 
  ( const gapAdvertisingData_t *pAdvertisingData, const uint8_t *pKey, const uint8_t *pIV, uint8_t *pOutput )
```

**Parent topic:**[Encrypted Advertising Data](../topics/encrypted_advertising_data_0.md)

