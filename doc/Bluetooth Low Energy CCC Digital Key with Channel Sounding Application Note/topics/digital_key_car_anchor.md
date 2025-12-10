(examples__wireless_examples__bluetooth__digital_key_car_anchor_cs_docs)=
# Digital Key Car Anchor application

The Digital Key Car Anchor demo application implements one of the multiple Bluetooth Low Energy Anchors that can reside inside the car. It is a Bluetooth Low Energy peripheral. Depending on the scenario, this peripheral can perform both Legacy \(1M PHY\) and Extended Long Range \(500 kilobits per sec Coded PHY\) advertising, searching for a CCC Digital Key-enabled device such as a smartphone.

There can be multiple Anchors inside a car. The Anchors must act as a single device and share information between them such as addresses, bonding data, or application-specific keys. The information must be shared such that a smartphone can connect to any one of the Anchors, depending on positioning and not detect any difference.

For the purposes of localization, the Digital Key Car Anchor acts either as a Channel Sounding Initiator or Reflector and as a Server/Client in the context of the Bluetooth LE Channel Sounding (BTCS). The Digital Key Car Anchor runs the localization algorithm using the local and remote distance measurement data.

**Parent topic:**[CCC Digital Key applications overview](../topics/ccc_digital_key_applications_overview.md)

