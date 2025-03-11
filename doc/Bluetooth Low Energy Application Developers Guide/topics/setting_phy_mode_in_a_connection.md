# Setting PHY mode in a connection

In Bluetooth LE 5.0, the user is able to change the PHY mode in a connection through the Link Layer PHY Update Procedure and choose between default 1 Mbit/s, 2 Mbit/s high data rate or the coded S2 or S8 PHYs with 500 Kbps or 125 Kbps for longer range.

To set the PHY, the user can call:

```
bleResult_t Gap_LeSetPhy
(
    bool_t       defaultMode,
    deviceId_t   deviceId,
    uint8_t      allPhys,
    uint8_t      txPhys,
    uint8_t      rxPhys,
    uint16_t     phyOptions
);
```

There are two modes to use this API:

1.  If defaultMode is set to TRUE, the user can call this function without being in a connection, i.e. provide a device ID. The PHY option is used by the Link Layer in the PHY response when a connection is created and the peer device initiates the PHY Update Procedure. The application should listen for gLePhyEvent\_c with the gPhySetDefaultComplete\_c sub event type for the confirmation of the operation.
2.  If defaultMode is set to FALSE, the user must also provide a valid device ID. The Host asks the Link Layer to initiate the PHY Update Procedure with the peer device using the provided parameters.

The application should listen for gLePhyEvent\_c with the gPhyUpdateComplete\_c sub event type for the confirmation of the update procedure to have ended. The result of the operation populates in the txPhy and rxPhy of the event. The result is from the negotiation of the local parameters and the peer PHY preferences.

To read the current PHY on a connection, call the following API:

```
bleResult_t Gap_LeReadPhy
(
    deviceId_t deviceId
);
```

The application should listen for gLePhyEvent\_c with the gPhyRead\_c sub event type for the confirmation of the operation. The txPhy and rxPhy indicate the current modes used in the connection.

**Parent topic:**[Generic Access Profile \(GAP\) Layer](../topics/generic_access_profile_gap_layer.md)

