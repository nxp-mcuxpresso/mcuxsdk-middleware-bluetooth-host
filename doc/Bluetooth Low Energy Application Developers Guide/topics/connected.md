# Connected

When a device connects while Controller Privacy is enabled, the *gConnEvtConnected\_c* connection event parameter structure contains more relevant fields than without Controller Privacy.

The *peerRpaResolved* field equals TRUE if the peer was using an RPA that was resolved using an IRK from the list. In that case, the *peerAddressType* and *peerAddress*fields contain the identity address of the resolved device, and the actual RPA used to create the connection \(the RPA that a Central used when initiating the connection, or the RPA that the Peripheral advertised with\) is contained by the *peerRpa* field.

The *localRpaUsed*field equals TRUE if the local Controller was automatically generating an RPA when the connection was created, and the actual RPA is contained by the *localRpa* field.

**Parent topic:**[Controller privacy](../topics/controller_privacy.md)

