# Intrusion Detection System

## Overview

The Intrusion Detection System (IDS) provides a comprehensive security monitoring framework for Bluetooth Low Energy applications. This interface defines various security events that can be detected across different layers of the Bluetooth LE stack and provides mechanisms to register callbacks and handle security incidents.

## Event Types

The `idsEventType_t` enumeration defines various security events that can be detected:

### Pairing-Related Events

 `gSecEvt_Sweyntooth_MaxEncKeySize_c` (BIT0)
- **Attack**: Sweyntooth - Max Encryption Key Size
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Attacker sends a Pairing Request with a value for the Encryption Key Size field which exceeds the maximum allowed (16 bytes), hoping to later trigger a buffer overflow on the victim device.
- **Preventive Action**: Sanity check the fields of received SMP messages.

`gSecEvt_DhKeyCheckZero_c` (BIT1)
- **Attack**: Deviating Behavior - DHKey Check Zero
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, the attacker sends a DHKey Check message with all fields set to zero, hoping to bypass this step of the process.
- **Preventive Action**: Sanity check the fields of received SMP messages.

 `gSecEvt_UnexpectedSmpMessage_c` (BIT2)
- **Attack**: Deviating Behavior - Unexpected SMP message
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, an SMP message is received out of turn. Attacker may try to bypass one of the steps of the process.
- **Preventive Action**: Ensure the SMP state machine does not allow out of turn messages.

 `gSecEvt_IdenticalPeerPublicKey_c` (BIT3)
- **Attack**: ANSSI - Identical Public Key
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, the attacker sends a Public Key whose X coordinate is equal to the X coordinate of the victim device's Public Key. This is a precursor step to a successful attack.
- **Preventive Action**: Check the peer's Public Key X coordinate and do not accept it if it is equal to our own.

 `gSecEvt_ForcedRepairing_c` (BIT13)
- **Attack**: Forced re-pairing
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: A bonded peer rejects link encryption and, if it is a GAP central, attempts to pair again. This could indicate an attempt to downgrade the security of an existing connection or to force a new pairing process that may be vulnerable to attacks.
- **Preventive action**: Signal the application and allow it to decide whether to accept the new pairing request or terminate the connection.

 `gSecEvt_KNOB_c` (BIT16)
- **Attack**: Key Negotiation over Bluetooth
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, an attacker attempts to negotiate a lower encryption key size than the minimum allowed (7 bytes), weakening the security of the connection.
- **Preventive Action**: Validate that the negotiated key size meets minimum security requirements.

 `gSecEvt_InvalidPublicKey_c` (BIT18)
- **Attack**: Invalid public key
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, the attacker sends an invalid Public Key that does not conform to the expected elliptic curve parameters, attempting to cause processing errors or bypass key validation checks.
- **Preventive Action**: Validate that the received Public Key conforms to the expected elliptic curve and reject any malformed keys.

 `gSecEvt_WrongConfirmValue_c` (BIT19)
- **Attack**: Wrong confirm value
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, the attacker sends a Confirm Value that does not match the expected cryptographic computation, attempting to bypass the confirmation step of the pairing process.
- **Preventive Action**: Verify that the received Confirm Value matches the computed expected value and reject the pairing if it does not.

 `gSecEvt_DHKeyCheckFailed_c` (BIT20)
- **Attack**: DH Key Check failed
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During the pairing process, the attacker sends a DHKey Check message that fails cryptographic verification, attempting to bypass the key confirmation step of the pairing process.
- **Preventive Action**: Verify that the received DHKey Check value matches the computed expected value and reject the pairing if it does not.

 `gSecEvt_SameConfirmValue_c` (BIT21)
- **Attack**: Confirm Value Replay Attack (Same Confirm Value)
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: During Legacy Pairing Phase 2, the attacker (acting as Responder) sends LP_CONFIRM_R equal to LP_CONFIRM_I received from the Initiator, attempting to bypass the confirm value verification step and force the Initiator into an incorrect STK encryption attempt. Per Core Spec 6.3 Section 2.3.5.5, the Initiator SHALL verify that LP_CONFIRM_R is not equal to LP_CONFIRM_I.
- **Preventive Action**: Verify that LP_CONFIRM_R != LP_CONFIRM_I and reject the pairing with Confirm Value Failed (0x04) if equal. The IDS event allows the application to take additional defensive action (e.g., disconnect, block device).

### Connection-Related Events

 `gSecEvt_MicFailureDisconnect_c` (BIT5)
- **Attack**: MIC Failure Disconnect
- **Target**: Connection
- **Stack Location**: Host
- **Description**: A packet received from the peer device fails MIC (Message Integrity Check) verification, indicating potential tampering or corruption of the encrypted data. This could indicate an active attack or a corrupted connection.
- **Preventive Action**: Disconnect from the peer device and report the security event for monitoring.

 `gSecEvt_EncReqNonBonded_c` (BIT6)
- **Attack**: Encryption Request from Non-Bonded Device
- **Target**: Connection
- **Stack Location**: Host
- **Description**: An encryption request is received from a device that is not bonded with the local device. This could indicate an attempt to establish an encrypted connection without proper pairing, potentially allowing unauthorized access.
- **Preventive Action**: Reject encryption requests from non-bonded devices and require proper pairing before allowing encrypted connections.

 `gSecEvt_MalformedL2cap_c` (BIT7)
- **Attack**: Malformed L2CAP Packet
- **Target**: Connection
- **Stack Location**: Host
- **Description**: An L2CAP packet is received with malformed structure or invalid parameters, potentially indicating an attempt to exploit protocol parsing vulnerabilities or cause processing errors.
- **Preventive Action**: Validate L2CAP packet structure and parameters, reject malformed packets, and disconnect from the peer device if violations are detected.

 `gSecEvt_MalformedAtt_c` (BIT8)
- **Attack**: Malformed ATT Packet
- **Target**: Connection
- **Stack Location**: Host
- **Description**: An ATT packet is received with malformed structure or invalid parameters, potentially indicating an attempt to exploit protocol parsing vulnerabilities, cause processing errors, or bypass attribute access controls.
- **Preventive Action**: Validate ATT packet structure and parameters, reject malformed packets, and disconnect from the peer device if violations are detected.

`gSecEvt_L2capFragmentsExceedMTU_c` (BIT14)
- **Attack**: Blue Frag
- **Target**: Connection
- **Stack Location**: Host
- **Description**: An attacker sends L2CAP fragments that exceed the negotiated Maximum Transmission Unit (MTU), attempting to cause buffer overflow vulnerabilities or bypass MTU validation checks during packet reassembly.
- **Preventive Action**: Validate that L2CAP fragment sizes do not exceed the negotiated MTU, reject oversized fragments, and disconnect from the peer device if violations are detected.

### Application-Related Events

 `gSecEvt_UnauthenticatedServer_c` (BIT4)
- **Attack**: BLESA
- **Target**: Application
- **Stack Location**: Application
- **Description**: An attacker attempts to access GATT database attributes or services without proper authentication, potentially allowing unauthorized read/write operations on sensitive characteristics.
- **Preventive Action**: Ensure that the peer is properly authenticated before allowing access to protected attributes, enforce attribute-level access controls, and disconnect from the peer device if unauthorized access attempts are detected. This is an application-level operation.

 `gSecEvt_UnauthorizedDbAccess_c` (BIT9)
- **Attack**: Unauthorized GATT Database Access
- **Target**: Application
- **Stack Location**: Application
- **Description**: An attacker attempts to perform unauthorized read or write operations on GATT database attributes without proper authorization, potentially allowing access to sensitive data or modification of critical characteristics.
- **Preventive Action**: Enforce attribute-level authorization checks, validate that the peer has the required permissions before allowing read/write operations, and disconnect from the peer device if unauthorized access attempts are detected. This is an application-level operation.


## Usage Example

The user must set the `gIntrusionDetectionSystem_d` define to 1 in the application's `app_preinclude.h` header. The `IDS_RegisterCallback()` API is used to register a callback for events specified via an event mask.
```c
/* Register for specific security events */
uint32_t eventMask = gSecEvt_MicFailureDisconnect_c | 
                     gSecEvt_UnexpectedSmpMessage_c |
                     gSecEvt_MalformedAtt_c;

IDS_RegisterCallback(securityEventHandler, eventMask);
```

The security events can then be handled in the registered callback.
```c
/* Callback function to handle security events */
void securityEventHandler(idsEventData_t *pEventData) {
    /* Handle the security event based on type */
    switch(pEventData->type) {
        case gSecEvt_MicFailureDisconnect_c:
            /* Handle MIC failure */
            break;
        case gSecEvt_UnexpectedSmpMessage_c:
            /* Handle unexpected SMP message */
            break;
        /* ... handle other events */
    }
}
```

The IDS provides comprehensive security monitoring capabilities for Bluetooth LE applications, helping to detect and respond to various known attack vectors and security vulnerabilities.