# Peripheral Setup

1. First set the extended advertising parameters using `Gap_SetExtAdvertisingParameters`. The extended advertising type must be set to non-connectable and non-scannable.

2. Set the periodic advertising parameters with the same handle as in the previous command. Use the `Gap_SetPeriodicAdvParametersV2` command. Compared to `Gap_SetPeriodicAdvParameters`, this command also configures parameters relevant to PAwR, such as the number of subevents and response slots as well as timing information.

```
bleResult_t Gap_SetPeriodicAdvParametersV2 
   (gapPeriodicAdvParametersV2_t* pAdvertisingParameters);
```

Wait for a `gPeriodicAdvParamSetupComplete_c` event in the generic callback.

3. Start extended advertising using `Gap_StartExtAdvertising`.

4. Start periodic advertising using `Gap_StartPeriodicAdvertising`.

5. Wait for `gPerAdvSubeventDataRequest_c` events. These events are used by the Controller to indicate that it is ready to transmit one or more subevents and it is requesting the advertising data for these subevents. Upon receiving an event, use `Gap_SetPeriodicAdvSubeventData` to set the advertising data for specific subevents.

```
bleResult_t Gap_SetPeriodicAdvSubeventData 
      (uint8_t advHandle, const gapPeriodicAdvertisingSubeventData_t* pData);
```

Wait for the `gPeriodicAdvSetSubeventDataComplete_c` event in the generic callback.

6. Wait for `gPerAdvResponse_c` events. These events contain responses sent by devices who are synchronized to the periodic advertising. They include data in the AD format.

7. Optionally, PAwR allows the advertising device to initiate a connection to one of the synchronized scanners. The connection can be initiated by calling `Gap_ConnectFromPawr`.

```
bleResult_t Gap_ConnectFromPawr 
      (const gapConnectionFromPawrParameters_t* pParameters, gapConnectionCallback_t connCallback);
```

**Parent topic:**[Periodic Advertising with Responses \(PAwR\)](../topics/periodic_advertizing_with_responses.md)

