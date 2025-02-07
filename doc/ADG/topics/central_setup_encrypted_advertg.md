# Central Setup

Use the `Gap_DecryptAdvertisingData` API to decrypt the contents of “Encrypted Advertising Data” \(0x31\) AD types included in scanned data.

```
bleResult_t Gap_DecryptAdvertisingData 
    (uint8_t *pData, uint16_t dataLength, const uint8_t *pKey, const uint8_t *pIV, uint8_t *pOutput)
```

**Parent topic:**[Encrypted Advertising Data](../topics/encrypted_advertising_data_0.md)

