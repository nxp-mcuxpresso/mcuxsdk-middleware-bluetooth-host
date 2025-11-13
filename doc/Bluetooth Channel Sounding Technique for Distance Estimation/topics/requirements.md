# Bluetooth Channel Sounding high-level requirements

This chapter provides several aspects of CS requirements for localization demands. The distance-measurement accuracy is divided into 2 categories. The measured channel response can be used to calculate the position of a Bluetooth device with the high accuracy mode - defined at a distance of 5 m with an accuracy of ± 0.5 m.

The CS also defines conditions for non-stationary devices with the maximum relative speed between the initiator and the target. The maximum absolute speed of either device is less than or equal to 5 km/h (typical walking speed). The device should be capable to make at least 2 distance measurements per second to realize real-time distance estimation to an object moving at a speed of 5 km/h.

The immunity to relay attacks is also part of the CS specification. The mitigation of both packet and extender relay attacks is at least as cryptographically strong as a link layer encryption. The link layer security must be enabled as a prerequisite for channel-sounding operations. The protection against symbol manipulation attacks is covered by the resiliency to the Early Detect Late Commit (EDLC) and symbol attacks on the RTT packets. This is provided by symbol sharpening and shortening.

Another requirement defines the attack detection in terms of distance estimation. The CS should detect an attack in which the distance measurement is spoofed by more than:
-   3 m for distances of up to 15 m
-   20 % of the total distance for distances greater than 15 m
