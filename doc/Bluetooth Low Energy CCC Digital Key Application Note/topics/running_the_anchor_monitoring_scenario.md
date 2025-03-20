# Running the Anchor Monitoring scenario

Anchor Monitoring is an NXP proprietary feature that allows a second Anchor to monitor the connection between the first Anchor and the Device. The prerequisites are the same hardware setup as used for the Connection Handover scenario. Note, however, that the Connection Handover scenario cannot be performed while Anchor Monitoring is in progress.

**Demo steps:**

1.  Connect a Car Anchor and a Device using either the Owner Pairing or Passive Entry scenario as described in [Owner Pairing Scenario](owner_pairing_scenario.md) and [Passive Entry Scenario](passive_entry_scenario.md).

**Anchor has performed Owner Pairing and the “monitor start” command is run**

![Figure 1](../images/monitor_start_anc1.png "Anchor has performed Owner Pairing and the “monitor start” command is run")

2.  Run the “`monitor 0 start`” shell command on the Car Anchor, as seen in [Figure 1](../images/monitor_start_anc1.png).

**The second Anchor has received the command over the serial interface and has started Anchor Monitoring**

![Figure 2](../images/monitor_start_anc2.png "The second Anchor has received the command over the serial interface and has started Anchor Monitoring")

3.  The second Car Anchor begins monitoring the connection and receives RSSI info events, which it forwards to the first Car Anchor (see [Figure 2](../images/monitor_start_anc2.png )).

**The first Anchor displays RSSI events received from the second Anchor.**

![Figure 3](../images/monitor_rssi_evt.png "The first Anchor displays RSSI events received from the second Anchor.")

4.  The received RSSI messages are displayed in the console (see [Figure 3](../images/monitor_rssi_evt.png)).
5.  To stop monitoring, run the “`monitor 0 stop`” shell command on either Car Anchor \(the second one is recommended, as its console is not flooded by RSSI event messages\).






**Parent topic:**[Running the Connection Handover scenario](../topics/running_the_connection_handover_scenario.md)

