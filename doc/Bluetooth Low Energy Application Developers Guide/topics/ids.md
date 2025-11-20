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
- **Description**: Detects attempts to negotiate weak encryption key sizes during pairing

`gSecEvt_DhKeyCheckZero_c` (BIT1)
- **Attack**: Deviating Behavior - DHKey Check Zero
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Identifies when a peer sends a zero value for the Diffie-Hellman key check

 `gSecEvt_UnexpectedSmpMessage_c` (BIT2)
- **Attack**: Deviating Behavior - Unexpected SMP message
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Detects SMP (Security Manager Protocol) messages received out of sequence or context

 `gSecEvt_IdenticalPeerPublicKey_c` (BIT3)
- **Attack**: ANSSI - Identical Public Key
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Identifies when a peer uses the same public key across multiple pairing attempts

 `gSecEvt_ForcedRepairing_c` (BIT13)
- **Attack**: Forced re-pairing
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Detects attempts to force re-pairing when devices are already bonded

 `gSecEvt_KNOB_c` (BIT16)
- **Attack**: Key Negotiation over Bluetooth
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Detects KNOB attacks that attempt to downgrade encryption key strength

 `gSecEvt_InvalidPublicKey_c` (BIT18)
- **Attack**: Invalid public key
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Identifies malformed or invalid public keys during pairing

 `gSecEvt_WrongConfirmValue_c` (BIT19)
- **Attack**: Wrong confirm value
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Detects incorrect confirmation values during pairing authentication

 `gSecEvt_DHKeyCheckFailed_c` (BIT20)
- **Attack**: DH Key Check failed
- **Target**: Pairing process
- **Stack Location**: Host
- **Description**: Identifies failures in Diffie-Hellman key verification

### Connection-Related Events

 `gSecEvt_MicFailureDisconnect_c` (BIT5)
- **Attack**: MIC Failure Disconnect
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Detects Message Integrity Check failures that result in disconnection

 `gSecEvt_EncReqNonBonded_c` (BIT6)
- **Attack**: Encryption Request from Non-Bonded Device
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Identifies encryption requests from devices that have not completed bonding

 `gSecEvt_MalformedL2cap_c` (BIT7)
- **Attack**: Malformed L2CAP Packet
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Detects improperly formatted L2CAP (Logical Link Control and Adaptation Protocol) packets

 `gSecEvt_MalformedAtt_c` (BIT8)
- **Attack**: Malformed ATT Packet
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Identifies malformed ATT (Attribute Protocol) packets

 `gSecEvt_LlLenOverflow_c` (BIT10)
- **Attack**: Sweyntooth - LL Length Overflow
- **Target**: Connection
- **Stack Location**: Link Layer
- **Description**: Detects Link Layer length field overflow attacks

 `gSecEvt_SequentialAttDeadlock_c` (BIT11)
- **Attack**: Sweyntooth - Sequential ATT Deadlock
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Identifies sequential ATT request patterns that can cause deadlocks

 `gSecEvt_RepeatedAttemptsDisconnect_c` (BIT12)
- **Attack**: Repeated attempts disconnect
- **Target**: Connection
- **Stack Location**: Link Layer
- **Description**: Detects rapid repeated authentication attempts that violate timing requirements

`gSecEvt_L2capFragmentsExceedMTU_c` (BIT14)
- **Attack**: Blue Frag
- **Target**: Connection
- **Stack Location**: Host
- **Description**: Identifies L2CAP fragmentation attacks where fragments exceed MTU limits

 `gSecEvt_BleedingTooth_c` (BIT15)
- **Attack**: Bleeding Tooth
- **Target**: Connection
- **Stack Location**: Link Layer
- **Description**: Detects Bleeding Tooth vulnerability exploitation attempts

 `gSecEvt_Sweyntooth_HciDesyncDeadlock_c` (BIT17)
- **Attack**: HCI Desync Deadlock
- **Target**: Connection
- **Stack Location**: Link Layer
- **Description**: Identifies HCI (Host Controller Interface) desynchronization attacks

### Application-Related Events

 `gSecEvt_UnauthenticatedServer_c` (BIT4)
- **Attack**: BLESA
- **Target**: Application
- **Stack Location**: Application
- **Description**: Detects connections to unauthenticated servers (BLESA attack)

 `gSecEvt_UnauthorizedDbAccess_c` (BIT9)
- **Attack**: Unauthorized GATT Database Access
- **Target**: Application
- **Stack Location**: Application
- **Description**: Identifies unauthorized attempts to access GATT database attributes


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