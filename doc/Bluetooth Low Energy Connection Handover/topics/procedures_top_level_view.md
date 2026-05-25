# Procedures - Top level view

The Connection Handover and Anchor/Packet Monitoring process comprises several procedures as illustrated below.

**Connection Handover**

![](../images/Handover_Top.png "Connection Handover Procedure Top Level View")

**Connection Handover (Monitoring Active)**

When Anchor or Packet Monitoring is already active on the Target device, the Connection Handover can be performed directly without Time Synchronization or Anchor Search. The application triggers this flow by calling `AppHandover_StartConnectionHandover()`. The process skips directly to Context Synchronization and uses the existing monitoring state for the handover connect.

**Anchor/Packet monitoring**

![](../images/AnchorPacket_Monitoring_Top.png "Anchor/Packet Monitoring Procedure Top Level View")