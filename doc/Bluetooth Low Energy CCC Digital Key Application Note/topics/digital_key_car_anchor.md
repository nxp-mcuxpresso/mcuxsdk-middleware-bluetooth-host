(examples__wireless_examples__bluetooth__digital_key_car_anchor_docs)=
# Digital Key Car Anchor

The Digital Key Car Anchor demo application implements one of the multiple Bluetooth Low Energy anchors that can reside inside the car. The anchor performs a dual role. Inside the CCC Digital Key scope, it is a Bluetooth Low Energy peripheral which, depending on the scenario, can perform both Legacy \(1M PHY\) and Extended Long Range \(500 kbps Coded PHY\) advertising, searching for a CCC Digital Key-enabled device such as a smartphone. Simultaneously it can also act as a Bluetooth Low Energy central, scanning for non-CCC key fobs. There can be multiple anchors inside a car. The anchors must act as a single device and share information between them \(addresses, bonding data, application specific keys, and such data\) such that a smartphone can connect to any one of them depending on positioning and not detect any difference.

**Parent topic:**[CCC Digital Key applications overview](../topics/ccc_digital_key_applications_overview.md)

