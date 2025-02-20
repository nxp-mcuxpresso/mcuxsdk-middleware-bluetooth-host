# Running the Packet Monitoring scenario

Packet Monitoring is an NXP proprietary feature that allows a second Anchor to receive the packets from the connection between the first Anchor and the Device. The prerequisites are the same hardware setup as used for the [Running the Connection Handover scenario](running_the_connection_handover_scenario.md). Note, however, that the Connection Handover scenario cannot be performed while Packet Monitoring is in progress. Packets received by the second Anchor are sent over the serial interface to the first Anchor for processing. The first Anchor determines the source of the packet based on the SN and NESN bits and displays the RSSI.

**Demo steps**:

1.  Connect the first Anchor and a Device using either the Owner Pairing or Passive Entry scenario as described in [Owner Pairing Scenario](owner_pairing_scenario.md) and [Passive Entry Scenario](passive_entry_scenario.md).  

    ![](../images/packetmonStartAnchor1.png "Anchor has performed Owner Pairing and the “packetmon start” command
                                is run ")

    Anchor has performed Owner Pairing and the “`packetmon start`” command is run.

2.  Run “`packetmon start`” on the first Anchor, as shown in [Figure 1](#fig_kcm_ddn_11c).

    ![](../images/packetmonStartedAcnhor2.png "The second Anchor has received the command over the serial interface
                                and has started Packet Monitoring ")

3.  The second Anchor begins monitoring the connection and receives packet information, which it forwards to the first Anchor. See [Figure 2](#fig_kxy_ldn_11c).

    ![](../images/packetmonStartedAnchor1.png "The first Anchor displays the RSSI and source device of the
                                    received packets from the second Anchor
                            ")



4.  The first Anchor receives the packet information from the second Anchor and determines the source of the packet based on the SN and NESN bits. The source and the RSSI are displayed in the console. See [Figure 3](#fig_ysr_gdn_11c).
5.  To stop monitoring, run “`packetmon stop`” shell command on either Anchor \(the second one is recommended, as its console is not flooded by packet information messages\).

**Parent topic:**[Running the Connection Handover scenario](../topics/running_the_connection_handover_scenario.md)

