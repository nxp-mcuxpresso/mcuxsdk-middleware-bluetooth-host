# Time synchronization

The Time Synchronization procedure enables precise timing alignment between the Source and Target devices for the Connection Handover and Anchor/Packet monitoring processes. The timing information obtained is further required for the Anchor Search procedure.

To start the time synchronization procedure, one of the devices must call `GAP_HandoverTimeSyncReceive()` and the other device must call `GAP_HandoverTimeSyncTransmit()`. Either device, Source or Target, can initiate the procedure. The Connection Handover application common module initiates the procedure on the Source device by notifying the Target device to call the `Gap_HandoverTimeSyncReceive()` function.

**Time Synchronization steps**

Time synchronization steps for two devices where the Target device starts the procedure:
1. Target device application calls `GAP_HandoverTimeSyncReceive()` with the `stopWhenFound` parameter set to 'gTimeSyncStopWhenFound_c'.
2. Target device receives the 'gHandoverTimeSyncReceiveComplete_c' event indicating that Source Link Layer is waiting for the time synchronization event.
3. Source device application calls `GAP_HandoverTimeSyncTransmit()`
4. Source device receives the 'gHandoverTimeSyncTransmitStateChanged_c' event indication that time synchronization transmission is enabled.
5. The Target device receives the 'gHandoverTimeSyncEvent_c' event containing the timing information.
6. The Target device computes the 'Gap_HandoverAnchorSearchStart' 'timingDiffSlot' and 'timingDiffOffset' parameters using the timing information received in the `gHandoverTimeSyncEvent_c` event as shown below:
   'searchParams.timingDiffSlot = mSlotLocal - mSlotRemote'
   'searchParams.timingDiffOffset = mOffsetLocal - mOffsetRemote;'

**Time Synchronization signaling chart**
![](../images/TimeSync_Individual.png "Time Synchronization overview")
