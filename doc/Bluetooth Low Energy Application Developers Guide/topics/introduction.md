# Introduction

This document explains how to integrate the NXP Bluetooth Low Energy Host Stack in an application and provides detailed explanation of the most commonly used APIs and code examples.

-   [Introduction](introduction.md): This section outlines the document structure.
-   [Prerequisites](prerequisites.md): The document sets out the prerequisites.
-   [Bluetooth LE Host Stack Initialization and APIs](bluetooth_le_host_stack_initialization_and_apis.md): This section describes the Bluetooth Low Energy Host Stack initialization. It also presents the APIs categorized according to the layer and by application role.
-   [Generic Access Profile \(GAP\) Layer](generic_access_profile_gap_layer.md): The Generic Access Profile \(GAP\) layer is divided into two sections according to the GAP role of the device: Central and Peripheral. The basic setup of two such devices is explained with code examples, such as how to prepare the devices for connections, how to connect them together, and pairing and bonding processes.
-   [Generic Attribute Profile \(GATT\) Layer](generic_attribute_profile_gatt_layer.md): This section describes the Generic Attribute Profile \(GATT\) layer and introduces the APIs required for data transfer between the two connected devices. This section is divided into two subsections according to the GATT role of the device: Client and Server.
-   [GATT database application interface](gatt_database_application_interface.md): The document further describes the usage of the GATT database APIs that allow the application to manipulate data stored in the GATT Server database.
-   [Creating GATT database](creating_gatt_database.md): This section describes a user-friendly method to build a GATT database statically. The method involves the use of a predefined set of macros that the application can include to build the database at application compile time.
-   [Creating a Custom Profile](creating_a_custom_profile.md): This section contains instructions on how to build a custom profile.
-   [Application Structure](application_structure.md): The section describes the structure of the typical application.
-   [Low-Power Management](low-power_management.md): This section describes low-power management and how an application can use the low-power modes of the hardware and software.
-   [Over the Air Programming \(OTAP\)](over_the_air_programming_otap.md): This section describes the Over The Air Programming \(OTAP\) capabilities that the Host Stack offers via a dedicated Service/Profile. The section also describes how to use the OTAP capabilities in an application and also contains a detailed description of the SDK components involved in the OTAP process.
-   [Creating a Bluetooth LE application when the Host Stack runs on another processor](creating_a_bluetooth_low_energy_application_when_t.md): This section describes how to build a Bluetooth Low Energy application when the Host Stack is running on a separate processor.
-   [Intrusion Detection System](ids.md): This section describes the usage of the Intrusion Detection System (IDS).
-   [References](reference_documentation.md): This section lists the documents that can be referred to for more information.
-   [Acronyms and abbreviations](Acronyms.md): This section lists the acronyms used in this document.

