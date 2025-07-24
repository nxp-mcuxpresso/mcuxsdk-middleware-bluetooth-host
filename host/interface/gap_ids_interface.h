/*! *********************************************************************************
* Copyright 2025 NXP
*
* \file gap_ids_interface.h
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */
#ifndef GAP_IDS_INTERFACE_H
#define GAP_IDS_INTERFACE_H

#include "EmbeddedTypes.h"
#include "ble_constants.h"
#include "ble_general.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
/* All events enabled */
#define gGapIdsAllFlags_c (0xFFFFFFFFU)

#define IDS_IS_ENABLED(event)   ((gIdsMask & ((uint32_t)event)) != 0U)

/*************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
typedef enum {
    /*! Sweyntooth - Max Encryption Key Size. Target: Pairing. Stack location: Host. */
    gSecEvt_Sweyntooth_MaxEncKeySize_c      = BIT0,

    /*! Deviating Behavior - DHKey Check Zero. Target: Pairing. Stack location: Host. */
    gSecEvt_DhKeyCheckZero_c                = BIT1,

    /*! Deviating Behavior - Unexpected SMP message. Target: Pairing. Stack location: Host. */
    gSecEvt_UnexpectedSmpMessage_c          = BIT2,

    /*! ANSSI - Identical Public Key. Target: Pairing. Stack location: Host. */
    gSecEvt_IdenticalPeerPublicKey_c        = BIT3,

    /*! BLESA. Target: Application. Stack location: Application. */
    gSecEvt_UnauthenticatedServer_c         = BIT4,

    /*! MIC Failure Disconnect. Target: Connection. Stack location: Host. */
    gSecEvt_MicFailureDisconnect_c          = BIT5,

    /*! Encryption Request from Non-Bonded Device. Target: Connection. Stack location: Host. */
    gSecEvt_EncReqNonBonded_c               = BIT6,

    /*! Malformed LCAP Packet. Target: Connection. Stack location: Host. */
    gSecEvt_MalformedL2cap_c                = BIT7,

    /*! Malformed ATT Packet. Target: Connection. Stack location: Host. */
    gSecEvt_MalformedAtt_c                  = BIT8,

    /*! Unauthorized GATT Database Access. Target: Application. Stack location: Application. */
    gSecEvt_UnauthorizedDbAccess_c          = BIT9,

    /*! Sweyntooth - LL Length Overflow. Target: Connection. Stack location: Link Layer. */
    gSecEvt_LlLenOverflow_c                 = BIT10,

    /*! Sweyntooth - Sequential ATT Deadlock. Target: Connection. Stack location: Host. */
    gSecEvt_SequentialAttDeadlock_c         = BIT11,

    /*! Repeated attempts disconnect - Too little time has elapsed since the last
     * authentication or pairing attempt failed. Target: Connection. Stack location: Link Layer. */
    gSecEvt_RepeatedAttemptsDisconnect_c    = BIT12,

    /*! Forced re-pairing. Target: Pairing. Stack location: Host. */
    gSecEvt_ForcedRepairing_c              = BIT13,

    /*! Blue Frag. Target: Connection. Stack location: Host */
    gSecEvt_L2capFragmentsExceedMTU_c       = BIT14,

    /*! Bleeding Tooth. Target: Connection. Stack location: Link Layer. */
    gSecEvt_BleedingTooth_c                 = BIT15,

    /*! Key Negotiation over Bluetooth. Target: Pairing. Stack location: Host. */
    gSecEvt_KNOB_c                          = BIT16,

    /*! HCI Desync Deadlock. Target: Connection. Stack location: Link Layer. */
    gSecEvt_Sweyntooth_HciDesyncDeadlock_c  = BIT17,

    /*!Invalid public key. Target: Pairing. Stack location: Host. */
    gSecEvt_InvalidPublicKey_c              = BIT18,

    /*! Wrong confirm value. Target: Pairing. Stack location: Host. */
    gSecEvt_WrongConfirmValue_c             = BIT19,

    /*! DH Key Check failed. Target: Pairing. Stack location: Host. */
    gSecEvt_DHKeyCheckFailed_c              = BIT20,
} idsEventType_t;


/*! Structure holding event information */
typedef struct
{
    idsEventType_t type;
    uint8_t aAddr[gcBleDeviceAddressSize_c];    /*!< MAC address of the sender */
    uint16_t connInterval;                      /*!< Connection interval */
    uint16_t peripheralLatency;                 /*!< Peripheral latency */
    uint16_t supervisionTimeout;                /*!< Supervision timeout */
} idsEventData_t;

/*! Function pointer type */
typedef void(*idsCallback_t)(idsEventData_t *pEventData);

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
extern uint32_t gIdsMask;

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!*************************************************************************************************
*\fn     void IDS_RegisterCallback(idsCallback_t pfCallback, uint32_t bitmask)

*\brief  Enable/disable the IDS events reporting.
*
*\param [in] pfCallback     The callback used to report events
*\param [in] bitMask        Mask of bits, specifying which events will be reported.
*
*\return                    None
***************************************************************************************************/
void IDS_RegisterCallback(idsCallback_t pfCallback, uint32_t bitMask);

/*!*************************************************************************************************
*\fn     void IDS_TriggerCallback(deviceId_t deviceId, idsEventType_t eventType)

*\brief  Trigger the IDS callback with a security event.
*
*\param [in] deviceId       Peer device ID
*\param [in] eventType      IDS event type
*
***************************************************************************************************/
void IDS_TriggerCallback(deviceId_t deviceId, idsEventType_t eventType);

/*!*************************************************************************************************
*\fn    bleResult_t IDS_PopulateSecEvent(deviceId_t deviceId, idsEventData_t *pEventData)
*
*\brief Function which populates a security event for IDS.
*
* \param[in]  deviceId        Device ID.
* \param[out] pEventData      Pointer to event data to be populated.
*
*\return       bleResult_t
***************************************************************************************************/
bleResult_t IDS_PopulateSecEvent(deviceId_t deviceId, idsEventData_t *pEventData);

#ifdef __cplusplus
}
#endif

#endif /* GAP_IDS_INTERFACE_H */

/*!*************************************************************************************************
* @}
***************************************************************************************************/