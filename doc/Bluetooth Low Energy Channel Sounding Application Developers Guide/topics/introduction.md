# Introduction

Channel Sounding (CS) is a Bluetooth Low Energy version 6.0 feature which, together with the Ranging Service (RAS)/Ranging Profile (RAP), enables two devices to securely estimate the distance between them.

The devices have the following roles:

- within GAP, one device is a Peripheral, the other is a Central.
- within CS, one device is the CS Initiator, the other is the CS Reflector.
- within RAS, one device is the Ranging Requester, the other is the Ranging Responder.

The role configurations above are described in detail in the following sections.

This document explains how to integrate CS and RAS/RAP functionality into an application and provides detailed explanation of the most commonly used APIs and code examples.