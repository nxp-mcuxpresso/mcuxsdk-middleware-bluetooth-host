# Introduction

The application presented in this document relies on channel sounding \(CS\) Bluetooth Low Energy implementation as defined by the Bluetooth Special Interest Group \(SIG\).

CS protocol performs secure and highly accurate distance estimation between two Bluetooth Low Energy devices. CS supports two different measurement modes: RTT and RTP. RTP is used for an accurate distance estimation and RTT to get a coarse but secure distance bound.

For more details about CS techniques, see *Bluetooth Channel Sounding Technique for Distance Estimation* \(document AN13481\).

The application relies on `LE_CS_Create_Config`, `LE_CS_Set_Procedure_Parameters`, and `LE_CS_Set_Procedure_Enable` HCI commands to perform CS measurements. CS measurement results are sent through host controller interface \(HCI\) using `LE_CS_Subevent_Result_Continue` and `LE_CS_Subevent_Result_Continue` HCI events. They are centralized on the client board by the wireless ranging application to be combined and passed to various distance computation algorithms, being embedded, or PC-based.

See [Acronyms](glossary.md) for details on which part of the standard is supported.

Wireless ranging application allows to:

-   Configure most of the parameters required for a CS measurement \(number of steps, type of steps, CS channels, CS timings \(T\_FCS, T\_IP, T\_PM\), and so on\)
-   Select what type of measurement to be performed \(RTT, RTP, or both\) based on configured CS step modes \(1 or 2\)
-   Initiate CS setup phase \(exchange of capabilities, configuration, and so on\)
-   Trigger CS measurements
-   Retrieve CS measurement results \(RTP, RTT, and other debug information\) from peer device over Bluetooth Low Energy connection
-   Compute distance estimations by combining RTT and RTP measurements from both devices
-   Log system debug information but also raw IQ data information in Matlab \(\*.mat\), Excel \(\*.xls\), or numpy \(\*.npz\) format that can be used for further analysis or development of distance computation algorithms

Wireless ranging application is made of two parts:

-   Embedded firmware that can be controlled manually via a serial connection
-   Host application \(Python\) running on a PC and controlling the firmware using a serial link

The embedded firmware is in charge of:

-   Initiate CS setup \(CS capabilities, CS security\)
-   Handling CS configuration and measurement requests from the host application
-   Synchronizing initiator and reflector devices
-   Scheduling and executing CS measurements over the air
-   Generate pseudo random sequences for channels, AAs, and so on, using CS DRBG
-   Collecting measurement results from one device to the other
-   Send back CS measurement results to the host application
-   Perform distance computation using embedded algorithms \(namely CDE and RADE\)

While the host application:

-   Automates measurement requests
-   Logs measurement results in output files
-   Performs two ways channel reconstruction from captured PCT data and runs the high-resolution phase-based ranging algorithm \(RADE\)

