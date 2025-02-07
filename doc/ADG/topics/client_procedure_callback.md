# Client procedure callback

All the procedures initiated by a Client are asynchronous. They rely on exchanging ATT packets over the air.

To be informed of the procedure completion, the application must install a callback with the following signature:

```
**typedef ****void** (* gattClientProcedureCallback_t )
(
    deviceId_t             deviceId,
    gattProcedureType_t    procedureType,
    gattProcedureResult_t  procedureResult,
    bleResult_t            error
);
```

For EATT, the following signature should be used:

```
typedef void (*gattClientEnhancedProcedureCallback_t)
(
   deviceId_t deviceId,
   bearerId_t bearerId,
   gattProcedureType_t procedureType,
   gattProcedureResult_t procedureResult,
   bleResult_t error
);
```

To install this callback, the following function must be called:

```
bleResult_t **GattClient\_RegisterProcedureCallback**
(
    gattClientProcedureCallback_t callback
);
```

The EATT procedure callback should be installed using the following API:

```
bleResult_t **GattClient\_RegisterEnhancedProcedureCallback**
(
 gattClientEnhancedProcedureCallback_t callback
);
```

The *procedureType* parameter can be used to identify the procedure that was started and has reached completion. Only one procedure would be active at a given moment. Trying to start another procedure while a procedure is already in progress returns the error *gGattAnotherProcedureInProgress\_c*.

The *procedureResult* parameter indicates whether the procedure completes successfully or an error occurs. In the latter case, the *error* parameter contains the error code.

```
**void ****gatt ClientProcedureCallback**
(
    deviceId_t             deviceId,
    gattProcedureType_t    procedureType,
    gattProcedureResult_t  procedureResult,
    bleResult_t            error
)
{
    **switch** (procedureType)
    {
        /* ... */
    }
}
GattClient_RegisterProcedureCallback(gattClientProcedureCallback);
```

**Parent topic:**[Installing client callbacks](../topics/installing_client_callbacks.md)

