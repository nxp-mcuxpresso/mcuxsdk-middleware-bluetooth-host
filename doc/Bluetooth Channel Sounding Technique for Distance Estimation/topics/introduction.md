# Introduction

The low-power 2.4-GHz ISM band wireless standards, such as Bluetooth 2 The channel sounding standard Low Energy (BLE), have broad adoption in consumer products such as smartphones, tablets, smartwatches, or edge nodes in Internet of Things (IoT) networks. As the application domains of the BLE technology expand into industrial and automotive markets, their mass adoption and omni-presence makes this technology usable for applications that go beyond data transfer between devices. The radios included in these products can be used for more complex use cases, such as distance estimation, angle (of arrival/departure) computations, and localization applications. They span the joint estimation of both the angle and the distance when used with specialized hardware enabled as anchors.

This document describes the distance measurement between 2 Bluetooth LE devices using NXP MCUs. This solution only applies to Bluetooth LE devices. This document refers to the KW47 MCU and its associated SDK. The SDK also contains the Wireless Localization middleware, which uses a special distance-estimation technique called Channel Sounding (CS). By enabling the CS feature between Bluetooth devices, a measured channel response can be used to calculate the position of a Bluetooth device in a high accuracy mode (at a distance of 5 m with an accuracy of ± 0.5 m in ideal conditions). Channel sounding verifies whether a device is in a predefined area, which makes beacons easier to install and improves indoor localization. Deriving a distance estimate using CS requires 1 device to initiate the process (initiator) and a 2nd device to reciprocate the transmission (reflector). The Measuring Device (MD) and Reflector Device (RD) terms are also used. This solution does not require multiple antennas (but can benefit from their presence) to compute the distance between 2 nodes. There are many aspects to consider when creating a localization solution that meets unique requirements. This application note highlights the aspects that impact performance with a focus on how to use NXP enablement to create your own solution. This document also refers to the following documents for localization purposes:

-   Channel Sounding Wireless Ranging Demo Application
-   KW47 Ranging Performance Measurement
-   KW47-LOC Board User Manual

This application note describes a method to estimate the distance partly based on Round-Trip Phase (RTP) measurements. The legacy name of the RTP is Phase-based Distance Estimation (PDE). The phase of RF signals is proportional to the frequency of the carrier and the distance traveled. The method is based on measuring the received signals’ phases relative to their local oscillators. The measurements are then performed using tone signals at different frequencies and combined for a final distance estimation.

A part of the description incorporates a method of measuring the Round-Trip Time (RTT) of the RF signal. The legacy name of the RTT is the Time of Flight (ToF) distance estimation. This measurement has lower accuracy when compared to the phase measurement. However, it has an inherent advantage of adding security. The security aspect is described as well.

```{include} ../topics/requirements.md
:heading-offset: 1
```
