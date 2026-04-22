# Intrusion Detection System

The Intrusion Detection System (IDS) is a proprietary feature which allows the Car Anchor to monitor anomalous events related to the traffic and conditions generated during the operation of the Bluetooth LE software stack. These events are detected at Host and Controller level. The application registers a callback where it receives and handles reports.

The IDS feature is supported on the following platforms:

-   KW45B41Z-EVK
-   KW47-EVK
-   FRDM-KW43

To enable IDS, modify the Digital Key Car Anchor as per the steps listed below:

-   In `app_preinclude.h` file, set `gIntrusionDetectionSystem_d` to `1`.

# Intrusion Detection System Test Mode

The IDS Test Mode is a development and testing feature that allows developers to simulate and validate IDS security event detection without requiring actual attack scenarios or malicious devices.

### Enabling Test Mode

To enable IDS Test Mode, set `gIntrusionDetectionSystemTestMode_d` to `1` in `app_preinclude.h`.

This enables:
- Shell command `idstest` for triggering security events
- Pairing message interception hooks
- Test packet generation utilities

### Supported Security Events

IDS Test Mode supports triggering the following security events:

| Bit | Event Macro | Description | Category | Trigger Method |
|-----|-------------|-------------|----------|----------------|
| 0 | `gSecEvt_Sweyntooth_MaxEncKeySize_c` | Max Encryption Key Size (Sweyntooth) | Pairing | Hook |
| 1 | `gSecEvt_DhKeyCheckZero_c` | DH Key Check Zero | Pairing | Hook |
| 2 | `gSecEvt_UnexpectedSmpMessage_c` | Unexpected SMP Message | Pairing | Hook |
| 5 | `gSecEvt_MicFailureDisconnect_c` | MIC Failure Disconnect | Connection | Packet |
| 6 | `gSecEvt_EncReqNonBonded_c` | Non-Bonded Device | Connection | Packet |
| 7 | `gSecEvt_MalformedL2cap_c` | Malformed L2CAP Packet | Connection | Packet |
| 8 | `gSecEvt_MalformedAtt_c` | Malformed ATT Packet | Connection | Packet |
| 13 | `gSecEvt_ForcedRepairing_c` | Forced Re-pairing | Pairing | Hook |
| 14 | `gSecEvt_L2capFragmentsExceedMTU_c` | L2CAP Fragments Exceed MTU (Blue Frag) | Connection | Packet |
| 16 | `gSecEvt_KNOB_c` | Key Negotiation over Bluetooth (KNOB) | Pairing | Hook |
| 18 | `gSecEvt_InvalidPublicKey_c` | Invalid Public Key | Pairing | Hook |
| 19 | `gSecEvt_WrongConfirmValue_c` | Wrong Confirm Value | Pairing | Hook |
| 20 | `gSecEvt_DHKeyCheckFailed_c` | DH Key Check Failed | Pairing | Hook |

### Event Categories

#### Connection Events (Immediate Trigger)
These events are triggered immediately when the `idstest` command is executed:
- **BIT5**: MIC Failure Disconnect
- **BIT7**: Malformed L2CAP Packet
- **BIT8**: Malformed ATT Packet
- **BIT14**: L2CAP Fragments Exceed MTU

#### BIT6: Encryption Request from Non-Bonded Device

**Natural Scenario:**
1. Connect device to anchor with bonding enabled
2. Complete pairing and bonding successfully
3. Disconnect device
4. On anchor: Run `factoryreset` command
5. On device: Reconnect without clearing bonds
6. IDS event triggers when device sends encryption request

#### Pairing Events (Hook-based Trigger)
These events activate a message interception hook and trigger during the next pairing procedure:
- **BIT0**: Sweyntooth Max Encryption Key Size
- **BIT1**: DH Key Check Zero
- **BIT2**: Unexpected SMP Message
- **BIT13**: Forced Re-pairing
- **BIT16**: KNOB (Key Negotiation)
- **BIT18**: Invalid Public Key
- **BIT19**: Wrong Confirm Value
- **BIT20**: DH Key Check Failed

### Using the `idstest` Command
`idstest <event_bit> [device_id]`

#### Examples

**Trigger Connection Event (Immediate):**
```
Anchor> idstest 5 0
[IDS TEST] Event BIT5 (mask 0x20000000) triggered for device 0.
Anchor> Received IDS event 32 triggered by peer 25008F195173
```
**Trigger Pairing Event (During Next Pairing):**
```
Anchor> idstest 0 0
[IDS TEST] Pairing event BIT0 (mask 0x00000001) will be triggered on next pairing.
[IDS TEST] Hook activated. Now run 'sd op' to start Owner Pairing.

Anchor> sd op
Advertising started - Legacy.
Connected!
Received IDS event 65536 triggered by peer 1BE0914D955A
```