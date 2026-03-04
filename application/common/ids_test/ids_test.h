/*! *********************************************************************************
* Copyright 2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef IDS_TEST_H
#define IDS_TEST_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ble_general.h"
#include "hci_transport.h"

/************************************************************************************
*************************************************************************************
* Public macros
*************************************************************************************
************************************************************************************/
#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == TRUE)

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/
/* Event triggered during connection.*/
#define IDS_EVENT_CATEGORY_ANYTIME      0x01U

/* Event triggered during pairing.*/
#define IDS_EVENT_CATEGORY_PAIRING      0x02U

/* Macro to determine IDS event category based on event bit mask */
#define IDS_GET_EVENT_CATEGORY(eventBitMask) \
    (((eventBitMask) == (uint32_t)gSecEvt_Sweyntooth_MaxEncKeySize_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_DhKeyCheckZero_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_UnexpectedSmpMessage_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_ForcedRepairing_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_KNOB_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_InvalidPublicKey_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_WrongConfirmValue_c) || \
     ((eventBitMask) == (uint32_t)gSecEvt_DHKeyCheckFailed_c) \
     ? IDS_EVENT_CATEGORY_PAIRING \
     : IDS_EVENT_CATEGORY_ANYTIME)

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Activate pairing message interception hook.
*
*\param        [in]  deviceId             Device ID for the connection.
*\param        [in]  eventBitMask         IDS event bit mask to trigger.
*
*\retval       gBleSuccess_c              Hook activated successfully.
********************************************************************************** */
bleResult_t IdsTest_ActivatePairingHook
(
    deviceId_t deviceId,
    uint32_t eventBitMask
);

/*! *********************************************************************************
*\brief        Deactivate pairing message interception hook.
*
*\retval       void.
********************************************************************************** */
void IdsTest_DeactivatePairingHook(void);

/*! *********************************************************************************
*\brief        Check if pairing hook is active.
*
*\retval       TRUE if active, FALSE otherwise.
********************************************************************************** */
bool_t IdsTest_IsPairingHookActive(void);

/*! *********************************************************************************
*\brief        Process and potentially modify pairing messages for IDS testing.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet buffer.
*\param        [in]  packetLength         Total packet length in bytes.
*
*\retval       gBleSuccess_c              Message processed successfully.
*\retval       gBleInvalidParameter_c     Invalid packet structure or length.
********************************************************************************** */
bleResult_t IdsTest_ProcessPairingMessage
(
    uint8_t* pPacket,
    uint16_t packetLength
);

/*! *********************************************************************************
*\brief        Trigger an IDS security event for testing purposes.
*
*\param        [in]  deviceId             Device ID for the connection (if applicable).
*\param        [in]  eventBitMask         IDS event bit mask to trigger.
*
*\retval       gBleSuccess_c              Event triggered successfully.
*\retval       gBleInvalidParameter_c     Invalid event or device ID.
********************************************************************************** */
bleResult_t IdsTest_TriggerEvent
(
    deviceId_t deviceId,
    uint32_t eventBitMask
);

#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == TRUE) */

#endif /* IDS_TEST_H */