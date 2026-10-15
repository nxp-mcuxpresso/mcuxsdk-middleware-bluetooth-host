/*! *********************************************************************************
 * \addtogroup AUTO
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2022-2026 NXP
*
*
* \file app_serial_interface.h
*
* This is a source file for the Anchor to Anchor serial interface.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef APP_A2A_INTERFACE_H
#define APP_A2A_INTERFACE_H
/*! *********************************************************************************
*************************************************************************************
* Include
*************************************************************************************
********************************************************************************** */
#include "EmbeddedTypes.h"
#include "ble_general.h"
#include "fsl_component_serial_manager.h"

/*************************************************************************************
**************************************************************************************
* Public macros
**************************************************************************************
*************************************************************************************/

#define gA2ACommandStartMarker_c                    0x02
#define gA2ACommandsOpGroup_c                       0xCC

/* A2A node addressing for the shared multi-drop bus. Main is the bus master. */
/* Anchor IDs are assigned at runtime via the "setid" shell command (RAM only). */
#define gA2ANodeIdMain_c                            0x00U   /* Bus master / main anchor */
#define gA2ANodeIdBroadcast_c                       0xFFU   /* Addresses every anchor at once */
#define gA2ANodeIdUnassigned_c                      0xFEU   /* Default before setid */

/* A2A commands identifier */
#define gSetBdCommandOpCode_c                           0x00    /* S1 -> S2 - transfer bonding data automatically at owner pairing (substitute for setbd shell command) */
#define gHandoverApplicationDataCommandOpCode_c         0x01    /* S1 -> S2 - Send Application Level Data (peer handle values, modified attribute values) */
/* CS sniffing / CS calibration A2A command opcodes (0x02..0x0B) are defined in the
   private add-on header app_a2a_sniffing_interface.h (bluetooth_private). */

/* A2A commands length */
#if (defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U))
#if defined(gHandoverIncluded_d) && (gHandoverIncluded_d > 0U)
#define gHandoverSetBdCommandLen_c                  106U
#else
#define gHandoverSetBdCommandLen_c                  90U
#endif /* defined(gHandoverIncluded_d) && (gHandoverIncluded_d > 0U) */
#else
#define  gHandoverSetBdCommandLen_c                 42U
#endif /* ((defined(gAppSecureMode_d) && (gAppSecureMode_d > 0U)) */

/* A2A commands */
/* Bonding data is provisioned to every anchor on the bus. */
#define A2A_SendSetBondingDataCommand(buf, len)                 A2A_SendCommandTo(gA2ANodeIdBroadcast_c, gA2ACommandsOpGroup_c, gSetBdCommandOpCode_c, buf, len)

#define A2A_SendApplicationDataCommand(buf, len)                A2A_SendCommand(gA2ACommandsOpGroup_c, gHandoverApplicationDataCommandOpCode_c, buf, len)
/* CS sniffing / CS calibration A2A send helpers (A2A_SendCsConfigDataCommand,
   A2A_SendCsCalStartCommand, ...) are defined in the private add-on header
   app_a2a_sniffing_interface.h (bluetooth_private), on top of A2A_SendCommandTo. */

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

typedef uint8_t clientPacketStatus_t;
typedef uint8_t opCode_t;
typedef uint8_t opGroup_t;

typedef PACKED_STRUCT clientPacketHdr_tag
{
    uint8_t    startMarker;
    uint8_t    dstId;    /* Destination node ID (or broadcast) */
    uint8_t    srcId;    /* Sender node ID */
    opGroup_t  opGroup;
    opCode_t   opCode;
    uint16_t   len;      /* Actual length of payload[] */
} clientPacketHdr_t;

/* The terminal checksum is actually stored at payload[len]. The checksum */
/* field ensures that there is always space for it, even if the payload */
/* is full. */
typedef PACKED_STRUCT clientPacketStructured_tag
{
    clientPacketHdr_t header;
    uint8_t payload[1];
} clientPacketStructured_t;

/* defines the working packet type */
typedef PACKED_UNION clientPacket_tag
{
    /* The entire packet as unformatted data. */
    uint8_t raw[sizeof(clientPacketStructured_t)];
    /* The packet as header + payload. */
    clientPacketStructured_t structured;
    /* A minimal packet with only a status value as a payload. */
    PACKED_STRUCT
    {                              /* The packet as header + payload. */
        clientPacketHdr_t header;
        clientPacketStatus_t status;
    } headerAndStatus;
} clientPacket_t;

typedef enum {
  PACKET_IS_VALID,
  PACKET_TOO_SHORT,
  FRAMING_ERROR,
  INTERNAL_ERROR
} packetStatus_t;

typedef void (*appA2ADataIndicationCb_t)(void *pMsg);

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*! *********************************************************************************
*\fn           bleResult_t A2A_Init(serial_handle_t pSerialHandle,
*                                   appA2ADataIndicationCb_t pfDataIndCb)
*\brief        Performs initialization of the Anchor to Anchor serial communication.
*
*\param  [in]  pSerialHandle    Serial interface handle to be used for communication.
*\param  [in]  pfDataIndCb      Application callback for received packets.
*
*\return       bleResult_t    Result of the operation.
********************************************************************************** */
bleResult_t A2A_Init
(
    serial_handle_t pSerialHandle,
    appA2ADataIndicationCb_t pfDataIndCb
);

/*! *********************************************************************************
*\fn           void A2A_SendCommand(uint8_t opGroup, uint8_t opCode, uint8_t *pPayload, uint16_t len)
*\brief        Send packet over the Anchor to Anchor interface.
*
*\param  [in]  opGroup      Group identifier.
*\param  [in]  opCode       Command identifier.
*\param  [in]  pPayload     Pointer to command data.
*\param  [in]  len          Command length.
*
*\retval       void.
********************************************************************************** */
void A2A_SendCommand
(
    uint8_t opGroup,
    uint8_t opCode,
    uint8_t *pPayload,
    uint16_t len
);

/*! *********************************************************************************
*\fn           void A2A_SendCommandTo(uint8_t dstId, uint8_t opGroup, uint8_t opCode,
*                                      uint8_t *pPayload, uint16_t len)
*\brief        Send an addressed packet over the Anchor to Anchor interface.
*
*\param  [in]  dstId        Destination node ID (or gA2ANodeIdBroadcast_c).
*\param  [in]  opGroup      Group identifier.
*\param  [in]  opCode       Command identifier.
*\param  [in]  pPayload     Pointer to command data.
*\param  [in]  len          Command length.
*
*\retval       void.
********************************************************************************** */
void A2A_SendCommandTo
(
    uint8_t dstId,
    uint8_t opGroup,
    uint8_t opCode,
    uint8_t *pPayload,
    uint16_t len
);

/*! *********************************************************************************
*\fn           void A2A_SetLocalId(uint8_t id)
*\brief        Set this node's A2A address (RAM only). Main keeps gA2ANodeIdMain_c;
*              Other anchors assign a unique ID via the setid shell command.
*
*\param  [in]  id           Local node ID.
*
*\retval       void.
********************************************************************************** */
void A2A_SetLocalId(uint8_t id);

/*! *********************************************************************************
*\fn           uint8_t A2A_GetLocalId(void)
*\brief        Get this node's A2A address.
*
*\retval       uint8_t      Local node ID.
********************************************************************************** */
uint8_t A2A_GetLocalId(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* APP_A2A_INTERFACE_H */