/*! *********************************************************************************
* Copyright 2026 NXP
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "ids_test.h"

#if defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U)
#include "att_types.h"
#include "gap_interface.h"
#include "gap_ids_interface.h"
#include "gatt_interface.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/
/* Maximum packet size for IDS test packets */
#define gIdsTestMaxPacketSize_c         300U

/* HCI ACL Data Packet Header Size */
#define gHciAclDataPacketHeaderSize_c   4U

/* L2CAP Header Size */
#define gL2capHeaderSize_c              4U

/* SMP Opcodes */
#define gSmpOpcodePairingRequest_c      0x01U
#define gSmpOpcodePairingResponse_c     0x02U
#define gSmpOpcodePairingPublicKey_c    0x0CU
#define gSmpOpcodePairingDHKeyCheck_c   0x0DU

/* PB Flag */
#define gHciPbfC2H_StartOfPacket        0x02U
/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/
typedef struct idsTestPairingHook_tag
{
    bool_t      isActive;
    uint32_t    eventBitMask;
    deviceId_t  deviceId;
} idsTestPairingHook_t;

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* Connection events */
static bleResult_t IdsTest_BuildMicFailurePacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize);
static bleResult_t IdsTest_BuildMalformedL2capPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize);
static bleResult_t IdsTest_BuildMalformedAttPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize);
static bleResult_t IdsTest_BuildL2capFragmentsExceedMtuPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize);

/* Pairing events */
static bleResult_t IdsTest_ModifyMaxEncKeySize(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyDhKeyCheckZero(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyUnexpectedSmpMessage(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyInvalidPublicKey(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyForcedRepairing(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyKNOB(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyWrongConfirmValue(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);
static bleResult_t IdsTest_ModifyDHKeyCheckFailed(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode);

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
static uint8_t mIdsTestPacketBuffer[gIdsTestMaxPacketSize_c] = {0U};

static idsTestPairingHook_t mIdsTestPairingHook = 
{
    .isActive = FALSE,
    .eventBitMask = 0U,
    .deviceId = gInvalidDeviceId_c
};

/************************************************************************************
*************************************************************************************
* Public functions
************************************************************************************/
/*! *********************************************************************************
*\brief        Activate pairing message interception hook.
*
*\param        [in]  deviceId             Device ID for the connection.
*\param        [in]  eventBitMask         IDS event bit mask to trigger.
*
*\retval       gBleSuccess_c              Hook activated successfully.
********************************************************************************** */
bleResult_t IdsTest_ActivatePairingHook(deviceId_t deviceId, uint32_t eventBitMask)
{
    mIdsTestPairingHook.isActive = TRUE;
    mIdsTestPairingHook.eventBitMask = eventBitMask;
    mIdsTestPairingHook.deviceId = deviceId;

    return gBleSuccess_c;
}

/*! *********************************************************************************
*\brief        Deactivate pairing message interception hook.
*
*\retval       void.
********************************************************************************** */
void IdsTest_DeactivatePairingHook(void)
{
    mIdsTestPairingHook.isActive = FALSE;
    mIdsTestPairingHook.eventBitMask = 0U;
    mIdsTestPairingHook.deviceId = gInvalidDeviceId_c;
}

/*! *********************************************************************************
*\brief        Check if pairing hook is active.
*
*\retval       TRUE if active, FALSE otherwise.
********************************************************************************** */
bool_t IdsTest_IsPairingHookActive(void)
{
    return mIdsTestPairingHook.isActive;
}

/*! *********************************************************************************
*\brief        Process and potentially modify pairing messages for IDS testing.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet buffer.
*\param        [in]  packetLength         Total packet length in bytes.
*
*\retval       gBleSuccess_c              Message processed successfully.
*\retval       gBleInvalidParameter_c     Invalid packet structure or length.
********************************************************************************** */
bleResult_t IdsTest_ProcessPairingMessage(uint8_t* pPacket, uint16_t packetLength)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t index = 0U;
    uint8_t smpOpcode = 0U;

    do
    {
        /* Check if hook is active */
        if (!mIdsTestPairingHook.isActive)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Minimum packet size check: HCI header (4) + L2CAP header (4) + SMP opcode (1) */
        if (packetLength < (gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 1U))
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Skip HCI ACL header */
        index = gHciAclDataPacketHeaderSize_c;

        /* Extract L2CAP Length */
        uint32_t l2capLengthTemp = (uint32_t)pPacket[index] | ((uint32_t)pPacket[index + 1U] << 8U);

        /* Verify value fits in uint16_t */
        if (l2capLengthTemp > 0xFFFFU)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        uint16_t l2capLength = (uint16_t)l2capLengthTemp;
        index += 2U;

        /* Extract L2CAP CID with overflow check */
        uint32_t l2capCidTemp = (uint32_t)pPacket[index] | ((uint32_t)pPacket[index + 1U] << 8U);

        /* Verify value fits in uint16_t */
        if (l2capCidTemp > 0xFFFFU)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        uint16_t l2capCid = (uint16_t)l2capCidTemp;
        index += 2U;

        /* Validate L2CAP length */
        uint32_t expectedPacketLengthCheck = (uint32_t)gHciAclDataPacketHeaderSize_c + 
                                              (uint32_t)gL2capHeaderSize_c + 
                                              (uint32_t)l2capLength;

        if (expectedPacketLengthCheck > 0xFFFFU)
        {
            result = gBleInvalidParameter_c;
            break;
        }
        
        uint16_t expectedPacketLength = (uint16_t)expectedPacketLengthCheck;

        if (packetLength < expectedPacketLength)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Check if this is an SMP message */
        if (l2capCid != gL2capCidSmp_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Validate that we have at least 1 byte for SMP opcode */
        if (l2capLength < 1U)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Extract SMP Opcode */
        smpOpcode = pPacket[index];

    } while(FALSE);

    /* Process based on event type */
    switch (mIdsTestPairingHook.eventBitMask)
    {
        case (uint32_t)gSecEvt_Sweyntooth_MaxEncKeySize_c:
        {
            result = IdsTest_ModifyMaxEncKeySize(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_DhKeyCheckZero_c:
        {
            result = IdsTest_ModifyDhKeyCheckZero(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_UnexpectedSmpMessage_c:
        {
            result = IdsTest_ModifyUnexpectedSmpMessage(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_ForcedRepairing_c:
        {
            result = IdsTest_ModifyForcedRepairing(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_KNOB_c:
        {
            result = IdsTest_ModifyKNOB(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_InvalidPublicKey_c:
        {
            result = IdsTest_ModifyInvalidPublicKey(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_WrongConfirmValue_c:
        {
            result = IdsTest_ModifyWrongConfirmValue(pPacket, packetLength, smpOpcode);
        }
        break;

        case (uint32_t)gSecEvt_DHKeyCheckFailed_c:
        {
            result = IdsTest_ModifyDHKeyCheckFailed(pPacket, packetLength, smpOpcode);
        }
        break;

        default:
        {
            /* No modification needed for this event */
        }
        break;
    }

    return result;
}

/*! *********************************************************************************
*\brief        Trigger an IDS security event for testing purposes.
*
*\param        [in]  deviceId             Device ID for the connection (if applicable).
*\param        [in]  eventBitMask         IDS event bit mask to trigger.
*
*\retval       gBleSuccess_c              Event triggered successfully.
*\retval       gBleInvalidParameter_c     Invalid event or device ID.
********************************************************************************** */
bleResult_t IdsTest_TriggerEvent(deviceId_t deviceId, uint32_t eventBitMask)
{
    bleResult_t result = gBleSuccess_c;
    uint8_t category = IDS_GET_EVENT_CATEGORY(eventBitMask);

    /* Check if this is a pairing-related event */
    if (category == IDS_EVENT_CATEGORY_PAIRING)
    {
        /* Pairing events - activate hook */
        result = IdsTest_ActivatePairingHook(deviceId, eventBitMask);
    }
    else
    {
        /* Connection events - trigger immediately */
        uint8_t* pPacket = NULL;
        uint16_t packetSize = 0U;
        hciPacketType_t packetType = gHciDataPacket_c;

        switch (eventBitMask)
        {
            case (uint32_t)gSecEvt_MicFailureDisconnect_c:
            {
                result = IdsTest_BuildMicFailurePacket(deviceId, &pPacket, &packetSize);
                packetType = gHciEventPacket_c;
            }
            break;

            case (uint32_t)gSecEvt_MalformedL2cap_c:
            {
                result = IdsTest_BuildMalformedL2capPacket(deviceId, &pPacket, &packetSize);
                packetType = gHciDataPacket_c;
            }
            break;

            case (uint32_t)gSecEvt_MalformedAtt_c:
            {
                result = IdsTest_BuildMalformedAttPacket(deviceId, &pPacket, &packetSize);
                packetType = gHciDataPacket_c;
            }
            break;

            case (uint32_t)gSecEvt_L2capFragmentsExceedMTU_c:
            {
                result = IdsTest_BuildL2capFragmentsExceedMtuPacket(deviceId, &pPacket, &packetSize);
                packetType = gHciDataPacket_c;
            }
            break;

            default:
            {
                result = gBleInvalidParameter_c;
            }
            break;
        }

        if ((result == gBleSuccess_c) && (pPacket != NULL))
        {
            result = Hcit_InjectPacket(packetType, pPacket, packetSize);
        }
    }

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\brief        Build a packet that will trigger MIC failure detection for IDS.
*
*\param        [in]  deviceId             Device ID.
*\param        [out] ppPacket             Pointer to store the packet pointer.
*\param        [out] pPacketSize          Pointer to store the packet size.
*
*\retval       gBleSuccess_c              Packet built successfully.
*\retval       gBleInvalidParameter_c     Invalid device ID.
*\retval       gBleOutOfMemory_c          Memory allocation failed.
********************************************************************************** */
static bleResult_t IdsTest_BuildMicFailurePacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t connHandle = 0U;
    uint16_t index = 0U;
    const uint16_t totalSize = 6U;

    do
    {
        /* Get connection handle */
        result = Gap_GetConnectionHandleFromDeviceId(deviceId, &connHandle);

        if (result == gBleSuccess_c)
        {

            FLib_MemSet(mIdsTestPacketBuffer, 0x00, totalSize);

            /* Event Code - Disconnection Complete */
            mIdsTestPacketBuffer[index++] = 0x05U;

            /* Parameter Total Length */
            mIdsTestPacketBuffer[index++] = 0x04U;

            /* Status - Success */
            mIdsTestPacketBuffer[index++] = 0x00U;

            /* Connection Handle */
            mIdsTestPacketBuffer[index++] = (uint8_t)(connHandle & 0xFFU);
            mIdsTestPacketBuffer[index++] = (uint8_t)((connHandle >> 8U) & 0xFFU);

            /* Reason - MIC Failure (0x3D) */
            mIdsTestPacketBuffer[index] = 0x3DU;

            *ppPacket = mIdsTestPacketBuffer;
            *pPacketSize = totalSize;

        }
    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Build a malformed L2CAP packet that will trigger IDS detection.
*              The packet will have an L2CAP signaling command with incorrect length.
*
*\param        [in]  deviceId             Device ID.
*\param        [out] ppPacket             Pointer to store the packet pointer.
*\param        [out] pPacketSize          Pointer to store the packet size.
*
*\retval       gBleSuccess_c              Packet built successfully.
*\retval       gBleInvalidParameter_c     Invalid device ID.
*\retval       gBleOutOfMemory_c          Memory allocation failed.
********************************************************************************** */
static bleResult_t IdsTest_BuildMalformedL2capPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t connHandle = 0U;
    uint16_t index = 0U;
    const uint16_t totalSize = 15U;

    do
    {
       /* Get connection handle */
        result = Gap_GetConnectionHandleFromDeviceId(deviceId, &connHandle);

        if (result == gBleSuccess_c)
        {
            FLib_MemSet(mIdsTestPacketBuffer, 0x00, totalSize);

            /* HCI ACL Header */
            uint16_t handleAndFlags = (connHandle & 0x0FFFU) | 
                                     ((uint16_t)gHciPbfC2H_StartOfPacket<< 12U);
            mIdsTestPacketBuffer[index++] = (uint8_t)(handleAndFlags & 0xFFU);
            mIdsTestPacketBuffer[index++] = (uint8_t)((handleAndFlags >> 8U) & 0xFFU);

            /* HCI Data Length */
            mIdsTestPacketBuffer[index++] = 0x0BU;
            mIdsTestPacketBuffer[index++] = 0x00U;

            /* L2CAP Header */
            mIdsTestPacketBuffer[index++] = 0x07U;
            mIdsTestPacketBuffer[index++] = 0x00U;

            /* CID */
            mIdsTestPacketBuffer[index++] = 0x05U;
            mIdsTestPacketBuffer[index++] = 0x00U;

            /* Disconnect Request */
            mIdsTestPacketBuffer[index++] = 0x06U;

            /* Identifier */
            mIdsTestPacketBuffer[index++] = 0x01U;

            /* Length */
            mIdsTestPacketBuffer[index++] = 0x03U;
            mIdsTestPacketBuffer[index++] = 0x00U;

            /* Payload */
            mIdsTestPacketBuffer[index++] = 0x40U;
            mIdsTestPacketBuffer[index++] = 0x00U;
            mIdsTestPacketBuffer[index] = 0x40U;

            *ppPacket = mIdsTestPacketBuffer;
            *pPacketSize = totalSize;
        }
    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Build a malformed ATT packet that will trigger IDS detection.
*
*\param        [in]  deviceId             Device ID.
*\param        [out] ppPacket             Pointer to store the packet pointer.
*\param        [out] pPacketSize          Pointer to store the packet size.
*
*\retval       gBleSuccess_c              Packet built successfully.
*\retval       gBleInvalidParameter_c     Invalid device ID.
*\retval       gBleOutOfMemory_c          Memory allocation failed.
********************************************************************************** */
static bleResult_t IdsTest_BuildMalformedAttPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t connHandle = 0U;
    uint16_t connectionMtu = gAttDefaultMtu_c;
    uint16_t malformedAttLen = 0U;
    uint16_t totalSize = 0U;
    uint16_t index = 0U;

    do
    {
        /* Get connection handle */
        result = Gap_GetConnectionHandleFromDeviceId(deviceId, &connHandle);
        if (result != gBleSuccess_c)
        {
            break;
        }

        /* Get the current MTU for this connection using GATT API */
        (void)Gatt_GetMtu(deviceId, &connectionMtu);
        result = gBleSuccess_c;

        /* Build malformed ATT packet that exceeds MTU by 10 bytes */
        malformedAttLen = connectionMtu + 10U;

        /* Total packet size: HCI header + L2CAP header + ATT data */
        totalSize = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + malformedAttLen;

        /* Check if static buffer is large enough */
        if (totalSize > gIdsTestMaxPacketSize_c)
        {
            result = gBleOutOfMemory_c;
            break;
        }

        /* Clear static buffer */
        FLib_MemSet(mIdsTestPacketBuffer, 0x00, totalSize);

        /* Build HCI ACL Header */
        uint16_t handleAndFlags = (connHandle & 0x0FFFU) |
                                 ((uint16_t)gHciPbfC2H_StartOfPacket << 12U);

        mIdsTestPacketBuffer[index++] = (uint8_t)(handleAndFlags & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((handleAndFlags >> 8U) & 0xFFU);

        /* HCI Data Length */
        uint16_t hciDataLen = gL2capHeaderSize_c + malformedAttLen;
        mIdsTestPacketBuffer[index++] = (uint8_t)(hciDataLen & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((hciDataLen >> 8U) & 0xFFU);

        /* L2CAP Header */
        mIdsTestPacketBuffer[index++] = (uint8_t)(malformedAttLen & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((malformedAttLen >> 8U) & 0xFFU);

        /* L2CAP CID - ATT */
        mIdsTestPacketBuffer[index++] = (uint8_t)(gL2capCidAtt_c & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)(((uint16_t)gL2capCidAtt_c >> 8U) & 0xFFU);

        /* ATT Opcode - Read Request */
        mIdsTestPacketBuffer[index++] = 0x0AU;

        /* Attribute Handle */
        mIdsTestPacketBuffer[index++] = 0x01U;
        mIdsTestPacketBuffer[index++] = 0x00U;

        /* This will trigger IDS malformed ATT detection */
        for (uint16_t i = 3U; i < malformedAttLen; i++)
        {
            mIdsTestPacketBuffer[index++] = 0xAAU;
        }

        *ppPacket = mIdsTestPacketBuffer;
        *pPacketSize = totalSize;

    } while (FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Build L2CAP Fragments Exceed MTU packet (Blue Frag attack).
*
*\param        [in]  deviceId             Device ID for the connection.
*\param        [out] ppPacket             Pointer to store the packet pointer.
*\param        [out] pPacketSize          Pointer to store the packet size.
*
*\retval       gBleSuccess_c              Packet built successfully.
*\retval       gBleInvalidParameter_c     Invalid device ID.
*\retval       gBleOutOfMemory_c          Memory allocation failed or MTU exceeded.
********************************************************************************** */
static bleResult_t IdsTest_BuildL2capFragmentsExceedMtuPacket(deviceId_t deviceId, uint8_t** ppPacket, uint16_t* pPacketSize)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t connHandle = 0U;
    uint16_t connectionMtu = gAttDefaultMtu_c;
    uint16_t index = 0U;
    const uint16_t l2capCid = 0x0040U;
    const uint16_t dataPayloadSize = 60U;
    const uint16_t l2capPayloadLength = 2U + dataPayloadSize;
    const uint16_t totalSize = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + l2capPayloadLength;
    uint16_t handleAndFlags = 0U;
    uint16_t hciDataLen = 0U;
    uint16_t malformedSduLength = 0U;

    do
    {
        result = Gap_GetConnectionHandleFromDeviceId(deviceId, &connHandle);
        if (result != gBleSuccess_c)
        {
            break;
        }

        /* Get the current MTU for this connection using GATT API */
        (void)Gatt_GetMtu(deviceId, &connectionMtu);

        malformedSduLength = connectionMtu + 50U;

        FLib_MemSet(mIdsTestPacketBuffer, 0x00, totalSize);

        handleAndFlags = (connHandle & 0x0FFFU) | ((uint16_t)gHciPbfC2H_StartOfPacket << 12U);
        mIdsTestPacketBuffer[index++] = (uint8_t)(handleAndFlags & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((handleAndFlags >> 8U) & 0xFFU);

        hciDataLen = gL2capHeaderSize_c + l2capPayloadLength;
        mIdsTestPacketBuffer[index++] = (uint8_t)(hciDataLen & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((hciDataLen >> 8U) & 0xFFU);

        mIdsTestPacketBuffer[index++] = (uint8_t)(l2capPayloadLength & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((l2capPayloadLength >> 8U) & 0xFFU);

        mIdsTestPacketBuffer[index++] = (uint8_t)(l2capCid & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((l2capCid >> 8U) & 0xFFU);

        mIdsTestPacketBuffer[index++] = (uint8_t)(malformedSduLength & 0xFFU);
        mIdsTestPacketBuffer[index++] = (uint8_t)((malformedSduLength >> 8U) & 0xFFU);

        for (uint16_t i = 0U; i < dataPayloadSize; i++)
        {
            mIdsTestPacketBuffer[index++] = 0xCCU;
        }

        *ppPacket = mIdsTestPacketBuffer;
        *pPacketSize = totalSize;

    } while (FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger Max Encryption Key Size event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyMaxEncKeySize(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t maxKeyOffset = 0U;

    do
    {
        /* Only modify Pairing Request or Response */
        if ((smpOpcode != gSmpOpcodePairingRequest_c) && 
            (smpOpcode != gSmpOpcodePairingResponse_c))
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to Max Key Size field: HCI(4) + L2CAP(4) + Opcode(1) + IO(1) + OOB(1) + Auth(1) + MaxKey(1) */
        maxKeyOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 4U;

        /* Validate packet length */
        if (packetLength <= maxKeyOffset)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Set Max Encryption Key Size to 1 (minimum, triggers Sweyntooth) */
        pPacket[maxKeyOffset] = 0x01U;

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger DH Key Check Zero event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyDhKeyCheckZero(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t dhKeyCheckOffset = 0U;
    const uint16_t dhKeyCheckSize = 16U;

    do
    {
        /* Only modify Pairing DHKey Check */
        if (smpOpcode != gSmpOpcodePairingDHKeyCheck_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to DHKey Check data: HCI(4) + L2CAP(4) + Opcode(1) = 9 */
        dhKeyCheckOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 1U;

        /* Validate packet length */
        if (packetLength < (dhKeyCheckOffset + dhKeyCheckSize))
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Set DHKey Check to all zeros */
        FLib_MemSet(&pPacket[dhKeyCheckOffset], 0x00, dhKeyCheckSize);

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger Unexpected SMP Message event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length.
********************************************************************************** */
static bleResult_t IdsTest_ModifyUnexpectedSmpMessage(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t smpOpcodeOffset = 0U;

    do
    {
        /* Only modify first SMP message (Pairing Request) */
        if (smpOpcode != gSmpOpcodePairingRequest_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to SMP opcode: HCI(4) + L2CAP(4) = 8 */
        smpOpcodeOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c;

        /* Validate packet length */
        if (packetLength <= smpOpcodeOffset)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Change opcode to Pairing Confirm unexpected at this stage */
        pPacket[smpOpcodeOffset] = 0x03U;

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP Public Key to trigger Invalid Public Key event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length.
********************************************************************************** */
static bleResult_t IdsTest_ModifyInvalidPublicKey(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t publicKeyOffset = 0U;
    const uint16_t publicKeySize = 64U;

    do
    {
         /* Only modify Pairing Public Key */
        if (smpOpcode != gSmpOpcodePairingPublicKey_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to Public Key data: HCI(4) + L2CAP(4) + Opcode(1) = 9 */
        publicKeyOffset = gHciAclDataPacketHeaderSize_c + 
                                    gL2capHeaderSize_c + 
                                    1U;

        /* Validate packet length */
        if (packetLength < (publicKeyOffset + publicKeySize))
        {
            result =  gBleInvalidParameter_c;
            break;
        }

        /* Set public key to invalid value */
        FLib_MemSet(&pPacket[publicKeyOffset], 0xFF, publicKeySize);

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger Forced Re-pairing event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyForcedRepairing(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t authReqOffset = 0U;

    do
    {
        /* Only modify Pairing Request */
        if (smpOpcode != gSmpOpcodePairingRequest_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to Auth Req field: HCI(4) + L2CAP(4) + Opcode(1) + IO(1) + OOB(1) = 11 */
        authReqOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 3U;

        /* Validate packet length */
        if (packetLength <= authReqOffset)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Clear bonding bit to force re-pairing */
        pPacket[authReqOffset] &= 0xFEU;

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger KNOB (Key Negotiation of Bluetooth).
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyKNOB(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t maxKeyOffset = 0U;

    do
    {
        /* Only modify Pairing Request (0x01) */
        if (smpOpcode != gSmpOpcodePairingRequest_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* HCI(4) + L2CAP(4) + SMP Opcode(1) + IO(1) + OOB(1) + Auth(1) = 12 */
        maxKeyOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 4U;

        /* Validate packet length */
        if (packetLength <= maxKeyOffset)
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Set Max Encryption Key Size to 0x01 */
        pPacket[maxKeyOffset] = 0x01U;

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger Wrong Confirm Value event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyWrongConfirmValue(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t dhKeyCheckOffset = 0U;
    const uint16_t dhKeyCheckSize = 16U;

    do
    {
        /* Modify Pairing DHKey Check (0x0D) to trigger detection */
        if (smpOpcode != gSmpOpcodePairingDHKeyCheck_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to DHKey Check Value: HCI(4) + L2CAP(4) + Opcode(1) = 9 */
        dhKeyCheckOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 1U;

        /* Validate packet length */
        if (packetLength < (dhKeyCheckOffset + dhKeyCheckSize))
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Set wrong DHKey Check value (all 0xAA) - triggers DHKey check failure */
        FLib_MemSet(&pPacket[dhKeyCheckOffset], 0xAAU, dhKeyCheckSize);

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}

/*! *********************************************************************************
*\brief        Modify SMP message to trigger DH Key Check Failed event.
*
*\param        [in]  pPacket              Pointer to HCI ACL data packet.
*\param        [in]  packetLength         Packet length in bytes.
*\param        [in]  smpOpcode            SMP opcode from the packet.
*
*\retval       gBleSuccess_c              Message modified successfully.
*\retval       gBleInvalidParameter_c     Invalid packet length or wrong SMP opcode.
********************************************************************************** */
static bleResult_t IdsTest_ModifyDHKeyCheckFailed(uint8_t* pPacket, uint16_t packetLength, uint8_t smpOpcode)
{
    bleResult_t result = gBleSuccess_c;
    uint16_t dhKeyCheckOffset = 0U;
    const uint16_t dhKeyCheckSize = 16U;

    do
    {
        /* Only modify Pairing DHKey Check */
        if (smpOpcode != gSmpOpcodePairingDHKeyCheck_c)
        {
            result = gBleSuccess_c;
            break;
        }

        /* Calculate offset to DHKey Check data: HCI(4) + L2CAP(4) + Opcode(1) = 9 */
        dhKeyCheckOffset = gHciAclDataPacketHeaderSize_c + gL2capHeaderSize_c + 1U;

        /* Validate packet length */
        if (packetLength < (dhKeyCheckOffset + dhKeyCheckSize))
        {
            result = gBleInvalidParameter_c;
            break;
        }

        /* Set wrong DHKey Check value */
        FLib_MemSet(&pPacket[dhKeyCheckOffset], 0xFFU, dhKeyCheckSize);

        /* Deactivate hook */
        IdsTest_DeactivatePairingHook();

    } while(FALSE);

    return result;
}
#endif /* defined(gIntrusionDetectionSystemTestMode_d) && (gIntrusionDetectionSystemTestMode_d == 1U) */