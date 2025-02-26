/* Source file generated from BLE.xml */
 /*
 * Copyright 2024 NXP
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*==================================================================================================
Include Files
==================================================================================================*/
#include "host_cmd_ble.h"

/*==================================================================================================
Public Functions
==================================================================================================*/
#if HCI_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t HCIModeSelectRequest(HCIModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable HCI

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t HCIModeSelectRequest(HCIModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x40, 0x00, (void*)req, sizeof(HCIModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t HCICommandRequest(HCICommandRequest_t *req, uint8_t fsciInterface)
\brief	Send a HCI command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t HCICommandRequest(HCICommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* CommandLength */
	msgLen += req->CommandLength;  /* Command */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->CommandLength), sizeof(req->CommandLength)); idx += sizeof(req->CommandLength);
	FLib_MemCpy(pMsg + idx, req->Command, req->CommandLength); idx += req->CommandLength;

	/* Send the request */
	FSCI_transmitPayload(0x40, 0x01, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t HCIDataRequest(HCIDataRequest_t *req, uint8_t fsciInterface)
\brief	Send a HCI data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t HCIDataRequest(HCIDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* DataLength */
	msgLen += req->DataLength;  /* Data */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->DataLength), sizeof(req->DataLength)); idx += sizeof(req->DataLength);
	FLib_MemCpy(pMsg + idx, req->Data, req->DataLength); idx += req->DataLength;

	/* Send the request */
	FSCI_transmitPayload(0x40, 0x02, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t HCISynchronousDataRequest(HCISynchronousDataRequest_t *req, uint8_t fsciInterface)
\brief	Send a HCI synchronous data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t HCISynchronousDataRequest(HCISynchronousDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* SynchronousDataLength */
	msgLen += req->SynchronousDataLength;  /* SynchronousData */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->SynchronousDataLength), sizeof(req->SynchronousDataLength)); idx += sizeof(req->SynchronousDataLength);
	FLib_MemCpy(pMsg + idx, req->SynchronousData, req->SynchronousDataLength); idx += req->SynchronousDataLength;

	/* Send the request */
	FSCI_transmitPayload(0x40, 0x03, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* HCI_ENABLE */

#if L2CAP_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t L2CAPModeSelectRequest(L2CAPModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable L2CAP

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPModeSelectRequest(L2CAPModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x41, 0x00, (void*)req, sizeof(L2CAPModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPInitRequest(uint8_t fsciInterface)
\brief	L2CAP initialization function

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t L2CAPInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x41, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPConfigRequest(L2CAPConfigRequest_t *req, uint8_t fsciInterface)
\brief	Config the L2CAP module

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPConfigRequest(L2CAPConfigRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x41, 0x02, (void*)req, sizeof(L2CAPConfigRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPSendAttDataRequest(L2CAPSendAttDataRequest_t *req, uint8_t fsciInterface)
\brief	Sends a data packet through ATT Channel

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPSendAttDataRequest(L2CAPSendAttDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* PacketLength */
	msgLen += req->PacketLength;  /* Packet */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->PacketLength), sizeof(req->PacketLength)); idx += sizeof(req->PacketLength);
	FLib_MemCpy(pMsg + idx, req->Packet, req->PacketLength); idx += req->PacketLength;

	/* Send the request */
	FSCI_transmitPayload(0x41, 0x03, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPSendSmpDataRequest(L2CAPSendSmpDataRequest_t *req, uint8_t fsciInterface)
\brief	Sends a data packet through SM Channel

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPSendSmpDataRequest(L2CAPSendSmpDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* PacketLength */
	msgLen += req->PacketLength;  /* Packet */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->PacketLength), sizeof(req->PacketLength)); idx += sizeof(req->PacketLength);
	FLib_MemCpy(pMsg + idx, req->Packet, req->PacketLength); idx += req->PacketLength;

	/* Send the request */
	FSCI_transmitPayload(0x41, 0x04, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPSendSignalingDataRequest(L2CAPSendSignalingDataRequest_t *req, uint8_t fsciInterface)
\brief	Sends a data packet through Signaling Channel

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPSendSignalingDataRequest(L2CAPSendSignalingDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* PacketLength */
	msgLen += req->PacketLength;  /* Packet */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->PacketLength), sizeof(req->PacketLength)); idx += sizeof(req->PacketLength);
	FLib_MemCpy(pMsg + idx, req->Packet, req->PacketLength); idx += req->PacketLength;

	/* Send the request */
	FSCI_transmitPayload(0x41, 0x05, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPRegisterAttCallbackRequest(uint8_t fsciInterface)
\brief	Register callback for receiving ATT data

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t L2CAPRegisterAttCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x41, 0x06, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPRegisterSmpCallbackRequest(uint8_t fsciInterface)
\brief	Register callback for receiving SM data

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t L2CAPRegisterSmpCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x41, 0x07, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPEnableL2capConnParamReqProcedureRequest(L2CAPEnableL2capConnParamReqProcedureRequest_t *req, uint8_t fsciInterface)
\brief	Set the gHciConnectionParametersRequestProcedure_c bit of the local supported features

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPEnableL2capConnParamReqProcedureRequest(L2CAPEnableL2capConnParamReqProcedureRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x41, 0x08, (void*)req, sizeof(L2CAPEnableL2capConnParamReqProcedureRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* L2CAP_ENABLE */

#if L2CAPCB_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBModeSelectRequest(L2CAPCBModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable L2CAP

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBModeSelectRequest(L2CAPCBModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x00, (void*)req, sizeof(L2CAPCBModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBRegisterLeCbCallbacksRequest(uint8_t fsciInterface)
\brief	Registers callbacks for credit based data and control events on L2CAP.

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t L2CAPCBRegisterLeCbCallbacksRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x42, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBRegisterLePsmRequest(L2CAPCBRegisterLePsmRequest_t *req, uint8_t fsciInterface)
\brief	Register LePsm

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBRegisterLePsmRequest(L2CAPCBRegisterLePsmRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x02, (void*)req, sizeof(L2CAPCBRegisterLePsmRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBDeregisterLePsmRequest(L2CAPCBDeregisterLePsmRequest_t *req, uint8_t fsciInterface)
\brief	Deregister LePsm

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBDeregisterLePsmRequest(L2CAPCBDeregisterLePsmRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x03, (void*)req, sizeof(L2CAPCBDeregisterLePsmRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBConnectLePsmRequest(L2CAPCBConnectLePsmRequest_t *req, uint8_t fsciInterface)
\brief	L2CAP connect

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBConnectLePsmRequest(L2CAPCBConnectLePsmRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x04, (void*)req, sizeof(L2CAPCBConnectLePsmRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBDisconnectLeCbChannelRequest(L2CAPCBDisconnectLeCbChannelRequest_t *req, uint8_t fsciInterface)
\brief	L2CAP disconnect

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBDisconnectLeCbChannelRequest(L2CAPCBDisconnectLeCbChannelRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x05, (void*)req, sizeof(L2CAPCBDisconnectLeCbChannelRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBCancelConnectionRequest(L2CAPCBCancelConnectionRequest_t *req, uint8_t fsciInterface)
\brief	Termination of an L2CAP channel

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBCancelConnectionRequest(L2CAPCBCancelConnectionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x06, (void*)req, sizeof(L2CAPCBCancelConnectionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBSendLeCbDataRequest(L2CAPCBSendLeCbDataRequest_t *req, uint8_t fsciInterface)
\brief	Sends a data packet through a Credit Based Channel

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBSendLeCbDataRequest(L2CAPCBSendLeCbDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* ChannelId */
	msgLen += sizeof(uint16_t);  /* PacketLength */
	msgLen += req->PacketLength;  /* Packet */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->ChannelId), sizeof(req->ChannelId)); idx += sizeof(req->ChannelId);
	FLib_MemCpy(pMsg + idx, &(req->PacketLength), sizeof(req->PacketLength)); idx += sizeof(req->PacketLength);
	FLib_MemCpy(pMsg + idx, req->Packet, req->PacketLength); idx += req->PacketLength;

	/* Send the request */
	FSCI_transmitPayload(0x42, 0x07, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBSendLeCreditRequest(L2CAPCBSendLeCreditRequest_t *req, uint8_t fsciInterface)
\brief	Sends credits to a device when capable of receiving additional LE-frames

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBSendLeCreditRequest(L2CAPCBSendLeCreditRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x42, 0x08, (void*)req, sizeof(L2CAPCBSendLeCreditRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBEnhancedConnectLePsmRequest(L2CAPCBEnhancedConnectLePsmRequest_t *req, uint8_t fsciInterface)
\brief	Initiates a connection with a peer device for a registered LE_PSM, enhanced mode

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBEnhancedConnectLePsmRequest(L2CAPCBEnhancedConnectLePsmRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* LePsm */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Mtu */
	msgLen += sizeof(uint16_t);  /* InitialCredits */
	msgLen += sizeof(uint8_t);  /* NoOfChannels */
	msgLen += req->NoOfChannels;  /* Cids */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->LePsm), sizeof(req->LePsm)); idx += sizeof(req->LePsm);
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Mtu), sizeof(req->Mtu)); idx += sizeof(req->Mtu);
	FLib_MemCpy(pMsg + idx, &(req->InitialCredits), sizeof(req->InitialCredits)); idx += sizeof(req->InitialCredits);
	pMsg[idx] = req->NoOfChannels; idx++;
	FLib_MemCpy(pMsg + idx, req->Cids, req->NoOfChannels); idx += req->NoOfChannels;

	/* Send the request */
	FSCI_transmitPayload(0x42, 0x09, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBEnhancedChannelReconfigureRequest(L2CAPCBEnhancedChannelReconfigureRequest_t *req, uint8_t fsciInterface)
\brief	Reconfigures up to 5 channels with new values for MTU and/or MPS

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBEnhancedChannelReconfigureRequest(L2CAPCBEnhancedChannelReconfigureRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* NewMtu */
	msgLen += sizeof(uint16_t);  /* NewMps */
	msgLen += sizeof(uint8_t);  /* NoOfChannels */
	msgLen += req->NoOfChannels;  /* Cids */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->NewMtu), sizeof(req->NewMtu)); idx += sizeof(req->NewMtu);
	FLib_MemCpy(pMsg + idx, &(req->NewMps), sizeof(req->NewMps)); idx += sizeof(req->NewMps);
	pMsg[idx] = req->NoOfChannels; idx++;
	FLib_MemCpy(pMsg + idx, req->Cids, req->NoOfChannels); idx += req->NoOfChannels;

	/* Send the request */
	FSCI_transmitPayload(0x42, 0x0A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t L2CAPCBEnhancedCancelConnectionRequest(L2CAPCBEnhancedCancelConnectionRequest_t *req, uint8_t fsciInterface)
\brief	Terminates pending L2CAP channels

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t L2CAPCBEnhancedCancelConnectionRequest(L2CAPCBEnhancedCancelConnectionRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* LePsm */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* RefuseReason */
	msgLen += sizeof(uint8_t);  /* NoOfChannels */
	msgLen += req->NoOfChannels;  /* Cids */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->LePsm), sizeof(req->LePsm)); idx += sizeof(req->LePsm);
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &req->RefuseReason, sizeof(uint16_t)); idx += sizeof(uint16_t);
	pMsg[idx] = req->NoOfChannels; idx++;
	FLib_MemCpy(pMsg + idx, req->Cids, req->NoOfChannels); idx += req->NoOfChannels;

	/* Send the request */
	FSCI_transmitPayload(0x42, 0x0B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* L2CAPCB_ENABLE */

#if SM_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t SMModeSelectRequest(SMModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable SM

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMModeSelectRequest(SMModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x00, (void*)req, sizeof(SMModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMInitRequest(uint8_t fsciInterface)
\brief	Security Manager initialization function

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t SMInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x43, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMRegisterUserCallbackRequest(uint8_t fsciInterface)
\brief	Security Manager function for registering an upper layer callback

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t SMRegisterUserCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x43, 0x02, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMInitiatorStartPairingRequest(SMInitiatorStartPairingRequest_t *req, uint8_t fsciInterface)
\brief	Initiate the SMP pairing procedure on a Central

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMInitiatorStartPairingRequest(SMInitiatorStartPairingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x03, (void*)req, sizeof(SMInitiatorStartPairingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMSendSecurityRequestRequest(SMSendSecurityRequestRequest_t *req, uint8_t fsciInterface)
\brief	Send a SMP security request from a Peripheral to a Central

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMSendSecurityRequestRequest(SMSendSecurityRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x04, (void*)req, sizeof(SMSendSecurityRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMTerminatePairingRequest(SMTerminatePairingRequest_t *req, uint8_t fsciInterface)
\brief	Terminates a remotely or locally started pairing procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMTerminatePairingRequest(SMTerminatePairingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x05, (void*)req, sizeof(SMTerminatePairingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMPairingRequestReplyRequest(SMPairingRequestReplyRequest_t *req, uint8_t fsciInterface)
\brief	Respond to a remote pairing request with the parameters required for the pairing procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMPairingRequestReplyRequest(SMPairingRequestReplyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x06, (void*)req, sizeof(SMPairingRequestReplyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMPasskeyRequestReplyRequest(SMPasskeyRequestReplyRequest_t *req, uint8_t fsciInterface)
\brief	Respond to a Passkey Request Event from the SM with the passkey (0 to 999.999 decimal or 16 bytes OOB)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMPasskeyRequestReplyRequest(SMPasskeyRequestReplyRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DestinationDeviceId */
	msgLen += sizeof(uint8_t);  /* KeyType */

	switch (req->SmPasskeyReqReplyParams.KeyType)
	{
		case SMPasskeyRequestReplyRequest_SmPasskeyReqReplyParams_KeyType_Passkey:
			msgLen += sizeof(uint32_t);  /* Passkey */
			break;

		case SMPasskeyRequestReplyRequest_SmPasskeyReqReplyParams_KeyType_Oob:
			msgLen += sizeof(uint32_t);  /* Passkey */
			break;
	}	/* Key */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DestinationDeviceId; idx++;
	pMsg[idx] = req->SmPasskeyReqReplyParams.KeyType; idx++;

	switch (req->SmPasskeyReqReplyParams.KeyType)
	{
		case SMPasskeyRequestReplyRequest_SmPasskeyReqReplyParams_KeyType_Passkey:
			FLib_MemCpy(pMsg + idx, &(req->SmPasskeyReqReplyParams.Key.Passkey), sizeof(req->SmPasskeyReqReplyParams.Key.Passkey)); idx += sizeof(req->SmPasskeyReqReplyParams.Key.Passkey);
			break;

		case SMPasskeyRequestReplyRequest_SmPasskeyReqReplyParams_KeyType_Oob:
			FLib_MemCpy(pMsg + idx, &(req->SmPasskeyReqReplyParams.Key.Passkey), sizeof(req->SmPasskeyReqReplyParams.Key.Passkey)); idx += sizeof(req->SmPasskeyReqReplyParams.Key.Passkey);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x43, 0x07, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMPairingKeysetRequestReplyRequest(SMPairingKeysetRequestReplyRequest_t *req, uint8_t fsciInterface)
\brief	Respond to a Pairing Keyset Request Event from the SM with the keyset and additional data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMPairingKeysetRequestReplyRequest(SMPairingKeysetRequestReplyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x08, (void*)req, sizeof(SMPairingKeysetRequestReplyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLlStartEncryptionRequest(SMLlStartEncryptionRequest_t *req, uint8_t fsciInterface)
\brief	Start the encryption of the Link Layer connection to the specified device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMLlStartEncryptionRequest(SMLlStartEncryptionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x09, (void*)req, sizeof(SMLlStartEncryptionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLlLtkRequestReplyRequest(SMLlLtkRequestReplyRequest_t *req, uint8_t fsciInterface)
\brief	Respond to a Link Layer LTK request for the encryption of the connection to the specified device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMLlLtkRequestReplyRequest(SMLlLtkRequestReplyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x0A, (void*)req, sizeof(SMLlLtkRequestReplyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLlLtkRequestNegativeReplyRequest(SMLlLtkRequestNegativeReplyRequest_t *req, uint8_t fsciInterface)
\brief	Reject a Link Layer LTK request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMLlLtkRequestNegativeReplyRequest(SMLlLtkRequestNegativeReplyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x0B, (void*)req, sizeof(SMLlLtkRequestNegativeReplyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLlEncryptRequestRequest(SMLlEncryptRequestRequest_t *req, uint8_t fsciInterface)
\brief	Request an AES-128 encryption of a 128 bit block providing a 128 bit key

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMLlEncryptRequestRequest(SMLlEncryptRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x0C, (void*)req, sizeof(SMLlEncryptRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLlRandRequestRequest(uint8_t fsciInterface)
\brief	Request the generation of a 64 bit random number

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t SMLlRandRequestRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x43, 0x0D, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMTbCreateRandomDeviceAddressRequestRequest(SMTbCreateRandomDeviceAddressRequestRequest_t *req, uint8_t fsciInterface)
\brief	Request the creation of a BLE random device address

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMTbCreateRandomDeviceAddressRequestRequest(SMTbCreateRandomDeviceAddressRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* AddrType */

	switch (req->AddrType)
	{
		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_RandAddrTypeNone:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_StaticAddr:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_NonResolvablePrivateAddr:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_ResolvablePrivateAddr:
			msgLen += 16;  /* IRK */
			msgLen += sizeof(bool_t);  /* RandIncluded */

			if (req->Data.ResolvablePrivateAddr.RandIncluded)
			{
				msgLen += 3;
			}  /* Rand */

			break;
	}	/* Data */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->AddrType; idx++;

	switch (req->AddrType)
	{
		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_RandAddrTypeNone:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_StaticAddr:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_NonResolvablePrivateAddr:
			break;

		case SMTbCreateRandomDeviceAddressRequestRequest_AddrType_ResolvablePrivateAddr:
			FLib_MemCpy(pMsg + idx, req->Data.ResolvablePrivateAddr.IRK, 16); idx += 16;
			pMsg[idx] = req->Data.ResolvablePrivateAddr.RandIncluded; idx++;

			if (req->Data.ResolvablePrivateAddr.RandIncluded)
			{
				FLib_MemCpy(pMsg + idx, req->Data.ResolvablePrivateAddr.Rand, 3); idx += 3;
			}

			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x43, 0x0E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMTbCheckResolvablePrivateAddressRequestRequest(SMTbCheckResolvablePrivateAddressRequestRequest_t *req, uint8_t fsciInterface)
\brief	Request to check a Resolvable Private Address against an IRK

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMTbCheckResolvablePrivateAddressRequestRequest(SMTbCheckResolvablePrivateAddressRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x0F, (void*)req, sizeof(SMTbCheckResolvablePrivateAddressRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMTbSignDataRequestRequest(SMTbSignDataRequestRequest_t *req, uint8_t fsciInterface)
\brief	Request to calculate an Authentication Signature for a block of data using a provided CSRK

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMTbSignDataRequestRequest(SMTbSignDataRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += 16;  /* CSRK */
	msgLen += sizeof(uint16_t);  /* DataLength */
	msgLen += req->DataLength;  /* Data */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, req->CSRK, 16); idx += 16;
	FLib_MemCpy(pMsg + idx, &(req->DataLength), sizeof(req->DataLength)); idx += sizeof(req->DataLength);
	FLib_MemCpy(pMsg + idx, req->Data, req->DataLength); idx += req->DataLength;

	/* Send the request */
	FSCI_transmitPayload(0x43, 0x10, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMTbVerifyDataSignatureRequestRequest(SMTbVerifyDataSignatureRequestRequest_t *req, uint8_t fsciInterface)
\brief	Request to verify an Authentication Signature for a block of data using a provided CSRK

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMTbVerifyDataSignatureRequestRequest(SMTbVerifyDataSignatureRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += 16;  /* CSRK */
	msgLen += sizeof(uint16_t);  /* DataLength */
	msgLen += req->DataLength;  /* Data */
	msgLen += 8;  /* Signature */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, req->CSRK, 16); idx += 16;
	FLib_MemCpy(pMsg + idx, &(req->DataLength), sizeof(req->DataLength)); idx += sizeof(req->DataLength);
	FLib_MemCpy(pMsg + idx, req->Data, req->DataLength); idx += req->DataLength;
	FLib_MemCpy(pMsg + idx, req->Signature, 8); idx += 8;

	/* Send the request */
	FSCI_transmitPayload(0x43, 0x11, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMDeviceConnectNotificationRequest(SMDeviceConnectNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Send a Device Connection Notification to the SM when a LL connection is created with the specified device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMDeviceConnectNotificationRequest(SMDeviceConnectNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x12, (void*)req, sizeof(SMDeviceConnectNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMDeviceDisconnectNotificationRequest(SMDeviceDisconnectNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Send a Device Disconnection Notification to the SM when the LL connection with the specified device is terminated

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMDeviceDisconnectNotificationRequest(SMDeviceDisconnectNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x13, (void*)req, sizeof(SMDeviceDisconnectNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMSetDefaultPasskeyRequest(SMSetDefaultPasskeyRequest_t *req, uint8_t fsciInterface)
\brief	Set a default passkey in the SM

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMSetDefaultPasskeyRequest(SMSetDefaultPasskeyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x14, (void*)req, sizeof(SMSetDefaultPasskeyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMSetOobMitmProtectionRequest(SMSetOobMitmProtectionRequest_t *req, uint8_t fsciInterface)
\brief	Set the MITM protection property of the OOB pairing method

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMSetOobMitmProtectionRequest(SMSetOobMitmProtectionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x15, (void*)req, sizeof(SMSetOobMitmProtectionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMSendKeypressNotificationRequest(SMSendKeypressNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Send a Keypress Notification to a peer when pairing using the Passkey Entry pairing method

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMSendKeypressNotificationRequest(SMSendKeypressNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x16, (void*)req, sizeof(SMSendKeypressNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMNcDisplayConfirmRequest(SMNcDisplayConfirmRequest_t *req, uint8_t fsciInterface)
\brief	Send a Numeric Comparison display confirmation when pairing using the LE SC Numeric Comparison pairing method

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMNcDisplayConfirmRequest(SMNcDisplayConfirmRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x17, (void*)req, sizeof(SMNcDisplayConfirmRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLeScOobDataRequestReplyRequest(SMLeScOobDataRequestReplyRequest_t *req, uint8_t fsciInterface)
\brief	Provide the SM with the remote LE SC OOB Data when pairing using the LE SC OOB pairing method

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMLeScOobDataRequestReplyRequest(SMLeScOobDataRequestReplyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x18, (void*)req, sizeof(SMLeScOobDataRequestReplyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMLocalLeScOobDataRequestReqRequest(uint8_t fsciInterface)
\brief	Request from the SM the local LE SC OOB Data when pairing using the LE SC OOB pairing method

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t SMLocalLeScOobDataRequestReqRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x43, 0x19, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMGenerateNewEcdhPkSkPairRequest(SMGenerateNewEcdhPkSkPairRequest_t *req, uint8_t fsciInterface)
\brief	Generate a new ECDH Public Key - Secret Key pair in the Security Manager

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMGenerateNewEcdhPkSkPairRequest(SMGenerateNewEcdhPkSkPairRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x1A, (void*)req, sizeof(SMGenerateNewEcdhPkSkPairRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t SMSetMinPairingSecurityPropertiesRequest(SMSetMinPairingSecurityPropertiesRequest_t *req, uint8_t fsciInterface)
\brief	Set the minimum SMP Pairing security properties in the Security Manager

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t SMSetMinPairingSecurityPropertiesRequest(SMSetMinPairingSecurityPropertiesRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x43, 0x1B, (void*)req, sizeof(SMSetMinPairingSecurityPropertiesRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* SM_ENABLE */

#if ATT_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t ATTModeSelectRequest(ATTModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable ATT

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTModeSelectRequest(ATTModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x00, (void*)req, sizeof(ATTModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTInitRequest(uint8_t fsciInterface)
\brief	ATT initialization function

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t ATTInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x44, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTNotifyConnectionRequest(ATTNotifyConnectionRequest_t *req, uint8_t fsciInterface)
\brief	ATT notify connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTNotifyConnectionRequest(ATTNotifyConnectionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x02, (void*)req, sizeof(ATTNotifyConnectionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTNotifyDisconnectionRequest(ATTNotifyDisconnectionRequest_t *req, uint8_t fsciInterface)
\brief	ATT notify disconnection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTNotifyDisconnectionRequest(ATTNotifyDisconnectionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x03, (void*)req, sizeof(ATTNotifyDisconnectionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTSetMtuRequest(ATTSetMtuRequest_t *req, uint8_t fsciInterface)
\brief	ATT set MTU

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTSetMtuRequest(ATTSetMtuRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x04, (void*)req, sizeof(ATTSetMtuRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTGetMtuRequest(ATTGetMtuRequest_t *req, uint8_t fsciInterface)
\brief	ATT get MTU

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTGetMtuRequest(ATTGetMtuRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x05, (void*)req, sizeof(ATTGetMtuRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTRegisterOpcodeCallbackRequest(ATTRegisterOpcodeCallbackRequest_t *req, uint8_t fsciInterface)
\brief	ATT register callback for opcode

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTRegisterOpcodeCallbackRequest(ATTRegisterOpcodeCallbackRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x06, (void*)req, sizeof(ATTRegisterOpcodeCallbackRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTRegisterUnsupportedOpcodeCallbackRequest(uint8_t fsciInterface)
\brief	ATT register callback for unsupported opcode

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t ATTRegisterUnsupportedOpcodeCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x44, 0x07, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTRegisterTimeoutCallbackRequest(uint8_t fsciInterface)
\brief	ATT register callback for signalling timeout

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t ATTRegisterTimeoutCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x44, 0x08, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendErrorResponseRequest(ATTServerSendErrorResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send error response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendErrorResponseRequest(ATTServerSendErrorResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x09, (void*)req, sizeof(ATTServerSendErrorResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendExchangeMtuRequestRequest(ATTClientSendExchangeMtuRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send exchange MTU request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendExchangeMtuRequestRequest(ATTClientSendExchangeMtuRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x0A, (void*)req, sizeof(ATTClientSendExchangeMtuRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendExchangeMtuResponseRequest(ATTServerSendExchangeMtuResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send exchange MTU response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendExchangeMtuResponseRequest(ATTServerSendExchangeMtuResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x0B, (void*)req, sizeof(ATTServerSendExchangeMtuResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendFindInformationRequestRequest(ATTClientSendFindInformationRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send find information request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendFindInformationRequestRequest(ATTClientSendFindInformationRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x0C, (void*)req, sizeof(ATTClientSendFindInformationRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendFindInformationResponseRequest(ATTServerSendFindInformationResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send find information response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendFindInformationResponseRequest(ATTServerSendFindInformationResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case ATTServerSendFindInformationResponseRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint8_t);  /* HandleUuid16PairCount */

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				msgLen += sizeof(uint16_t);  /* Handle */
				msgLen += sizeof(uint16_t);  /* Uuid16 */
			}			/* HandleUuid16 */
			break;

		case ATTServerSendFindInformationResponseRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(uint8_t);  /* HandleUuid128PairCount */

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				msgLen += sizeof(uint16_t);  /* Handle */
				msgLen += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);  /* Uuid128 */
			}			/* HandleUuid128 */
			break;
	}	/* InformationData */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case ATTServerSendFindInformationResponseRequest_Params_Format_Uuid16BitFormat:
			pMsg[idx] = req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; idx++;

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle), sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle)); idx += sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle);
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16), sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16)); idx += sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16);
			}
			break;

		case ATTServerSendFindInformationResponseRequest_Params_Format_Uuid128BitFormat:
			pMsg[idx] = req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; idx++;

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle), sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle)); idx += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle);
				FLib_MemCpy(pMsg + idx, req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128, sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128)); idx += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);
			}
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x0D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendFindByTypeValueRequestRequest(ATTClientSendFindByTypeValueRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send find by type value request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendFindByTypeValueRequestRequest(ATTClientSendFindByTypeValueRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint16_t);  /* AttributeType */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType), sizeof(req->Params.AttributeType)); idx += sizeof(req->Params.AttributeType);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x0E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendFindByTypeValueResponseRequest(ATTServerSendFindByTypeValueResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send find by type value response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendFindByTypeValueResponseRequest(ATTServerSendFindByTypeValueResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* GroupCount */

	for (uint32_t i = 0; i < req->Params.GroupCount; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartingHandle */
		msgLen += sizeof(uint16_t);  /* EndingHandle */
	}	/* HandleGroup */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.GroupCount), sizeof(req->Params.GroupCount)); idx += sizeof(req->Params.GroupCount);

	for (uint32_t i = 0; i < req->Params.GroupCount; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Params.HandleGroup[i].StartingHandle), sizeof(req->Params.HandleGroup[i].StartingHandle)); idx += sizeof(req->Params.HandleGroup[i].StartingHandle);
		FLib_MemCpy(pMsg + idx, &(req->Params.HandleGroup[i].EndingHandle), sizeof(req->Params.HandleGroup[i].EndingHandle)); idx += sizeof(req->Params.HandleGroup[i].EndingHandle);
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x0F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadByTypeRequestRequest(ATTClientSendReadByTypeRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read by type request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadByTypeRequestRequest(ATTClientSendReadByTypeRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case ATTClientSendReadByTypeRequestRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case ATTClientSendReadByTypeRequestRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case ATTClientSendReadByTypeRequestRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case ATTClientSendReadByTypeRequestRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x10, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadByTypeResponseRequest(ATTServerSendReadByTypeResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read by type response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadByTypeResponseRequest(ATTServerSendReadByTypeResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Length */
	msgLen += sizeof(uint16_t);  /* AttributeDataListLength */
	msgLen += req->Params.AttributeDataListLength;  /* AttributeDataList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Params.Length; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeDataListLength), sizeof(req->Params.AttributeDataListLength)); idx += sizeof(req->Params.AttributeDataListLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeDataList, req->Params.AttributeDataListLength); idx += req->Params.AttributeDataListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x11, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadRequestRequest(ATTClientSendReadRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadRequestRequest(ATTClientSendReadRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x12, (void*)req, sizeof(ATTClientSendReadRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadResponseRequest(ATTServerSendReadResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadResponseRequest(ATTServerSendReadResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x13, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadBlobRequestRequest(ATTClientSendReadBlobRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read blob request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadBlobRequestRequest(ATTClientSendReadBlobRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x14, (void*)req, sizeof(ATTClientSendReadBlobRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadBlobResponseRequest(ATTServerSendReadBlobResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read blob response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadBlobResponseRequest(ATTServerSendReadBlobResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x15, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadMultipleRequestRequest(ATTClientSendReadMultipleRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read multiple request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadMultipleRequestRequest(ATTClientSendReadMultipleRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* HandleCount */
	msgLen += req->Params.HandleCount * sizeof(uint16_t);  /* ListOfHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.HandleCount), sizeof(req->Params.HandleCount)); idx += sizeof(req->Params.HandleCount);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfHandles, req->Params.HandleCount * sizeof(uint16_t)); idx += req->Params.HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x16, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadMultipleResponseRequest(ATTServerSendReadMultipleResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read multiple response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadMultipleResponseRequest(ATTServerSendReadMultipleResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* ListLength */
	msgLen += req->Params.ListLength;  /* ListOfValues */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.ListLength), sizeof(req->Params.ListLength)); idx += sizeof(req->Params.ListLength);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfValues, req->Params.ListLength); idx += req->Params.ListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x17, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadByGroupTypeRequestRequest(ATTClientSendReadByGroupTypeRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read by group type request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadByGroupTypeRequestRequest(ATTClientSendReadByGroupTypeRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case ATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case ATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case ATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case ATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x18, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadByGroupTypeResponseRequest(ATTServerSendReadByGroupTypeResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send read by group type response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadByGroupTypeResponseRequest(ATTServerSendReadByGroupTypeResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Length */
	msgLen += sizeof(uint16_t);  /* AttributeDataListLength */
	msgLen += req->Params.AttributeDataListLength;  /* AttributeDataList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Params.Length; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeDataListLength), sizeof(req->Params.AttributeDataListLength)); idx += sizeof(req->Params.AttributeDataListLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeDataList, req->Params.AttributeDataListLength); idx += req->Params.AttributeDataListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x19, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendWriteRequestRequest(ATTClientSendWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendWriteRequestRequest(ATTClientSendWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x1A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendWriteResponseRequest(ATTServerSendWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendWriteResponseRequest(ATTServerSendWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x1B, (void*)req, sizeof(ATTServerSendWriteResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendWriteCommandRequest(ATTClientSendWriteCommandRequest_t *req, uint8_t fsciInterface)
\brief	ATT send write command (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendWriteCommandRequest(ATTClientSendWriteCommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x1C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendSignedWriteCommandRequest(ATTClientSendSignedWriteCommandRequest_t *req, uint8_t fsciInterface)
\brief	ATT send signed write command (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendSignedWriteCommandRequest(ATTClientSendSignedWriteCommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */
	msgLen += 12;  /* AuthenticationSignature */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;
	FLib_MemCpy(pMsg + idx, req->Params.AuthenticationSignature, 12); idx += 12;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x1D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendPrepareWriteRequestRequest(ATTClientSendPrepareWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send prepare write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendPrepareWriteRequestRequest(ATTClientSendPrepareWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x1E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendPrepareWriteResponseRequest(ATTServerSendPrepareWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send prepare write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendPrepareWriteResponseRequest(ATTServerSendPrepareWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x1F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendExecuteWriteRequestRequest(ATTClientSendExecuteWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	ATT send execute write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendExecuteWriteRequestRequest(ATTClientSendExecuteWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x20, (void*)req, sizeof(ATTClientSendExecuteWriteRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendExecuteWriteResponseRequest(ATTServerSendExecuteWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	ATT send execute write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendExecuteWriteResponseRequest(ATTServerSendExecuteWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x21, (void*)req, sizeof(ATTServerSendExecuteWriteResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendHandleValueNotificationRequest(ATTServerSendHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	ATT send handle value notification (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendHandleValueNotificationRequest(ATTServerSendHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x22, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendHandleValueIndicationRequest(ATTServerSendHandleValueIndicationRequest_t *req, uint8_t fsciInterface)
\brief	ATT send handle value indication (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendHandleValueIndicationRequest(ATTServerSendHandleValueIndicationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x23, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendHandleValueConfirmationRequest(ATTClientSendHandleValueConfirmationRequest_t *req, uint8_t fsciInterface)
\brief	ATT send handle value confirmation (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendHandleValueConfirmationRequest(ATTClientSendHandleValueConfirmationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x24, (void*)req, sizeof(ATTClientSendHandleValueConfirmationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendMultipleHandleValueNotificationRequest(ATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	ATT send multiple handle value notification (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendMultipleHandleValueNotificationRequest(ATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x25, (void*)req, sizeof(ATTServerSendMultipleHandleValueNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTClientSendReadMultipleVariableRequestRequest(ATTClientSendReadMultipleVariableRequestRequest_t *req, uint8_t fsciInterface)
\brief	 ATT send read multiple variable request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTClientSendReadMultipleVariableRequestRequest(ATTClientSendReadMultipleVariableRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* HandleCount */
	msgLen += req->Params.HandleCount * sizeof(uint16_t);  /* ListOfHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.HandleCount), sizeof(req->Params.HandleCount)); idx += sizeof(req->Params.HandleCount);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfHandles, req->Params.HandleCount * sizeof(uint16_t)); idx += req->Params.HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x26, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ATTServerSendReadMultipleVariableResponseRequest(ATTServerSendReadMultipleVariableResponseRequest_t *req, uint8_t fsciInterface)
\brief	 ATT send read multiple variable response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ATTServerSendReadMultipleVariableResponseRequest(ATTServerSendReadMultipleVariableResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* ListLength */
	msgLen += req->Params.ListLength;  /* lengthValueTupleList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.ListLength), sizeof(req->Params.ListLength)); idx += sizeof(req->Params.ListLength);
	FLib_MemCpy(pMsg + idx, req->Params.lengthValueTupleList, req->Params.ListLength); idx += req->Params.ListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x27, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendErrorResponseRequest(EATTServerSendErrorResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send error response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendErrorResponseRequest(EATTServerSendErrorResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x28, (void*)req, sizeof(EATTServerSendErrorResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendFindInformationRequestRequest(EATTClientSendFindInformationRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send find information request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendFindInformationRequestRequest(EATTClientSendFindInformationRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x2B, (void*)req, sizeof(EATTClientSendFindInformationRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendFindInformationResponseRequest(EATTServerSendFindInformationResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send find information response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendFindInformationResponseRequest(EATTServerSendFindInformationResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case EATTServerSendFindInformationResponseRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint8_t);  /* HandleUuid16PairCount */

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				msgLen += sizeof(uint16_t);  /* Handle */
				msgLen += sizeof(uint16_t);  /* Uuid16 */
			}			/* HandleUuid16 */
			break;

		case EATTServerSendFindInformationResponseRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(uint8_t);  /* HandleUuid128PairCount */

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				msgLen += sizeof(uint16_t);  /* Handle */
				msgLen += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);  /* Uuid128 */
			}			/* HandleUuid128 */
			break;
	}	/* InformationData */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case EATTServerSendFindInformationResponseRequest_Params_Format_Uuid16BitFormat:
			pMsg[idx] = req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; idx++;

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid16BitFormat.HandleUuid16PairCount; i++)
			{
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle), sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle)); idx += sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Handle);
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16), sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16)); idx += sizeof(req->Params.InformationData.Uuid16BitFormat.HandleUuid16[i].Uuid16);
			}
			break;

		case EATTServerSendFindInformationResponseRequest_Params_Format_Uuid128BitFormat:
			pMsg[idx] = req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; idx++;

			for (uint32_t i = 0; i < req->Params.InformationData.Uuid128BitFormat.HandleUuid128PairCount; i++)
			{
				FLib_MemCpy(pMsg + idx, &(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle), sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle)); idx += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Handle);
				FLib_MemCpy(pMsg + idx, req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128, sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128)); idx += sizeof(req->Params.InformationData.Uuid128BitFormat.HandleUuid128[i].Uuid128);
			}
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x2C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendFindByTypeValueRequestRequest(EATTClientSendFindByTypeValueRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send find by type value request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendFindByTypeValueRequestRequest(EATTClientSendFindByTypeValueRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint16_t);  /* AttributeType */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType), sizeof(req->Params.AttributeType)); idx += sizeof(req->Params.AttributeType);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x2D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendFindByTypeValueResponseRequest(EATTServerSendFindByTypeValueResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send find by type value response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendFindByTypeValueResponseRequest(EATTServerSendFindByTypeValueResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* GroupCount */

	for (uint32_t i = 0; i < req->Params.GroupCount; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartingHandle */
		msgLen += sizeof(uint16_t);  /* EndingHandle */
	}	/* HandleGroup */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.GroupCount), sizeof(req->Params.GroupCount)); idx += sizeof(req->Params.GroupCount);

	for (uint32_t i = 0; i < req->Params.GroupCount; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Params.HandleGroup[i].StartingHandle), sizeof(req->Params.HandleGroup[i].StartingHandle)); idx += sizeof(req->Params.HandleGroup[i].StartingHandle);
		FLib_MemCpy(pMsg + idx, &(req->Params.HandleGroup[i].EndingHandle), sizeof(req->Params.HandleGroup[i].EndingHandle)); idx += sizeof(req->Params.HandleGroup[i].EndingHandle);
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x2E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadByTypeRequestRequest(EATTClientSendReadByTypeRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read by type request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadByTypeRequestRequest(EATTClientSendReadByTypeRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case EATTClientSendReadByTypeRequestRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case EATTClientSendReadByTypeRequestRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case EATTClientSendReadByTypeRequestRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case EATTClientSendReadByTypeRequestRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x2F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadByTypeResponseRequest(EATTServerSendReadByTypeResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read by type response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadByTypeResponseRequest(EATTServerSendReadByTypeResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Length */
	msgLen += sizeof(uint16_t);  /* AttributeDataListLength */
	msgLen += req->Params.AttributeDataListLength;  /* AttributeDataList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Params.Length; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeDataListLength), sizeof(req->Params.AttributeDataListLength)); idx += sizeof(req->Params.AttributeDataListLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeDataList, req->Params.AttributeDataListLength); idx += req->Params.AttributeDataListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x30, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadRequestRequest(EATTClientSendReadRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadRequestRequest(EATTClientSendReadRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x31, (void*)req, sizeof(EATTClientSendReadRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadResponseRequest(EATTServerSendReadResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadResponseRequest(EATTServerSendReadResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x32, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadBlobRequestRequest(EATTClientSendReadBlobRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read blob request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadBlobRequestRequest(EATTClientSendReadBlobRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x33, (void*)req, sizeof(EATTClientSendReadBlobRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadBlobResponseRequest(EATTServerSendReadBlobResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read blob response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadBlobResponseRequest(EATTServerSendReadBlobResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x34, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadMultipleRequestRequest(EATTClientSendReadMultipleRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read multiple request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadMultipleRequestRequest(EATTClientSendReadMultipleRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* HandleCount */
	msgLen += req->Params.HandleCount * sizeof(uint16_t);  /* ListOfHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.HandleCount), sizeof(req->Params.HandleCount)); idx += sizeof(req->Params.HandleCount);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfHandles, req->Params.HandleCount * sizeof(uint16_t)); idx += req->Params.HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x35, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadMultipleResponseRequest(EATTServerSendReadMultipleResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read multiple response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadMultipleResponseRequest(EATTServerSendReadMultipleResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* ListLength */
	msgLen += req->Params.ListLength;  /* ListOfValues */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.ListLength), sizeof(req->Params.ListLength)); idx += sizeof(req->Params.ListLength);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfValues, req->Params.ListLength); idx += req->Params.ListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x36, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadByGroupTypeRequestRequest(EATTClientSendReadByGroupTypeRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read by group type request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadByGroupTypeRequestRequest(EATTClientSendReadByGroupTypeRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case EATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case EATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case EATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case EATTClientSendReadByGroupTypeRequestRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x37, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadByGroupTypeResponseRequest(EATTServerSendReadByGroupTypeResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read by group type response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadByGroupTypeResponseRequest(EATTServerSendReadByGroupTypeResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Length */
	msgLen += sizeof(uint16_t);  /* AttributeDataListLength */
	msgLen += req->Params.AttributeDataListLength;  /* AttributeDataList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Params.Length; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeDataListLength), sizeof(req->Params.AttributeDataListLength)); idx += sizeof(req->Params.AttributeDataListLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeDataList, req->Params.AttributeDataListLength); idx += req->Params.AttributeDataListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x38, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendWriteRequestRequest(EATTClientSendWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendWriteRequestRequest(EATTClientSendWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x39, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendWriteResponseRequest(EATTServerSendWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendWriteResponseRequest(EATTServerSendWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x3A, (void*)req, sizeof(EATTServerSendWriteResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendWriteCommandRequest(EATTClientSendWriteCommandRequest_t *req, uint8_t fsciInterface)
\brief	EATT send write command (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendWriteCommandRequest(EATTClientSendWriteCommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x3B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendPrepareWriteRequestRequest(EATTClientSendPrepareWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send prepare write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendPrepareWriteRequestRequest(EATTClientSendPrepareWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x3D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendPrepareWriteResponseRequest(EATTServerSendPrepareWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send prepare write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendPrepareWriteResponseRequest(EATTServerSendPrepareWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x3E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendExecuteWriteRequestRequest(EATTClientSendExecuteWriteRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send execute write request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendExecuteWriteRequestRequest(EATTClientSendExecuteWriteRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x3F, (void*)req, sizeof(EATTClientSendExecuteWriteRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendExecuteWriteResponseRequest(EATTServerSendExecuteWriteResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send execute write response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendExecuteWriteResponseRequest(EATTServerSendExecuteWriteResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x40, (void*)req, sizeof(EATTServerSendExecuteWriteResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendHandleValueNotificationRequest(EATTServerSendHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	EATT send handle value notification (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendHandleValueNotificationRequest(EATTServerSendHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x41, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendHandleValueIndicationRequest(EATTServerSendHandleValueIndicationRequest_t *req, uint8_t fsciInterface)
\brief	EATT send handle value indication (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendHandleValueIndicationRequest(EATTServerSendHandleValueIndicationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x42, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendHandleValueConfirmationRequest(EATTClientSendHandleValueConfirmationRequest_t *req, uint8_t fsciInterface)
\brief	EATT send handle value confirmation (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendHandleValueConfirmationRequest(EATTClientSendHandleValueConfirmationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x43, (void*)req, sizeof(EATTClientSendHandleValueConfirmationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendMultipleHandleValueNotificationRequest(EATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	EATT send multiple handle value notification (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendMultipleHandleValueNotificationRequest(EATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x44, 0x44, (void*)req, sizeof(EATTServerSendMultipleHandleValueNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTClientSendReadMultipleVariableRequestRequest(EATTClientSendReadMultipleVariableRequestRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read multiple variable request (on client)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTClientSendReadMultipleVariableRequestRequest(EATTClientSendReadMultipleVariableRequestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* HandleCount */
	msgLen += req->Params.HandleCount * sizeof(uint16_t);  /* ListOfHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.HandleCount), sizeof(req->Params.HandleCount)); idx += sizeof(req->Params.HandleCount);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfHandles, req->Params.HandleCount * sizeof(uint16_t)); idx += req->Params.HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x45, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t EATTServerSendReadMultipleVariableResponseRequest(EATTServerSendReadMultipleVariableResponseRequest_t *req, uint8_t fsciInterface)
\brief	EATT send read multiple variable response (on server)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t EATTServerSendReadMultipleVariableResponseRequest(EATTServerSendReadMultipleVariableResponseRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* ListLength */
	msgLen += req->Params.ListLength;  /* lengthValueTupleList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.ListLength), sizeof(req->Params.ListLength)); idx += sizeof(req->Params.ListLength);
	FLib_MemCpy(pMsg + idx, req->Params.lengthValueTupleList, req->Params.ListLength); idx += req->Params.ListLength;

	/* Send the request */
	FSCI_transmitPayload(0x44, 0x46, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* ATT_ENABLE */

#if GATT_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GATTModeSelectRequest(GATTModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable GATT

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTModeSelectRequest(GATTModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x00, (void*)req, sizeof(GATTModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTInitRequest(uint8_t fsciInterface)
\brief	Initializes the GATT module

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTGetMtuRequest(GATTGetMtuRequest_t *req, uint8_t fsciInterface)
\brief	Retrieves the MTU used with a given connected device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTGetMtuRequest(GATTGetMtuRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x02, (void*)req, sizeof(GATTGetMtuRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientInitRequest(uint8_t fsciInterface)
\brief	Initializes the GATT Client functionality

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x03, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientResetProcedureRequest(uint8_t fsciInterface)
\brief	Resets any ongoing GATT Client procedure

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientResetProcedureRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x04, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterProcedureCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for the GATT Client module Procedures

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterProcedureCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x05, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterNotificationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Server Notifications

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterNotificationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x06, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterMultipleValueNotificationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Server Multiple Handle Value Notifications

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterMultipleValueNotificationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x23, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterIndicationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Server Indications

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterIndicationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x07, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientExchangeMtuRequest(GATTClientExchangeMtuRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the MTU Exchange procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientExchangeMtuRequest(GATTClientExchangeMtuRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x08, (void*)req, sizeof(GATTClientExchangeMtuRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientDiscoverAllPrimaryServicesRequest(GATTClientDiscoverAllPrimaryServicesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Primary Service Discovery procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientDiscoverAllPrimaryServicesRequest(GATTClientDiscoverAllPrimaryServicesRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x09, (void*)req, sizeof(GATTClientDiscoverAllPrimaryServicesRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientDiscoverPrimaryServicesByUuidRequest(GATTClientDiscoverPrimaryServicesByUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Primary Service Discovery By UUID procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientDiscoverPrimaryServicesByUuidRequest(GATTClientDiscoverPrimaryServicesByUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* MaxNbOfServices */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->MaxNbOfServices; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientFindIncludedServicesRequest(GATTClientFindIncludedServicesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Find Included Services procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientFindIncludedServicesRequest(GATTClientFindIncludedServicesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfIncludedServices */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfIncludedServices; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientDiscoverAllCharacteristicsOfServiceRequest(GATTClientDiscoverAllCharacteristicsOfServiceRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Discovery procedure for a given Service

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientDiscoverAllCharacteristicsOfServiceRequest(GATTClientDiscoverAllCharacteristicsOfServiceRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfCharacteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfCharacteristics; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientDiscoverCharacteristicOfServiceByUuidRequest(GATTClientDiscoverCharacteristicOfServiceByUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Discovery procedure for a given Service, with a given UUID

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientDiscoverCharacteristicOfServiceByUuidRequest(GATTClientDiscoverCharacteristicOfServiceByUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfCharacteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfCharacteristics; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientDiscoverAllCharacteristicDescriptorsRequest(GATTClientDiscoverAllCharacteristicDescriptorsRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Discovery procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientDiscoverAllCharacteristicDescriptorsRequest(GATTClientDiscoverAllCharacteristicDescriptorsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* MaxNbOfDescriptors */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->EndingHandle), sizeof(req->EndingHandle)); idx += sizeof(req->EndingHandle);
	pMsg[idx] = req->MaxNbOfDescriptors; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientReadCharacteristicValueRequest(GATTClientReadCharacteristicValueRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientReadCharacteristicValueRequest(GATTClientReadCharacteristicValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x0F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientReadUsingCharacteristicUuidRequest(GATTClientReadUsingCharacteristicUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read By UUID procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientReadUsingCharacteristicUuidRequest(GATTClientReadUsingCharacteristicUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(bool_t);  /* HandleRangeIncluded */

	if (req->HandleRangeIncluded)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
	}	/* HandleRange */

	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->HandleRangeIncluded; idx++;

	if (req->HandleRangeIncluded)
	{
		FLib_MemCpy(pMsg + idx, &(req->HandleRange.StartHandle), sizeof(req->HandleRange.StartHandle)); idx += sizeof(req->HandleRange.StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->HandleRange.EndHandle), sizeof(req->HandleRange.EndHandle)); idx += sizeof(req->HandleRange.EndHandle);
	}
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x10, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientReadMultipleCharacteristicValuesRequest(GATTClientReadMultipleCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read Multiple procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientReadMultipleCharacteristicValuesRequest(GATTClientReadMultipleCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.Handle), sizeof(req->Characteristics[i].Value.Handle)); idx += sizeof(req->Characteristics[i].Value.Handle);
		pMsg[idx] = req->Characteristics[i].Value.UuidType; idx++;

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.ValueLength), sizeof(req->Characteristics[i].Value.ValueLength)); idx += sizeof(req->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.MaxValueLength), sizeof(req->Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].Handle), sizeof(req->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x11, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientWriteCharacteristicValueRequest(GATTClientWriteCharacteristicValueRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Write procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientWriteCharacteristicValueRequest(GATTClientWriteCharacteristicValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */
	msgLen += sizeof(bool_t);  /* WithoutResponse */
	msgLen += sizeof(bool_t);  /* SignedWrite */
	msgLen += sizeof(bool_t);  /* ReliableLongCharWrites */
	msgLen += 16;  /* Csrk */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;
	pMsg[idx] = req->WithoutResponse; idx++;
	pMsg[idx] = req->SignedWrite; idx++;
	pMsg[idx] = req->ReliableLongCharWrites; idx++;
	FLib_MemCpy(pMsg + idx, req->Csrk, 16); idx += 16;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x12, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientReadCharacteristicDescriptorRequest(GATTClientReadCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Read procedure for a given Characteristic Descriptor

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientReadCharacteristicDescriptorRequest(GATTClientReadCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.Handle), sizeof(req->Descriptor.Handle)); idx += sizeof(req->Descriptor.Handle);
	pMsg[idx] = req->Descriptor.UuidType; idx++;

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.ValueLength), sizeof(req->Descriptor.ValueLength)); idx += sizeof(req->Descriptor.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.MaxValueLength), sizeof(req->Descriptor.MaxValueLength)); idx += sizeof(req->Descriptor.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Descriptor.Value, 0); idx += 0;
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x13, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientWriteCharacteristicDescriptorRequest(GATTClientWriteCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Write procedure for a given Characteristic Descriptor

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientWriteCharacteristicDescriptorRequest(GATTClientWriteCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.Handle), sizeof(req->Descriptor.Handle)); idx += sizeof(req->Descriptor.Handle);
	pMsg[idx] = req->Descriptor.UuidType; idx++;

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.ValueLength), sizeof(req->Descriptor.ValueLength)); idx += sizeof(req->Descriptor.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.MaxValueLength), sizeof(req->Descriptor.MaxValueLength)); idx += sizeof(req->Descriptor.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Descriptor.Value, 0); idx += 0;
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x14, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerInitRequest(uint8_t fsciInterface)
\brief	Initializes the GATT Server module

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTServerInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x15, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerRegisterCallbackRequest(uint8_t fsciInterface)
\brief	Installs an application callback for the GATT Server module

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTServerRegisterCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x16, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerRegisterHandlesForWriteNotificationsRequest(GATTServerRegisterHandlesForWriteNotificationsRequest_t *req, uint8_t fsciInterface)
\brief	Registers the attribute handles that will be notified through the GATT Server callback when a GATT Client attempts to modify the attributes' values

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerRegisterHandlesForWriteNotificationsRequest(GATTServerRegisterHandlesForWriteNotificationsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* HandleCount */
	msgLen += req->HandleCount * sizeof(uint16_t);  /* AttributeHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->HandleCount; idx++;
	FLib_MemCpy(pMsg + idx, req->AttributeHandles, req->HandleCount * sizeof(uint16_t)); idx += req->HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x17, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendAttributeWrittenStatusRequest(GATTServerSendAttributeWrittenStatusRequest_t *req, uint8_t fsciInterface)
\brief	Responds to a Control Point write operation

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendAttributeWrittenStatusRequest(GATTServerSendAttributeWrittenStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x18, (void*)req, sizeof(GATTServerSendAttributeWrittenStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendNotificationRequest(GATTServerSendNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendNotificationRequest(GATTServerSendNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x19, (void*)req, sizeof(GATTServerSendNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendIndicationRequest(GATTServerSendIndicationRequest_t *req, uint8_t fsciInterface)
\brief	Sends an indication to a peer GATT Client

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendIndicationRequest(GATTServerSendIndicationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x1A, (void*)req, sizeof(GATTServerSendIndicationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendInstantValueNotificationRequest(GATTServerSendInstantValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client with data given as parameter, ignoring the GATT Database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendInstantValueNotificationRequest(GATTServerSendInstantValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Handle), sizeof(req->Handle)); idx += sizeof(req->Handle);
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x1B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendInstantValueIndicationRequest(GATTServerSendInstantValueIndicationRequest_t *req, uint8_t fsciInterface)
\brief	Sends an indication to a peer GATT Client with data given as parameter, ignoring the GATT Databas

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendInstantValueIndicationRequest(GATTServerSendInstantValueIndicationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Handle), sizeof(req->Handle)); idx += sizeof(req->Handle);
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x1C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendMultipleHandleValueNotificationRequest(GATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client with data given as parameter, ignoring the GATT Database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendMultipleHandleValueNotificationRequest(GATTServerSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* HandleCount */

	for (uint32_t i = 0; i < req->HandleCount; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += req->HandleLengthValueList[i].ValueLength;  /* Value */
	}	/* HandleLengthValueList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->HandleCount; idx++;

	for (uint32_t i = 0; i < req->HandleCount; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->HandleLengthValueList[i].Handle), sizeof(req->HandleLengthValueList[i].Handle)); idx += sizeof(req->HandleLengthValueList[i].Handle);
		FLib_MemCpy(pMsg + idx, &(req->HandleLengthValueList[i].ValueLength), sizeof(req->HandleLengthValueList[i].ValueLength)); idx += sizeof(req->HandleLengthValueList[i].ValueLength);
		FLib_MemCpy(pMsg + idx, req->HandleLengthValueList[i].Value, req->HandleLengthValueList[i].ValueLength); idx += req->HandleLengthValueList[i].ValueLength;
	}

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x22, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerRegisterHandlesForReadNotificationsRequest(GATTServerRegisterHandlesForReadNotificationsRequest_t *req, uint8_t fsciInterface)
\brief	Registers the attribute handles that will be notified through the GATT Server callback when a GATT Client attempts to read the attributes' values

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerRegisterHandlesForReadNotificationsRequest(GATTServerRegisterHandlesForReadNotificationsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* HandleCount */
	msgLen += req->HandleCount * sizeof(uint16_t);  /* AttributeHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->HandleCount; idx++;
	FLib_MemCpy(pMsg + idx, req->AttributeHandles, req->HandleCount * sizeof(uint16_t)); idx += req->HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x1D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerSendAttributeReadStatusRequest(GATTServerSendAttributeReadStatusRequest_t *req, uint8_t fsciInterface)
\brief	Responds to an intercepted attribute read operation

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerSendAttributeReadStatusRequest(GATTServerSendAttributeReadStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x1E, (void*)req, sizeof(GATTServerSendAttributeReadStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerRegisterUniqueHandlesForNotificationsRequest(GATTServerRegisterUniqueHandlesForNotificationsRequest_t *req, uint8_t fsciInterface)
\brief	Registers all attribute handles with unique value buffers to be notified through the GATT Server callback when a GATT Client attempts to read/write the attributes' values.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerRegisterUniqueHandlesForNotificationsRequest(GATTServerRegisterUniqueHandlesForNotificationsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x1F, (void*)req, sizeof(GATTServerRegisterUniqueHandlesForNotificationsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerUnregisterHandlesForWriteNotificationsRequest(GATTServerUnregisterHandlesForWriteNotificationsRequest_t *req, uint8_t fsciInterface)
\brief	Unregisters the attribute handles that will be notified through the GATT Server callback when a GATT Client attempts to write the attributes' values

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerUnregisterHandlesForWriteNotificationsRequest(GATTServerUnregisterHandlesForWriteNotificationsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* HandleCount */
	msgLen += req->HandleCount * sizeof(uint16_t);  /* AttributeHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->HandleCount; idx++;
	FLib_MemCpy(pMsg + idx, req->AttributeHandles, req->HandleCount * sizeof(uint16_t)); idx += req->HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x20, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerUnregisterHandlesForReadNotificationsRequest(GATTServerUnregisterHandlesForReadNotificationsRequest_t *req, uint8_t fsciInterface)
\brief	Unregisters the attribute handles that will be notified through the GATT Server callback when a GATT Client attempts to read the attributes' values

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerUnregisterHandlesForReadNotificationsRequest(GATTServerUnregisterHandlesForReadNotificationsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* HandleCount */
	msgLen += req->HandleCount * sizeof(uint16_t);  /* AttributeHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->HandleCount; idx++;
	FLib_MemCpy(pMsg + idx, req->AttributeHandles, req->HandleCount * sizeof(uint16_t)); idx += req->HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x21, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientReadMultipleVariableCharacteristicValuesRequest(GATTClientReadMultipleVariableCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
\brief	Unregisters the attribute handles that will be notified through the GATT Server callback when a GATT Client attempts to read the attributes' values

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientReadMultipleVariableCharacteristicValuesRequest(GATTClientReadMultipleVariableCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.Handle), sizeof(req->Characteristics[i].Value.Handle)); idx += sizeof(req->Characteristics[i].Value.Handle);
		pMsg[idx] = req->Characteristics[i].Value.UuidType; idx++;

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.ValueLength), sizeof(req->Characteristics[i].Value.ValueLength)); idx += sizeof(req->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.MaxValueLength), sizeof(req->Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].Handle), sizeof(req->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x24, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterEnhancedProcedureCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for the GATT Client module Procedures on Enhanced ATT bearers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterEnhancedProcedureCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x25, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterEnhancedNotificationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Server Notifications on Enhanced ATT bearers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterEnhancedNotificationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x26, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterEnhancedIndicationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Server Indications on Enhanced ATT bearers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterEnhancedIndicationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x27, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientRegisterEnhancedMultipleValueNotificationCallbackRequest(uint8_t fsciInterface)
\brief	Installs the application callback for Multiple Value Notifications on Enhanced ATT bearers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTClientRegisterEnhancedMultipleValueNotificationCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x28, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedDiscoverAllPrimaryServicesRequest(GATTClientEnhancedDiscoverAllPrimaryServicesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Primary Service Discovery procedure on an enhanced ATT bearer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedDiscoverAllPrimaryServicesRequest(GATTClientEnhancedDiscoverAllPrimaryServicesRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x29, (void*)req, sizeof(GATTClientEnhancedDiscoverAllPrimaryServicesRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedDiscoverPrimaryServicesByUuidRequest(GATTClientEnhancedDiscoverPrimaryServicesByUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Primary Service Discovery By UUID procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedDiscoverPrimaryServicesByUuidRequest(GATTClientEnhancedDiscoverPrimaryServicesByUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* MaxNbOfServices */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->MaxNbOfServices; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedFindIncludedServicesRequest(GATTClientEnhancedFindIncludedServicesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Find Included Services procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedFindIncludedServicesRequest(GATTClientEnhancedFindIncludedServicesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfIncludedServices */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfIncludedServices; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedDiscoverAllCharacteristicsOfServiceRequest(GATTClientEnhancedDiscoverAllCharacteristicsOfServiceRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Discovery procedure for a given Service

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedDiscoverAllCharacteristicsOfServiceRequest(GATTClientEnhancedDiscoverAllCharacteristicsOfServiceRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfCharacteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfCharacteristics; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedDiscoverCharacteristicOfServiceByUuidRequest(GATTClientEnhancedDiscoverCharacteristicOfServiceByUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Discovery procedure for a given Service, with a given UUID

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedDiscoverCharacteristicOfServiceByUuidRequest(GATTClientEnhancedDiscoverCharacteristicOfServiceByUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint16_t);  /* EndHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */
	msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */
		msgLen += sizeof(uint8_t);  /* NbOfIncludedServices */
	}	/* IncludedServices */
	msgLen += sizeof(uint8_t);  /* MaxNbOfCharacteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Service.StartHandle), sizeof(req->Service.StartHandle)); idx += sizeof(req->Service.StartHandle);
	FLib_MemCpy(pMsg + idx, &(req->Service.EndHandle), sizeof(req->Service.EndHandle)); idx += sizeof(req->Service.EndHandle);
	pMsg[idx] = req->Service.UuidType; idx++;

	switch (req->Service.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Service.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->Service.NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Service.Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.Handle), sizeof(req->Service.Characteristics[i].Value.Handle)); idx += sizeof(req->Service.Characteristics[i].Value.Handle);
		pMsg[idx] = req->Service.Characteristics[i].Value.UuidType; idx++;

		switch (req->Service.Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.ValueLength), sizeof(req->Service.Characteristics[i].Value.ValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Value.MaxValueLength), sizeof(req->Service.Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Service.Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Service.Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].Handle), sizeof(req->Service.Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Service.Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Service.Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Service.Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Service.Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Service.Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}
	pMsg[idx] = req->Service.NbOfIncludedServices; idx++;

	for (uint32_t i = 0; i < req->Service.NbOfIncludedServices; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].StartHandle), sizeof(req->Service.IncludedServices[i].StartHandle)); idx += sizeof(req->Service.IncludedServices[i].StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->Service.IncludedServices[i].EndHandle), sizeof(req->Service.IncludedServices[i].EndHandle)); idx += sizeof(req->Service.IncludedServices[i].EndHandle);
		pMsg[idx] = req->Service.IncludedServices[i].UuidType; idx++;

		switch (req->Service.IncludedServices[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Service.IncludedServices[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		pMsg[idx] = req->Service.IncludedServices[i].NbOfCharacteristics; idx++;
		pMsg[idx] = req->Service.IncludedServices[i].NbOfIncludedServices; idx++;
	}
	pMsg[idx] = req->MaxNbOfCharacteristics; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedDiscoverAllCharacteristicDescriptorsRequest(GATTClientEnhancedDiscoverAllCharacteristicDescriptorsRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Discovery procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedDiscoverAllCharacteristicDescriptorsRequest(GATTClientEnhancedDiscoverAllCharacteristicDescriptorsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* MaxNbOfDescriptors */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->EndingHandle), sizeof(req->EndingHandle)); idx += sizeof(req->EndingHandle);
	pMsg[idx] = req->MaxNbOfDescriptors; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedReadCharacteristicValueRequest(GATTClientEnhancedReadCharacteristicValueRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedReadCharacteristicValueRequest(GATTClientEnhancedReadCharacteristicValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x2F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedReadUsingCharacteristicUuidRequest(GATTClientEnhancedReadUsingCharacteristicUuidRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read By UUID procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedReadUsingCharacteristicUuidRequest(GATTClientEnhancedReadUsingCharacteristicUuidRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(bool_t);  /* HandleRangeIncluded */

	if (req->HandleRangeIncluded)
	{
		msgLen += sizeof(uint16_t);  /* StartHandle */
		msgLen += sizeof(uint16_t);  /* EndHandle */
	}	/* HandleRange */

	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->HandleRangeIncluded; idx++;

	if (req->HandleRangeIncluded)
	{
		FLib_MemCpy(pMsg + idx, &(req->HandleRange.StartHandle), sizeof(req->HandleRange.StartHandle)); idx += sizeof(req->HandleRange.StartHandle);
		FLib_MemCpy(pMsg + idx, &(req->HandleRange.EndHandle), sizeof(req->HandleRange.EndHandle)); idx += sizeof(req->HandleRange.EndHandle);
	}
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x30, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedReadMultipleCharacteristicValuesRequest(GATTClientEnhancedReadMultipleCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Read Multiple procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedReadMultipleCharacteristicValuesRequest(GATTClientEnhancedReadMultipleCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.Handle), sizeof(req->Characteristics[i].Value.Handle)); idx += sizeof(req->Characteristics[i].Value.Handle);
		pMsg[idx] = req->Characteristics[i].Value.UuidType; idx++;

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.ValueLength), sizeof(req->Characteristics[i].Value.ValueLength)); idx += sizeof(req->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.MaxValueLength), sizeof(req->Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].Handle), sizeof(req->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x31, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedWriteCharacteristicValueRequest(GATTClientEnhancedWriteCharacteristicValueRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Write procedure for a given Characteristic

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedWriteCharacteristicValueRequest(GATTClientEnhancedWriteCharacteristicValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* Properties */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
	}	/* Descriptors */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */
	msgLen += sizeof(bool_t);  /* WithoutResponse */
	msgLen += sizeof(bool_t);  /* SignedWrite */
	msgLen += sizeof(bool_t);  /* ReliableLongCharWrites */
	msgLen += 16;  /* Csrk */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->Characteristic.Properties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.Handle), sizeof(req->Characteristic.Value.Handle)); idx += sizeof(req->Characteristic.Value.Handle);
	pMsg[idx] = req->Characteristic.Value.UuidType; idx++;

	switch (req->Characteristic.Value.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.ValueLength), sizeof(req->Characteristic.Value.ValueLength)); idx += sizeof(req->Characteristic.Value.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Characteristic.Value.MaxValueLength), sizeof(req->Characteristic.Value.MaxValueLength)); idx += sizeof(req->Characteristic.Value.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Characteristic.Value.Value, 0); idx += 0;
	pMsg[idx] = req->Characteristic.NbOfDescriptors; idx++;

	for (uint32_t i = 0; i < req->Characteristic.NbOfDescriptors; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].Handle), sizeof(req->Characteristic.Descriptors[i].Handle)); idx += sizeof(req->Characteristic.Descriptors[i].Handle);
		pMsg[idx] = req->Characteristic.Descriptors[i].UuidType; idx++;

		switch (req->Characteristic.Descriptors[i].UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].ValueLength), sizeof(req->Characteristic.Descriptors[i].ValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristic.Descriptors[i].MaxValueLength), sizeof(req->Characteristic.Descriptors[i].MaxValueLength)); idx += sizeof(req->Characteristic.Descriptors[i].MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristic.Descriptors[i].Value, 0); idx += 0;
	}
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;
	pMsg[idx] = req->WithoutResponse; idx++;
	pMsg[idx] = req->SignedWrite; idx++;
	pMsg[idx] = req->ReliableLongCharWrites; idx++;
	FLib_MemCpy(pMsg + idx, req->Csrk, 16); idx += 16;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x32, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedReadCharacteristicDescriptorRequest(GATTClientEnhancedReadCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Read procedure for a given Characteristic Descriptor

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedReadCharacteristicDescriptorRequest(GATTClientEnhancedReadCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint16_t);  /* MaxReadBytes */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.Handle), sizeof(req->Descriptor.Handle)); idx += sizeof(req->Descriptor.Handle);
	pMsg[idx] = req->Descriptor.UuidType; idx++;

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.ValueLength), sizeof(req->Descriptor.ValueLength)); idx += sizeof(req->Descriptor.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.MaxValueLength), sizeof(req->Descriptor.MaxValueLength)); idx += sizeof(req->Descriptor.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Descriptor.Value, 0); idx += 0;
	FLib_MemCpy(pMsg + idx, &(req->MaxReadBytes), sizeof(req->MaxReadBytes)); idx += sizeof(req->MaxReadBytes);

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x33, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedWriteCharacteristicDescriptorRequest(GATTClientEnhancedWriteCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Characteristic Descriptor Write procedure for a given Characteristic Descriptor

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedWriteCharacteristicDescriptorRequest(GATTClientEnhancedWriteCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += 0;  /* Value */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.Handle), sizeof(req->Descriptor.Handle)); idx += sizeof(req->Descriptor.Handle);
	pMsg[idx] = req->Descriptor.UuidType; idx++;

	switch (req->Descriptor.UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Descriptor.Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.ValueLength), sizeof(req->Descriptor.ValueLength)); idx += sizeof(req->Descriptor.ValueLength);
	FLib_MemCpy(pMsg + idx, &(req->Descriptor.MaxValueLength), sizeof(req->Descriptor.MaxValueLength)); idx += sizeof(req->Descriptor.MaxValueLength);
	FLib_MemCpy(pMsg + idx, req->Descriptor.Value, 0); idx += 0;
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x34, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientEnhancedReadMultipleVariableCharacteristicValuesRequest(GATTClientEnhancedReadMultipleVariableCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
\brief	Initializes the Read Multiple Variable Length Characteristic Values procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientEnhancedReadMultipleVariableCharacteristicValuesRequest(GATTClientEnhancedReadMultipleVariableCharacteristicValuesRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint8_t);  /* NbOfCharacteristics */

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		msgLen += sizeof(uint8_t);  /* Properties */
		msgLen += sizeof(uint16_t);  /* Handle */
		msgLen += sizeof(uint8_t);  /* UuidType */

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				msgLen += 2;  /* Uuid16Bits */
				break;

			case Uuid128Bits:
				msgLen += 16;  /* Uuid128Bits */
				break;

			case Uuid32Bits:
				msgLen += 4;  /* Uuid32Bits */
				break;
		}		/* Uuid */
		msgLen += sizeof(uint16_t);  /* ValueLength */
		msgLen += sizeof(uint16_t);  /* MaxValueLength */
		msgLen += 0;  /* Value */
		msgLen += sizeof(uint8_t);  /* NbOfDescriptors */

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			msgLen += sizeof(uint16_t);  /* Handle */
			msgLen += sizeof(uint8_t);  /* UuidType */

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					msgLen += 2;  /* Uuid16Bits */
					break;

				case Uuid128Bits:
					msgLen += 16;  /* Uuid128Bits */
					break;

				case Uuid32Bits:
					msgLen += 4;  /* Uuid32Bits */
					break;
			}			/* Uuid */
			msgLen += sizeof(uint16_t);  /* ValueLength */
			msgLen += sizeof(uint16_t);  /* MaxValueLength */
			msgLen += 0;  /* Value */
		}		/* Descriptors */
	}	/* Characteristics */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	pMsg[idx] = req->NbOfCharacteristics; idx++;

	for (uint32_t i = 0; i < req->NbOfCharacteristics; i++)
	{
		pMsg[idx] = req->Characteristics[i].Properties; idx++;
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.Handle), sizeof(req->Characteristics[i].Value.Handle)); idx += sizeof(req->Characteristics[i].Value.Handle);
		pMsg[idx] = req->Characteristics[i].Value.UuidType; idx++;

		switch (req->Characteristics[i].Value.UuidType)
		{
			case Uuid16Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid16Bits, 2); idx += 2;
				break;

			case Uuid128Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid128Bits, 16); idx += 16;
				break;

			case Uuid32Bits:
				FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Uuid.Uuid32Bits, 4); idx += 4;
				break;
		}
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.ValueLength), sizeof(req->Characteristics[i].Value.ValueLength)); idx += sizeof(req->Characteristics[i].Value.ValueLength);
		FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Value.MaxValueLength), sizeof(req->Characteristics[i].Value.MaxValueLength)); idx += sizeof(req->Characteristics[i].Value.MaxValueLength);
		FLib_MemCpy(pMsg + idx, req->Characteristics[i].Value.Value, 0); idx += 0;
		pMsg[idx] = req->Characteristics[i].NbOfDescriptors; idx++;

		for (uint32_t j = 0; j < req->Characteristics[i].NbOfDescriptors; j++)
		{
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].Handle), sizeof(req->Characteristics[i].Descriptors[j].Handle)); idx += sizeof(req->Characteristics[i].Descriptors[j].Handle);
			pMsg[idx] = req->Characteristics[i].Descriptors[j].UuidType; idx++;

			switch (req->Characteristics[i].Descriptors[j].UuidType)
			{
				case Uuid16Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid16Bits, 2); idx += 2;
					break;

				case Uuid128Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid128Bits, 16); idx += 16;
					break;

				case Uuid32Bits:
					FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Uuid.Uuid32Bits, 4); idx += 4;
					break;
			}
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].ValueLength), sizeof(req->Characteristics[i].Descriptors[j].ValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].ValueLength);
			FLib_MemCpy(pMsg + idx, &(req->Characteristics[i].Descriptors[j].MaxValueLength), sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength)); idx += sizeof(req->Characteristics[i].Descriptors[j].MaxValueLength);
			FLib_MemCpy(pMsg + idx, req->Characteristics[i].Descriptors[j].Value, 0); idx += 0;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x35, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerRegisterEnhancedCallbackRequest(uint8_t fsciInterface)
\brief	Installs an application callback for the GATT Server module

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTServerRegisterEnhancedCallbackRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x45, 0x36, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendAttributeWrittenStatusRequest(GATTServerEnhancedSendAttributeWrittenStatusRequest_t *req, uint8_t fsciInterface)
\brief	Responds to a Control Point write operation

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendAttributeWrittenStatusRequest(GATTServerEnhancedSendAttributeWrittenStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x37, (void*)req, sizeof(GATTServerEnhancedSendAttributeWrittenStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendNotificationRequest(GATTServerEnhancedSendNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendNotificationRequest(GATTServerEnhancedSendNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x38, (void*)req, sizeof(GATTServerEnhancedSendNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendIndicationRequest(GATTServerEnhancedSendIndicationRequest_t *req, uint8_t fsciInterface)
\brief	Sends an indication to a peer GATT Client

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendIndicationRequest(GATTServerEnhancedSendIndicationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x39, (void*)req, sizeof(GATTServerEnhancedSendIndicationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendInstantValueNotificationRequest(GATTServerEnhancedSendInstantValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client with data given as parameter, ignoring the GATT Database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendInstantValueNotificationRequest(GATTServerEnhancedSendInstantValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Handle), sizeof(req->Handle)); idx += sizeof(req->Handle);
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x3A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendInstantValueIndicationRequest(GATTServerEnhancedSendInstantValueIndicationRequest_t *req, uint8_t fsciInterface)
\brief	Sends an indication to a peer GATT Client with data given as parameter, ignoring the GATT Databas

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendInstantValueIndicationRequest(GATTServerEnhancedSendInstantValueIndicationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Handle), sizeof(req->Handle)); idx += sizeof(req->Handle);
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x3B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendAttributeReadStatusRequest(GATTServerEnhancedSendAttributeReadStatusRequest_t *req, uint8_t fsciInterface)
\brief	Responds to an intercepted attribute read operation

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendAttributeReadStatusRequest(GATTServerEnhancedSendAttributeReadStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x3C, (void*)req, sizeof(GATTServerEnhancedSendAttributeReadStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTServerEnhancedSendMultipleHandleValueNotificationRequest(GATTServerEnhancedSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Sends a notification to a peer GATT Client with data given as parameter,
                ignoring the GATT Database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTServerEnhancedSendMultipleHandleValueNotificationRequest(GATTServerEnhancedSendMultipleHandleValueNotificationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* BearerId */
	msgLen += sizeof(uint32_t);  /* TotalLength */
	msgLen += req->TotalLength;  /* HandleLengthValueList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->BearerId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->TotalLength), sizeof(req->TotalLength)); idx += sizeof(req->TotalLength);
	FLib_MemCpy(pMsg + idx, req->HandleLengthValueList, req->TotalLength); idx += req->TotalLength;

	/* Send the request */
	FSCI_transmitPayload(0x45, 0x3D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTClientGetDatabaseHashRequest(GATTClientGetDatabaseHashRequest_t *req, uint8_t fsciInterface)
\brief	Updates the gatt database hash value for the peer server

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTClientGetDatabaseHashRequest(GATTClientGetDatabaseHashRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x45, 0x3E, (void*)req, sizeof(GATTClientGetDatabaseHashRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* GATT_ENABLE */

#if GATTDB_APP_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GATTDBModeSelectRequest(GATTDBModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable GATT Database (application)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBModeSelectRequest(GATTDBModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x46, 0x00, (void*)req, sizeof(GATTDBModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBWriteAttributeRequest(GATTDBWriteAttributeRequest_t *req, uint8_t fsciInterface)
\brief	Writes an attribute from the application level

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBWriteAttributeRequest(GATTDBWriteAttributeRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* Handle */
	msgLen += sizeof(uint16_t);  /* ValueLength */
	msgLen += req->ValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->Handle), sizeof(req->Handle)); idx += sizeof(req->Handle);
	FLib_MemCpy(pMsg + idx, &(req->ValueLength), sizeof(req->ValueLength)); idx += sizeof(req->ValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->ValueLength); idx += req->ValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x02, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBReadAttributeRequest(GATTDBReadAttributeRequest_t *req, uint8_t fsciInterface)
\brief	Reads an attribute from application level

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBReadAttributeRequest(GATTDBReadAttributeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x46, 0x03, (void*)req, sizeof(GATTDBReadAttributeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBFindServiceHandleRequest(GATTDBFindServiceHandleRequest_t *req, uint8_t fsciInterface)
\brief	Finds the handle of a Service Declaration with a given UUID inside the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBFindServiceHandleRequest(GATTDBFindServiceHandleRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* StartHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->StartHandle), sizeof(req->StartHandle)); idx += sizeof(req->StartHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x04, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBFindCharValueHandleInServiceRequest(GATTDBFindCharValueHandleInServiceRequest_t *req, uint8_t fsciInterface)
\brief	Finds the handle of a Characteristic Value with a given UUID inside a service

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBFindCharValueHandleInServiceRequest(GATTDBFindCharValueHandleInServiceRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* ServiceHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->ServiceHandle), sizeof(req->ServiceHandle)); idx += sizeof(req->ServiceHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x05, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBFindCccdHandleForCharValueHandleRequest(GATTDBFindCccdHandleForCharValueHandleRequest_t *req, uint8_t fsciInterface)
\brief	Finds the handle of a Characteristic's CCCD given the Characteristic's Value handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBFindCccdHandleForCharValueHandleRequest(GATTDBFindCccdHandleForCharValueHandleRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x46, 0x06, (void*)req, sizeof(GATTDBFindCccdHandleForCharValueHandleRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBFindDescriptorHandleForCharValueHandleRequest(GATTDBFindDescriptorHandleForCharValueHandleRequest_t *req, uint8_t fsciInterface)
\brief	Finds the handle of a Characteristic Descriptor given the Characteristic's Value handle and Descriptor's UUID

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBFindDescriptorHandleForCharValueHandleRequest(GATTDBFindDescriptorHandleForCharValueHandleRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* CharValueHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->CharValueHandle), sizeof(req->CharValueHandle)); idx += sizeof(req->CharValueHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x07, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicInitRequest(uint8_t fsciInterface)
\brief	Allocates smallest possible buffers and initializes an empty database.

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTDBDynamicInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x46, 0x08, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicReleaseDatabaseRequest(uint8_t fsciInterface)
\brief	Releases the allocated buffers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTDBDynamicReleaseDatabaseRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x46, 0x09, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddPrimaryServiceDeclarationRequest(GATTDBDynamicAddPrimaryServiceDeclarationRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Primary Service declaration into the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddPrimaryServiceDeclarationRequest(GATTDBDynamicAddPrimaryServiceDeclarationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* DesiredHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->DesiredHandle), sizeof(req->DesiredHandle)); idx += sizeof(req->DesiredHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x0A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddSecondaryServiceDeclarationRequest(GATTDBDynamicAddSecondaryServiceDeclarationRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Secondary Service declaration into the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddSecondaryServiceDeclarationRequest(GATTDBDynamicAddSecondaryServiceDeclarationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* DesiredHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->DesiredHandle), sizeof(req->DesiredHandle)); idx += sizeof(req->DesiredHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x0B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddIncludeDeclarationRequest(GATTDBDynamicAddIncludeDeclarationRequest_t *req, uint8_t fsciInterface)
\brief	Adds an Include declaration into the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddIncludeDeclarationRequest(GATTDBDynamicAddIncludeDeclarationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* IncludedServiceHandle */
	msgLen += sizeof(uint16_t);  /* EndGroupHandle */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->IncludedServiceHandle), sizeof(req->IncludedServiceHandle)); idx += sizeof(req->IncludedServiceHandle);
	FLib_MemCpy(pMsg + idx, &(req->EndGroupHandle), sizeof(req->EndGroupHandle)); idx += sizeof(req->EndGroupHandle);
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x0C, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Characteristic declaration and its Value into the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCharacteristicDeclarationAndValueRequest(GATTDBDynamicAddCharacteristicDeclarationAndValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* CharacteristicProperties */
	msgLen += sizeof(uint16_t);  /* MaxValueLength */
	msgLen += sizeof(uint16_t);  /* InitialValueLength */
	msgLen += req->InitialValueLength;  /* InitialValue */
	msgLen += sizeof(uint8_t);  /* ValueAccessPermissions */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->CharacteristicProperties; idx++;
	FLib_MemCpy(pMsg + idx, &(req->MaxValueLength), sizeof(req->MaxValueLength)); idx += sizeof(req->MaxValueLength);
	FLib_MemCpy(pMsg + idx, &(req->InitialValueLength), sizeof(req->InitialValueLength)); idx += sizeof(req->InitialValueLength);
	FLib_MemCpy(pMsg + idx, req->InitialValue, req->InitialValueLength); idx += req->InitialValueLength;
	pMsg[idx] = req->ValueAccessPermissions; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x0D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCharacteristicDescriptorRequest(GATTDBDynamicAddCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Characteristic descriptor into the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCharacteristicDescriptorRequest(GATTDBDynamicAddCharacteristicDescriptorRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint16_t);  /* DescriptorValueLength */
	msgLen += req->DescriptorValueLength;  /* Value */
	msgLen += sizeof(uint8_t);  /* DescriptorAccessPermissions */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	FLib_MemCpy(pMsg + idx, &(req->DescriptorValueLength), sizeof(req->DescriptorValueLength)); idx += sizeof(req->DescriptorValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->DescriptorValueLength); idx += req->DescriptorValueLength;
	pMsg[idx] = req->DescriptorAccessPermissions; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x0E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCccdRequest(uint8_t fsciInterface)
\brief	Adds a CCCD in the database

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCccdRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x46, 0x0F, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueRequest(GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Characteristic declaration with a Value contained in an universal value buffer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueRequest(GATTDBDynamicAddCharacteristicDeclarationWithUniqueValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* CharacteristicProperties */
	msgLen += sizeof(uint8_t);  /* ValueAccessPermissions */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->CharacteristicProperties; idx++;
	pMsg[idx] = req->ValueAccessPermissions; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x10, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicRemoveServiceRequest(GATTDBDynamicRemoveServiceRequest_t *req, uint8_t fsciInterface)
\brief	Removes a Service from the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicRemoveServiceRequest(GATTDBDynamicRemoveServiceRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x46, 0x11, (void*)req, sizeof(GATTDBDynamicRemoveServiceRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicRemoveCharacteristicRequest(GATTDBDynamicRemoveCharacteristicRequest_t *req, uint8_t fsciInterface)
\brief	Removes a Characteristic from the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicRemoveCharacteristicRequest(GATTDBDynamicRemoveCharacteristicRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x46, 0x12, (void*)req, sizeof(GATTDBDynamicRemoveCharacteristicRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCharDescriptorWithUniqueValueRequest(GATTDBDynamicAddCharDescriptorWithUniqueValueRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Characteristic descriptor into the database with an unique 512-byte value buffer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCharDescriptorWithUniqueValueRequest(GATTDBDynamicAddCharDescriptorWithUniqueValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* UuidType */

	switch (req->UuidType)
	{
		case Uuid16Bits:
			msgLen += 2;  /* Uuid16Bits */
			break;

		case Uuid128Bits:
			msgLen += 16;  /* Uuid128Bits */
			break;

		case Uuid32Bits:
			msgLen += 4;  /* Uuid32Bits */
			break;
	}	/* Uuid */
	msgLen += sizeof(uint8_t);  /* DescriptorAccessPermissions */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->UuidType; idx++;

	switch (req->UuidType)
	{
		case Uuid16Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid16Bits, 2); idx += 2;
			break;

		case Uuid128Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid128Bits, 16); idx += 16;
			break;

		case Uuid32Bits:
			FLib_MemCpy(pMsg + idx, req->Uuid.Uuid32Bits, 4); idx += 4;
			break;
	}
	pMsg[idx] = req->DescriptorAccessPermissions; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x13, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicEndDatabaseUpdateRequest(uint8_t fsciInterface)
\brief	Informs the peers of the changes produced in the database

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GATTDBDynamicEndDatabaseUpdateRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x46, 0x14, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBDynamicAddCharAggregateFormatRequest(GATTDBDynamicAddCharAggregateFormatRequest_t *req, uint8_t fsciInterface)
\brief	Adds a Characteristic Aggregate Format Descriptor in the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBDynamicAddCharAggregateFormatRequest(GATTDBDynamicAddCharAggregateFormatRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* DescriptorValueLength */
	msgLen += req->DescriptorValueLength;  /* Value */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->DescriptorValueLength), sizeof(req->DescriptorValueLength)); idx += sizeof(req->DescriptorValueLength);
	FLib_MemCpy(pMsg + idx, req->Value, req->DescriptorValueLength); idx += req->DescriptorValueLength;

	/* Send the request */
	FSCI_transmitPayload(0x46, 0x15, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* GATTDB_APP_ENABLE */

#if GATTDB_ATT_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttModeSelectRequest(GATTDBAttModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable GATT Database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttModeSelectRequest(GATTDBAttModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x47, 0x00, (void*)req, sizeof(GATTDBAttModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttFindInformationRequest(GATTDBAttFindInformationRequest_t *req, uint8_t fsciInterface)
\brief	Find information request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttFindInformationRequest(GATTDBAttFindInformationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x47, 0x01, (void*)req, sizeof(GATTDBAttFindInformationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttFindByTypeValueRequest(GATTDBAttFindByTypeValueRequest_t *req, uint8_t fsciInterface)
\brief	Find by type value request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttFindByTypeValueRequest(GATTDBAttFindByTypeValueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint16_t);  /* AttributeType */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType), sizeof(req->Params.AttributeType)); idx += sizeof(req->Params.AttributeType);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x02, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttReadByTypeRequest(GATTDBAttReadByTypeRequest_t *req, uint8_t fsciInterface)
\brief	Read by type request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttReadByTypeRequest(GATTDBAttReadByTypeRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case GATTDBAttReadByTypeRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case GATTDBAttReadByTypeRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case GATTDBAttReadByTypeRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case GATTDBAttReadByTypeRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x03, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttReadRequest(GATTDBAttReadRequest_t *req, uint8_t fsciInterface)
\brief	Read request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttReadRequest(GATTDBAttReadRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x47, 0x04, (void*)req, sizeof(GATTDBAttReadRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttReadBlobRequest(GATTDBAttReadBlobRequest_t *req, uint8_t fsciInterface)
\brief	Read blob request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttReadBlobRequest(GATTDBAttReadBlobRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x47, 0x05, (void*)req, sizeof(GATTDBAttReadBlobRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttReadMultipleRequest(GATTDBAttReadMultipleRequest_t *req, uint8_t fsciInterface)
\brief	Read multiple request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttReadMultipleRequest(GATTDBAttReadMultipleRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* HandleCount */
	msgLen += req->Params.HandleCount * sizeof(uint16_t);  /* ListOfHandles */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.HandleCount), sizeof(req->Params.HandleCount)); idx += sizeof(req->Params.HandleCount);
	FLib_MemCpy(pMsg + idx, req->Params.ListOfHandles, req->Params.HandleCount * sizeof(uint16_t)); idx += req->Params.HandleCount * sizeof(uint16_t);

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x06, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttReadByGroupTypeRequest(GATTDBAttReadByGroupTypeRequest_t *req, uint8_t fsciInterface)
\brief	Read by group type

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttReadByGroupTypeRequest(GATTDBAttReadByGroupTypeRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* StartingHandle */
	msgLen += sizeof(uint16_t);  /* EndingHandle */
	msgLen += sizeof(uint8_t);  /* Format */

	switch (req->Params.Format)
	{
		case GATTDBAttReadByGroupTypeRequest_Params_Format_Uuid16BitFormat:
			msgLen += sizeof(uint16_t);  /* Uuid16BitFormat */
			break;

		case GATTDBAttReadByGroupTypeRequest_Params_Format_Uuid128BitFormat:
			msgLen += sizeof(req->Params.AttributeType.Uuid128BitFormat);  /* Uuid128BitFormat */
			break;
	}	/* AttributeType */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.StartingHandle), sizeof(req->Params.StartingHandle)); idx += sizeof(req->Params.StartingHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.EndingHandle), sizeof(req->Params.EndingHandle)); idx += sizeof(req->Params.EndingHandle);
	pMsg[idx] = req->Params.Format; idx++;

	switch (req->Params.Format)
	{
		case GATTDBAttReadByGroupTypeRequest_Params_Format_Uuid16BitFormat:
			FLib_MemCpy(pMsg + idx, &(req->Params.AttributeType.Uuid16BitFormat), sizeof(req->Params.AttributeType.Uuid16BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid16BitFormat);
			break;

		case GATTDBAttReadByGroupTypeRequest_Params_Format_Uuid128BitFormat:
			FLib_MemCpy(pMsg + idx, req->Params.AttributeType.Uuid128BitFormat, sizeof(req->Params.AttributeType.Uuid128BitFormat)); idx += sizeof(req->Params.AttributeType.Uuid128BitFormat);
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x07, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttWriteRequest(GATTDBAttWriteRequest_t *req, uint8_t fsciInterface)
\brief	Write request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttWriteRequest(GATTDBAttWriteRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x08, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttWriteCommandRequest(GATTDBAttWriteCommandRequest_t *req, uint8_t fsciInterface)
\brief	Write command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttWriteCommandRequest(GATTDBAttWriteCommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x09, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttSignedWriteCommandRequest(GATTDBAttSignedWriteCommandRequest_t *req, uint8_t fsciInterface)
\brief	Signed write command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttSignedWriteCommandRequest(GATTDBAttSignedWriteCommandRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */
	msgLen += 12;  /* AuthenticationSignature */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;
	FLib_MemCpy(pMsg + idx, req->Params.AuthenticationSignature, 12); idx += 12;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x0A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttPrepareWriteRequest(GATTDBAttPrepareWriteRequest_t *req, uint8_t fsciInterface)
\brief	Prepare write request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttPrepareWriteRequest(GATTDBAttPrepareWriteRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x0B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttExecuteWriteRequest(GATTDBAttExecuteWriteRequest_t *req, uint8_t fsciInterface)
\brief	Execute write request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttExecuteWriteRequest(GATTDBAttExecuteWriteRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x47, 0x0C, (void*)req, sizeof(GATTDBAttExecuteWriteRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttExecuteWriteFromQueueRequest(GATTDBAttExecuteWriteFromQueueRequest_t *req, uint8_t fsciInterface)
\brief	Executes an operation from a Prepare Write queue

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttExecuteWriteFromQueueRequest(GATTDBAttExecuteWriteFromQueueRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* ValueOffset */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.ValueOffset), sizeof(req->Params.ValueOffset)); idx += sizeof(req->Params.ValueOffset);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x0D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GATTDBAttPrepareNotificationIndicationRequest(GATTDBAttPrepareNotificationIndicationRequest_t *req, uint8_t fsciInterface)
\brief	Prepare notification/indication

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GATTDBAttPrepareNotificationIndicationRequest(GATTDBAttPrepareNotificationIndicationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* AttributeHandle */
	msgLen += sizeof(uint16_t);  /* AttributeLength */
	msgLen += req->Params.AttributeLength;  /* AttributeValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeHandle), sizeof(req->Params.AttributeHandle)); idx += sizeof(req->Params.AttributeHandle);
	FLib_MemCpy(pMsg + idx, &(req->Params.AttributeLength), sizeof(req->Params.AttributeLength)); idx += sizeof(req->Params.AttributeLength);
	FLib_MemCpy(pMsg + idx, req->Params.AttributeValue, req->Params.AttributeLength); idx += req->Params.AttributeLength;

	/* Send the request */
	FSCI_transmitPayload(0x47, 0x0E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* GATTDB_ATT_ENABLE */

#if GAP_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GAPModeSelectRequest(GAPModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable GAP

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPModeSelectRequest(GAPModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x00, (void*)req, sizeof(GAPModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t BLEHostInitializeRequest(uint8_t fsciInterface)
\brief	Performs initialization of the BLE Host stack

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t BLEHostInitializeRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRegisterDeviceSecurityRequirementsRequest(GAPRegisterDeviceSecurityRequirementsRequest_t *req, uint8_t fsciInterface)
\brief	Registers the device security requirements

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRegisterDeviceSecurityRequirementsRequest(GAPRegisterDeviceSecurityRequirementsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* SecurityRequirementsIncluded */

	if (req->SecurityRequirementsIncluded)
	{
		msgLen += sizeof(uint8_t);  /* SecurityModeLevel */
		msgLen += sizeof(bool_t);  /* Authorization */
		msgLen += sizeof(uint16_t);  /* MinimumEncryptionKeySize */
		msgLen += sizeof(uint8_t);  /* NbOfServices */

		for (uint32_t i = 0; i < req->SecurityRequirements.NbOfServices; i++)
		{
			msgLen += sizeof(uint16_t);  /* ServiceHandle */
			msgLen += sizeof(uint8_t);  /* SecurityModeLevel */
			msgLen += sizeof(bool_t);  /* Authorization */
			msgLen += sizeof(uint16_t);  /* MinimumEncryptionKeySize */
		}		/* GapServiceSecurityRequirements */
	}	/* SecurityRequirements */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->SecurityRequirementsIncluded; idx++;

	if (req->SecurityRequirementsIncluded)
	{
		pMsg[idx] = req->SecurityRequirements.SecurityRequirements.SecurityModeLevel; idx++;
		pMsg[idx] = req->SecurityRequirements.SecurityRequirements.Authorization; idx++;
		FLib_MemCpy(pMsg + idx, &(req->SecurityRequirements.SecurityRequirements.MinimumEncryptionKeySize), sizeof(req->SecurityRequirements.SecurityRequirements.MinimumEncryptionKeySize)); idx += sizeof(req->SecurityRequirements.SecurityRequirements.MinimumEncryptionKeySize);
		pMsg[idx] = req->SecurityRequirements.NbOfServices; idx++;

		for (uint32_t i = 0; i < req->SecurityRequirements.NbOfServices; i++)
		{
			FLib_MemCpy(pMsg + idx, &(req->SecurityRequirements.GapServiceSecurityRequirements[i].ServiceHandle), sizeof(req->SecurityRequirements.GapServiceSecurityRequirements[i].ServiceHandle)); idx += sizeof(req->SecurityRequirements.GapServiceSecurityRequirements[i].ServiceHandle);
			pMsg[idx] = req->SecurityRequirements.GapServiceSecurityRequirements[i].Requirements.SecurityModeLevel; idx++;
			pMsg[idx] = req->SecurityRequirements.GapServiceSecurityRequirements[i].Requirements.Authorization; idx++;
			FLib_MemCpy(pMsg + idx, &(req->SecurityRequirements.GapServiceSecurityRequirements[i].Requirements.MinimumEncryptionKeySize), sizeof(req->SecurityRequirements.GapServiceSecurityRequirements[i].Requirements.MinimumEncryptionKeySize)); idx += sizeof(req->SecurityRequirements.GapServiceSecurityRequirements[i].Requirements.MinimumEncryptionKeySize);
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x02, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetAdvertisingParametersRequest(GAPSetAdvertisingParametersRequest_t *req, uint8_t fsciInterface)
\brief	Sets the advertising parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetAdvertisingParametersRequest(GAPSetAdvertisingParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x03, (void*)req, sizeof(GAPSetAdvertisingParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetAdvertisingDataRequest(GAPSetAdvertisingDataRequest_t *req, uint8_t fsciInterface)
\brief	Sets the advertising and scan response data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetAdvertisingDataRequest(GAPSetAdvertisingDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* AdvertisingDataIncluded */

	if (req->AdvertisingDataIncluded)
	{
		msgLen += sizeof(uint8_t);  /* NbOfAdStructures */

		for (uint32_t i = 0; i < req->AdvertisingData.NbOfAdStructures; i++)
		{
			msgLen += sizeof(uint8_t);  /* Length */
			msgLen += sizeof(uint8_t);  /* Type */
			msgLen += req->AdvertisingData.AdStructures[i].Length;  /* Data */
		}		/* AdStructures */
	}	/* AdvertisingData */

	msgLen += sizeof(bool_t);  /* ScanResponseDataIncluded */

	if (req->ScanResponseDataIncluded)
	{
		msgLen += sizeof(uint8_t);  /* NbOfAdStructures */

		for (uint32_t i = 0; i < req->ScanResponseData.NbOfAdStructures; i++)
		{
			msgLen += sizeof(uint8_t);  /* Length */
			msgLen += sizeof(uint8_t);  /* Type */
			msgLen += req->ScanResponseData.AdStructures[i].Length;  /* Data */
		}		/* AdStructures */
	}	/* ScanResponseData */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->AdvertisingDataIncluded; idx++;

	if (req->AdvertisingDataIncluded)
	{
		pMsg[idx] = req->AdvertisingData.NbOfAdStructures; idx++;

		for (uint32_t i = 0; i < req->AdvertisingData.NbOfAdStructures; i++)
		{
			pMsg[idx] = req->AdvertisingData.AdStructures[i].Length; idx++;
			pMsg[idx] = req->AdvertisingData.AdStructures[i].Type; idx++;
			FLib_MemCpy(pMsg + idx, req->AdvertisingData.AdStructures[i].Data, req->AdvertisingData.AdStructures[i].Length); idx += req->AdvertisingData.AdStructures[i].Length;
		}
	}
	pMsg[idx] = req->ScanResponseDataIncluded; idx++;

	if (req->ScanResponseDataIncluded)
	{
		pMsg[idx] = req->ScanResponseData.NbOfAdStructures; idx++;

		for (uint32_t i = 0; i < req->ScanResponseData.NbOfAdStructures; i++)
		{
			pMsg[idx] = req->ScanResponseData.AdStructures[i].Length; idx++;
			pMsg[idx] = req->ScanResponseData.AdStructures[i].Type; idx++;
			FLib_MemCpy(pMsg + idx, req->ScanResponseData.AdStructures[i].Data, req->ScanResponseData.AdStructures[i].Length); idx += req->ScanResponseData.AdStructures[i].Length;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x04, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStartAdvertisingRequest(uint8_t fsciInterface)
\brief	Commands the controller to start advertising

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPStartAdvertisingRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x05, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStopAdvertisingRequest(uint8_t fsciInterface)
\brief	Commands the controller to stop advertising

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPStopAdvertisingRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x06, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPAuthorizeRequest(GAPAuthorizeRequest_t *req, uint8_t fsciInterface)
\brief	Authorizes a peer for a certain attribute in the database

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPAuthorizeRequest(GAPAuthorizeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x07, (void*)req, sizeof(GAPAuthorizeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSaveCccdRequest(GAPSaveCccdRequest_t *req, uint8_t fsciInterface)
\brief	Save the CCCD value for a specific client and CCCD handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSaveCccdRequest(GAPSaveCccdRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x08, (void*)req, sizeof(GAPSaveCccdRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPCheckNotificationStatusRequest(GAPCheckNotificationStatusRequest_t *req, uint8_t fsciInterface)
\brief	Retrieves the notification status for a given client and a given CCCD handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPCheckNotificationStatusRequest(GAPCheckNotificationStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x09, (void*)req, sizeof(GAPCheckNotificationStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPCheckIndicationStatusRequest(GAPCheckIndicationStatusRequest_t *req, uint8_t fsciInterface)
\brief	Retrieves the indication status for a given client and a given CCCD handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPCheckIndicationStatusRequest(GAPCheckIndicationStatusRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x0A, (void*)req, sizeof(GAPCheckIndicationStatusRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPairRequest(GAPPairRequest_t *req, uint8_t fsciInterface)
\brief	Initiate pairing with a peer device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPairRequest(GAPPairRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x0C, (void*)req, sizeof(GAPPairRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSendPeripheralSecurityRequestRequest(GAPSendPeripheralSecurityRequestRequest_t *req, uint8_t fsciInterface)
\brief	Informs the peer Central about local security requirements

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSendPeripheralSecurityRequestRequest(GAPSendPeripheralSecurityRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x0D, (void*)req, sizeof(GAPSendPeripheralSecurityRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEncryptLinkRequest(GAPEncryptLinkRequest_t *req, uint8_t fsciInterface)
\brief	Encrypts the link with a bonded peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEncryptLinkRequest(GAPEncryptLinkRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x0E, (void*)req, sizeof(GAPEncryptLinkRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPAcceptPairingRequestRequest(GAPAcceptPairingRequestRequest_t *req, uint8_t fsciInterface)
\brief	Accepts the pairing request from a peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPAcceptPairingRequestRequest(GAPAcceptPairingRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x0F, (void*)req, sizeof(GAPAcceptPairingRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRejectPairingRequest(GAPRejectPairingRequest_t *req, uint8_t fsciInterface)
\brief	Rejects the peer's authentication request

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRejectPairingRequest(GAPRejectPairingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x10, (void*)req, sizeof(GAPRejectPairingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnterPasskeyRequest(GAPEnterPasskeyRequest_t *req, uint8_t fsciInterface)
\brief	Enters the passkey requested by a peer during the pairing process

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnterPasskeyRequest(GAPEnterPasskeyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x11, (void*)req, sizeof(GAPEnterPasskeyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPProvideOobRequest(GAPProvideOobRequest_t *req, uint8_t fsciInterface)
\brief	Provides the Out Of Band data for SMP pairing process

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPProvideOobRequest(GAPProvideOobRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x12, (void*)req, sizeof(GAPProvideOobRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRejectPasskeyRequestRequest(GAPRejectPasskeyRequestRequest_t *req, uint8_t fsciInterface)
\brief	Rejects the passkey request from a peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRejectPasskeyRequestRequest(GAPRejectPasskeyRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x13, (void*)req, sizeof(GAPRejectPasskeyRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSendSmpKeysRequest(GAPSendSmpKeysRequest_t *req, uint8_t fsciInterface)
\brief	Sends the SMP keys during SMP Key Exchange procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSendSmpKeysRequest(GAPSendSmpKeysRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(bool_t);  /* LtkIncluded */

	if (req->Keys.LtkIncluded)
	{
		msgLen += sizeof(uint8_t);  /* LtkSize */
		msgLen += req->Keys.LtkInfo.LtkSize;  /* Ltk */
	}	/* LtkInfo */

	msgLen += sizeof(bool_t);  /* IrkIncluded */

	if (req->Keys.IrkIncluded)
	{
		msgLen += 16;
	}  /* Irk */

	msgLen += sizeof(bool_t);  /* CsrkIncluded */

	if (req->Keys.CsrkIncluded)
	{
		msgLen += 16;
	}  /* Csrk */


	if (req->Keys.LtkIncluded)
	{
		msgLen += sizeof(uint8_t);  /* RandSize */
		msgLen += req->Keys.RandEdivInfo.RandSize;  /* Rand */
		msgLen += sizeof(uint16_t);  /* Ediv */
	}	/* RandEdivInfo */

	msgLen += sizeof(bool_t);  /* AddressIncluded */


	if (req->Keys.AddressIncluded)
	{
		msgLen += sizeof(uint8_t);  /* DeviceAddressType */
		msgLen += 6;  /* DeviceAddress */
	}	/* AddressInfo */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->Keys.LtkIncluded; idx++;

	if (req->Keys.LtkIncluded)
	{
		pMsg[idx] = req->Keys.LtkInfo.LtkSize; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.LtkInfo.Ltk, req->Keys.LtkInfo.LtkSize); idx += req->Keys.LtkInfo.LtkSize;
	}
	pMsg[idx] = req->Keys.IrkIncluded; idx++;

	if (req->Keys.IrkIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->Keys.Irk, 16); idx += 16;
	}

	pMsg[idx] = req->Keys.CsrkIncluded; idx++;

	if (req->Keys.CsrkIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->Keys.Csrk, 16); idx += 16;
	}


	if (req->Keys.LtkIncluded)
	{
		pMsg[idx] = req->Keys.RandEdivInfo.RandSize; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.RandEdivInfo.Rand, req->Keys.RandEdivInfo.RandSize); idx += req->Keys.RandEdivInfo.RandSize;
		FLib_MemCpy(pMsg + idx, &(req->Keys.RandEdivInfo.Ediv), sizeof(req->Keys.RandEdivInfo.Ediv)); idx += sizeof(req->Keys.RandEdivInfo.Ediv);
	}

	if (req->Keys.IrkIncluded)
	{
		pMsg[idx] = req->Keys.AddressIncluded; idx++;
	}


	if (req->Keys.AddressIncluded)
	{
		pMsg[idx] = req->Keys.AddressInfo.DeviceAddressType; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.AddressInfo.DeviceAddress, 6); idx += 6;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x14, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRejectKeyExchangeRequestRequest(GAPRejectKeyExchangeRequestRequest_t *req, uint8_t fsciInterface)
\brief	Rejects the Key Exchange procedure with a paired peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRejectKeyExchangeRequestRequest(GAPRejectKeyExchangeRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x15, (void*)req, sizeof(GAPRejectKeyExchangeRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPProvideLongTermKeyRequest(GAPProvideLongTermKeyRequest_t *req, uint8_t fsciInterface)
\brief	Provides the Long Term Key to the controller for encryption setup

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPProvideLongTermKeyRequest(GAPProvideLongTermKeyRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* LtkSize */
	msgLen += req->LtkSize;  /* Ltk */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->LtkSize; idx++;
	FLib_MemCpy(pMsg + idx, req->Ltk, req->LtkSize); idx += req->LtkSize;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x16, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPDenyLongTermKeyRequest(GAPDenyLongTermKeyRequest_t *req, uint8_t fsciInterface)
\brief	Rejects a long term key request from the controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPDenyLongTermKeyRequest(GAPDenyLongTermKeyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x17, (void*)req, sizeof(GAPDenyLongTermKeyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLoadEncryptionInformationRequest(GAPLoadEncryptionInformationRequest_t *req, uint8_t fsciInterface)
\brief	Loads encryption key for a bonded device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLoadEncryptionInformationRequest(GAPLoadEncryptionInformationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x18, (void*)req, sizeof(GAPLoadEncryptionInformationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetLocalPasskeyRequest(GAPSetLocalPasskeyRequest_t *req, uint8_t fsciInterface)
\brief	Sets the SMP passkey for this device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetLocalPasskeyRequest(GAPSetLocalPasskeyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x19, (void*)req, sizeof(GAPSetLocalPasskeyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStartScanningRequest(GAPStartScanningRequest_t *req, uint8_t fsciInterface)
\brief	Sets the scanning parameters (optionally) and begins scanning

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPStartScanningRequest(GAPStartScanningRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* ScanningParametersIncluded */

	if (req->ScanningParametersIncluded)
	{
		msgLen += sizeof(uint8_t);  /* Type */
		msgLen += sizeof(uint16_t);  /* Interval */
		msgLen += sizeof(uint16_t);  /* Window */
		msgLen += sizeof(uint8_t);  /* OwnAddressType */
		msgLen += sizeof(uint8_t);  /* FilterPolicy */
	}	/* ScanningParameters */

	msgLen += sizeof(uint8_t);  /* FilterDuplicates */
	msgLen += 1;  /* 0x04|0x02|0x01 */  /* ScanningPHYs */
	msgLen += sizeof(uint16_t);  /* Duration */
	msgLen += sizeof(uint16_t);  /* Period */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->ScanningParametersIncluded; idx++;

	if (req->ScanningParametersIncluded)
	{
		pMsg[idx] = req->ScanningParameters.Type; idx++;
		FLib_MemCpy(pMsg + idx, &(req->ScanningParameters.Interval), sizeof(req->ScanningParameters.Interval)); idx += sizeof(req->ScanningParameters.Interval);
		FLib_MemCpy(pMsg + idx, &(req->ScanningParameters.Window), sizeof(req->ScanningParameters.Window)); idx += sizeof(req->ScanningParameters.Window);
		pMsg[idx] = req->ScanningParameters.OwnAddressType; idx++;
		pMsg[idx] = req->ScanningParameters.FilterPolicy; idx++;
	}
	pMsg[idx] = req->FilterDuplicates; idx++;
	pMsg[idx] = req->ScanningPHYs; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Duration), sizeof(req->Duration)); idx += sizeof(req->Duration);
	FLib_MemCpy(pMsg + idx, &(req->Period), sizeof(req->Period)); idx += sizeof(req->Period);

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x1A, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStopScanningRequest(uint8_t fsciInterface)
\brief	Commands the controller to stop scanning

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPStopScanningRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x1B, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPConnectRequest(GAPConnectRequest_t *req, uint8_t fsciInterface)
\brief	Connects to a scanned device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPConnectRequest(GAPConnectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x1C, (void*)req, sizeof(GAPConnectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPDisconnectRequest(GAPDisconnectRequest_t *req, uint8_t fsciInterface)
\brief	Initiates disconnection from a connected peer device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPDisconnectRequest(GAPDisconnectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x1D, (void*)req, sizeof(GAPDisconnectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSaveCustomPeerInformationRequest(GAPSaveCustomPeerInformationRequest_t *req, uint8_t fsciInterface)
\brief	Saves custom peer information in raw data format

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSaveCustomPeerInformationRequest(GAPSaveCustomPeerInformationRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* Offset */
	msgLen += sizeof(uint16_t);  /* InfoSize */
	msgLen += req->InfoSize;  /* Info */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->Offset), sizeof(req->Offset)); idx += sizeof(req->Offset);
	FLib_MemCpy(pMsg + idx, &(req->InfoSize), sizeof(req->InfoSize)); idx += sizeof(req->InfoSize);
	FLib_MemCpy(pMsg + idx, req->Info, req->InfoSize); idx += req->InfoSize;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x1E, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLoadCustomPeerInformationRequest(GAPLoadCustomPeerInformationRequest_t *req, uint8_t fsciInterface)
\brief	Loads the custom peer information in raw data format

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLoadCustomPeerInformationRequest(GAPLoadCustomPeerInformationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x1F, (void*)req, sizeof(GAPLoadCustomPeerInformationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPCheckIfBondedRequest(GAPCheckIfBondedRequest_t *req, uint8_t fsciInterface)
\brief	Verifies if a connected peer device is bonded

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPCheckIfBondedRequest(GAPCheckIfBondedRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x20, (void*)req, sizeof(GAPCheckIfBondedRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadFilterAcceptListSizeRequest(uint8_t fsciInterface)
\brief	Retrieves the size of the Filter Accept List

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPReadFilterAcceptListSizeRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x21, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPClearFilterAcceptListRequest(uint8_t fsciInterface)
\brief	Removes all addresses from the Filter Accept List

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPClearFilterAcceptListRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x22, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPAddDeviceToFilterAcceptListRequest(GAPAddDeviceToFilterAcceptListRequest_t *req, uint8_t fsciInterface)
\brief	Adds a device address to the Filter Accept List

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPAddDeviceToFilterAcceptListRequest(GAPAddDeviceToFilterAcceptListRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x23, (void*)req, sizeof(GAPAddDeviceToFilterAcceptListRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRemoveDeviceFromFilterAcceptListRequest(GAPRemoveDeviceFromFilterAcceptListRequest_t *req, uint8_t fsciInterface)
\brief	Removes a device address from the Filter Accept List

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRemoveDeviceFromFilterAcceptListRequest(GAPRemoveDeviceFromFilterAcceptListRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x24, (void*)req, sizeof(GAPRemoveDeviceFromFilterAcceptListRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadPublicDeviceAddressRequest(uint8_t fsciInterface)
\brief	Reads the device's public address from the controller

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPReadPublicDeviceAddressRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x25, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPCreateRandomDeviceAddressRequest(GAPCreateRandomDeviceAddressRequest_t *req, uint8_t fsciInterface)
\brief	Requests the controller to create a random address

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPCreateRandomDeviceAddressRequest(GAPCreateRandomDeviceAddressRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* IrkIncluded */

	if (req->IrkIncluded)
	{
		msgLen += 16;
	}  /* Irk */

	msgLen += sizeof(bool_t);  /* RandomPartIncluded */


	if (req->RandomPartIncluded)
	{
		msgLen += 3;
	}  /* RandomPart */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->IrkIncluded; idx++;

	if (req->IrkIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->Irk, 16); idx += 16;
	}


	if (req->IrkIncluded)
	{
		pMsg[idx] = req->RandomPartIncluded; idx++;
	}


	if (req->RandomPartIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->RandomPart, sizeof(req->RandomPart)); idx += sizeof(req->RandomPart);
	}


	/* Send the request */
	FSCI_transmitPayload(0x48, 0x26, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSaveDeviceNameRequest(GAPSaveDeviceNameRequest_t *req, uint8_t fsciInterface)
\brief	Saves the name of a bonded device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSaveDeviceNameRequest(GAPSaveDeviceNameRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* NameSize */
	msgLen += req->NameSize;  /* Name */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->NameSize; idx++;
	FLib_MemCpy(pMsg + idx, req->Name, req->NameSize); idx += req->NameSize;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x27, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetBondedDevicesCountRequest(uint8_t fsciInterface)
\brief	Retrieves the number of bonded devices

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPGetBondedDevicesCountRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x28, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetBondedDeviceNameRequest(GAPGetBondedDeviceNameRequest_t *req, uint8_t fsciInterface)
\brief	Retrieves the name of a bonded device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGetBondedDeviceNameRequest(GAPGetBondedDeviceNameRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x29, (void*)req, sizeof(GAPGetBondedDeviceNameRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRemoveBondRequest(GAPRemoveBondRequest_t *req, uint8_t fsciInterface)
\brief	Removes the bond with a device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRemoveBondRequest(GAPRemoveBondRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x2A, (void*)req, sizeof(GAPRemoveBondRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRemoveAllBondsRequest(uint8_t fsciInterface)
\brief	Removes all bonds with other devices

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPRemoveAllBondsRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x2B, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadRadioPowerLevelRequest(GAPReadRadioPowerLevelRequest_t *req, uint8_t fsciInterface)
\brief	Reads the power level of the controller's radio

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPReadRadioPowerLevelRequest(GAPReadRadioPowerLevelRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x2C, (void*)req, sizeof(GAPReadRadioPowerLevelRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPVerifyPrivateResolvableAddressRequest(GAPVerifyPrivateResolvableAddressRequest_t *req, uint8_t fsciInterface)
\brief	Verifies a Private Resolvable Address with a bonded device's IRK

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPVerifyPrivateResolvableAddressRequest(GAPVerifyPrivateResolvableAddressRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x2D, (void*)req, sizeof(GAPVerifyPrivateResolvableAddressRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetRandomAddressRequest(GAPSetRandomAddressRequest_t *req, uint8_t fsciInterface)
\brief	Sets a random address into the controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetRandomAddressRequest(GAPSetRandomAddressRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x2E, (void*)req, sizeof(GAPSetRandomAddressRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetScanModeRequest(GAPSetScanModeRequest_t *req, uint8_t fsciInterface)
\brief	Sets internal scan filters and actions

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetScanModeRequest(GAPSetScanModeRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* ScanMode */
	msgLen += sizeof(uint8_t);  /* NbOfAddresses */
	msgLen += sizeof(bool_t);  /* WriteInFilterAcceptList */

	for (uint32_t i = 0; i < req->AutoConnectParams.NbOfAddresses; i++)
	{
		msgLen += sizeof(uint16_t);  /* ScanInterval */
		msgLen += sizeof(uint16_t);  /* ScanWindow */
		msgLen += sizeof(uint8_t);  /* FilterPolicy */
		msgLen += sizeof(uint8_t);  /* OwnAddressType */
		msgLen += sizeof(uint8_t);  /* PeerAddressType */
		msgLen += 6;  /* PeerAddress */
		msgLen += sizeof(uint16_t);  /* ConnIntervalMin */
		msgLen += sizeof(uint16_t);  /* ConnIntervalMax */
		msgLen += sizeof(uint16_t);  /* ConnLatency */
		msgLen += sizeof(uint16_t);  /* SupervisionTimeout */
		msgLen += sizeof(uint16_t);  /* ConnEventLengthMin */
		msgLen += sizeof(uint16_t);  /* ConnEventLengthMax */
		msgLen += sizeof(bool_t);  /* usePeerIdentityAddress */
		msgLen += 1;  /* 0x04|0x02|0x01 */  /* Initiating_PHYs */
	}	/* AutoConnectData */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->ScanMode; idx++;
	pMsg[idx] = req->AutoConnectParams.NbOfAddresses; idx++;
	pMsg[idx] = req->AutoConnectParams.WriteInFilterAcceptList; idx++;

	for (uint32_t i = 0; i < req->AutoConnectParams.NbOfAddresses; i++)
	{
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ScanInterval), sizeof(req->AutoConnectParams.AutoConnectData[i].ScanInterval)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ScanInterval);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ScanWindow), sizeof(req->AutoConnectParams.AutoConnectData[i].ScanWindow)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ScanWindow);
		pMsg[idx] = req->AutoConnectParams.AutoConnectData[i].FilterPolicy; idx++;
		pMsg[idx] = req->AutoConnectParams.AutoConnectData[i].OwnAddressType; idx++;
		pMsg[idx] = req->AutoConnectParams.AutoConnectData[i].PeerAddressType; idx++;
		FLib_MemCpy(pMsg + idx, req->AutoConnectParams.AutoConnectData[i].PeerAddress, 6); idx += 6;
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMin), sizeof(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMin)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMin);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMax), sizeof(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMax)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ConnIntervalMax);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ConnLatency), sizeof(req->AutoConnectParams.AutoConnectData[i].ConnLatency)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ConnLatency);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].SupervisionTimeout), sizeof(req->AutoConnectParams.AutoConnectData[i].SupervisionTimeout)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].SupervisionTimeout);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMin), sizeof(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMin)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMin);
		FLib_MemCpy(pMsg + idx, &(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMax), sizeof(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMax)); idx += sizeof(req->AutoConnectParams.AutoConnectData[i].ConnEventLengthMax);
		pMsg[idx] = req->AutoConnectParams.AutoConnectData[i].usePeerIdentityAddress; idx++;
		pMsg[idx] = req->AutoConnectParams.AutoConnectData[i].Initiating_PHYs; idx++;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x2F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetDefaultPairingParametersRequest(GAPSetDefaultPairingParametersRequest_t *req, uint8_t fsciInterface)
\brief	Sets the default pairing parameters to be used by automatic pairing procedures

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetDefaultPairingParametersRequest(GAPSetDefaultPairingParametersRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* PairingParametersIncluded */

	if (req->PairingParametersIncluded)
	{
		msgLen += sizeof(bool_t);  /* WithBonding */
		msgLen += sizeof(uint8_t);  /* SecurityModeAndLevel */
		msgLen += sizeof(uint8_t);  /* MaxEncryptionKeySize */
		msgLen += sizeof(uint8_t);  /* LocalIoCapabilities */
		msgLen += sizeof(bool_t);  /* OobAvailable */
		msgLen += 1;  /* 0x04|0x02|0x01 */  /* CentralKeys */
		msgLen += 1;  /* 0x04|0x02|0x01 */  /* PeripheralKeys */
		msgLen += sizeof(bool_t);  /* LeSecureConnectionSupported */
		msgLen += sizeof(bool_t);  /* UseKeypressNotifications */
	}	/* PairingParameters */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->PairingParametersIncluded; idx++;

	if (req->PairingParametersIncluded)
	{
		pMsg[idx] = req->PairingParameters.WithBonding; idx++;
		pMsg[idx] = req->PairingParameters.SecurityModeAndLevel; idx++;
		pMsg[idx] = req->PairingParameters.MaxEncryptionKeySize; idx++;
		pMsg[idx] = req->PairingParameters.LocalIoCapabilities; idx++;
		pMsg[idx] = req->PairingParameters.OobAvailable; idx++;
		pMsg[idx] = req->PairingParameters.CentralKeys; idx++;
		pMsg[idx] = req->PairingParameters.PeripheralKeys; idx++;
		pMsg[idx] = req->PairingParameters.LeSecureConnectionSupported; idx++;
		pMsg[idx] = req->PairingParameters.UseKeypressNotifications; idx++;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x30, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPUpdateConnectionParametersRequest(GAPUpdateConnectionParametersRequest_t *req, uint8_t fsciInterface)
\brief	Request a set of new connection parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPUpdateConnectionParametersRequest(GAPUpdateConnectionParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x31, (void*)req, sizeof(GAPUpdateConnectionParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableUpdateConnectionParametersRequest(GAPEnableUpdateConnectionParametersRequest_t *req, uint8_t fsciInterface)
\brief	Update the connection parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableUpdateConnectionParametersRequest(GAPEnableUpdateConnectionParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x32, (void*)req, sizeof(GAPEnableUpdateConnectionParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapUpdateLeDataLengthRequest(GapUpdateLeDataLengthRequest_t *req, uint8_t fsciInterface)
\brief	Update the Tx Data parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapUpdateLeDataLengthRequest(GapUpdateLeDataLengthRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x33, (void*)req, sizeof(GapUpdateLeDataLengthRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableHostPrivacyRequest(GAPEnableHostPrivacyRequest_t *req, uint8_t fsciInterface)
\brief	Enables or disables Host Privacy (automatic regeneration of a Private Address)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableHostPrivacyRequest(GAPEnableHostPrivacyRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* Enable */

	if (req->Enable)
	{
		msgLen += 16;
	}  /* Irk */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Enable; idx++;

	if (req->Enable)
	{
		FLib_MemCpy(pMsg + idx, req->Irk, 16); idx += 16;
	}


	/* Send the request */
	FSCI_transmitPayload(0x48, 0x35, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableControllerPrivacyRequest(GAPEnableControllerPrivacyRequest_t *req, uint8_t fsciInterface)
\brief	Enables or disables Controller Privacy (Enhanced Privacy feature)

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableControllerPrivacyRequest(GAPEnableControllerPrivacyRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(bool_t);  /* Enable */

	if (req->Enable)
	{
		msgLen += 16;
	}  /* OwnIrk */


	if (req->Enable)
	{
		msgLen += 1;
	}  /* PeerIdCount */


	for (uint32_t i = 0; i < req->PeerIdCount; i++)
	{
		msgLen += sizeof(uint8_t);  /* IdentityAddressType */
		msgLen += 6;  /* IdentityAddress */
		msgLen += 16;  /* Irk */
		msgLen += sizeof(uint8_t);  /* PrivacyMode */
	}	/* PeerIdentities */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Enable; idx++;

	if (req->Enable)
	{
		FLib_MemCpy(pMsg + idx, req->OwnIrk, 16); idx += 16;
	}


	if (req->Enable)
	{
		pMsg[idx] = req->PeerIdCount; idx++;
	}


	for (uint32_t i = 0; i < req->PeerIdCount; i++)
	{
		pMsg[idx] = req->PeerIdentities[i].IdentityAddressType; idx++;
		FLib_MemCpy(pMsg + idx, req->PeerIdentities[i].IdentityAddress, 6); idx += 6;
		FLib_MemCpy(pMsg + idx, req->PeerIdentities[i].Irk, 16); idx += 16;
		pMsg[idx] = req->PeerIdentities[i].PrivacyMode; idx++;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x36, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeScRegeneratePublicKeyRequest(uint8_t fsciInterface)
\brief	Regenerate the private/public key pair used for LE Secure Connections pairing

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPLeScRegeneratePublicKeyRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x37, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeScValidateNumericValueRequest(GAPLeScValidateNumericValueRequest_t *req, uint8_t fsciInterface)
\brief	Validate the numeric value during the Numeric Comparison LE Secure Connections pairing

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeScValidateNumericValueRequest(GAPLeScValidateNumericValueRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x38, (void*)req, sizeof(GAPLeScValidateNumericValueRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeScGetLocalOobDataRequest(uint8_t fsciInterface)
\brief	Retrieve local OOB data used for LE Secure Connections pairing

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPLeScGetLocalOobDataRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x39, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeScSetPeerOobDataRequest(GAPLeScSetPeerOobDataRequest_t *req, uint8_t fsciInterface)
\brief	Set peer OOB data used for LE Secure Connections pairing

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeScSetPeerOobDataRequest(GAPLeScSetPeerOobDataRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3A, (void*)req, sizeof(GAPLeScSetPeerOobDataRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeScSendKeypressNotificationPrivacyRequest(GAPLeScSendKeypressNotificationPrivacyRequest_t *req, uint8_t fsciInterface)
\brief	Send a Keypress Notification to the peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeScSendKeypressNotificationPrivacyRequest(GAPLeScSendKeypressNotificationPrivacyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3B, (void*)req, sizeof(GAPLeScSendKeypressNotificationPrivacyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetBondedDevicesIdentityInformationRequest(GAPGetBondedDevicesIdentityInformationRequest_t *req, uint8_t fsciInterface)
\brief	Retrieves a list of the identity information of bonded devices, if any

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGetBondedDevicesIdentityInformationRequest(GAPGetBondedDevicesIdentityInformationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3C, (void*)req, sizeof(GAPGetBondedDevicesIdentityInformationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetTxPowerLevelRequest(GAPSetTxPowerLevelRequest_t *req, uint8_t fsciInterface)
\brief	Sets a Transmission power level in the controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetTxPowerLevelRequest(GAPSetTxPowerLevelRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3D, (void*)req, sizeof(GAPSetTxPowerLevelRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeReadPhyRequest(GAPLeReadPhyRequest_t *req, uint8_t fsciInterface)
\brief	Read the Tx and Rx Phy on the connection with a device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeReadPhyRequest(GAPLeReadPhyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3E, (void*)req, sizeof(GAPLeReadPhyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeSetPhyRequest(GAPLeSetPhyRequest_t *req, uint8_t fsciInterface)
\brief	Set the Tx and Rx Phy preferences on the connection with a device or all subsequent connections

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeSetPhyRequest(GAPLeSetPhyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x3F, (void*)req, sizeof(GAPLeSetPhyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPControllerEnhancedNotificationRequest(GAPControllerEnhancedNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Configure enhanced ADV/SCAN/CONN events in the controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPControllerEnhancedNotificationRequest(GAPControllerEnhancedNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x40, (void*)req, sizeof(GAPControllerEnhancedNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLoadKeysRequest(GAPLoadKeysRequest_t *req, uint8_t fsciInterface)
\brief	Load the bond keys from an NVM index

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLoadKeysRequest(GAPLoadKeysRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x41, (void*)req, sizeof(GAPLoadKeysRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSaveKeysRequest(GAPSaveKeysRequest_t *req, uint8_t fsciInterface)
\brief	Save the bond keys to an NVM index

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSaveKeysRequest(GAPSaveKeysRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* NvmIndex */
	msgLen += sizeof(bool_t);  /* LtkIncluded */

	if (req->Keys.LtkIncluded)
	{
		msgLen += sizeof(uint8_t);  /* LtkSize */
		msgLen += req->Keys.LtkInfo.LtkSize;  /* Ltk */
	}	/* LtkInfo */

	msgLen += sizeof(bool_t);  /* IrkIncluded */

	if (req->Keys.IrkIncluded)
	{
		msgLen += 16;
	}  /* Irk */

	msgLen += sizeof(bool_t);  /* CsrkIncluded */

	if (req->Keys.CsrkIncluded)
	{
		msgLen += 16;
	}  /* Csrk */


	if (req->Keys.LtkIncluded)
	{
		msgLen += sizeof(uint8_t);  /* RandSize */
		msgLen += req->Keys.RandEdivInfo.RandSize;  /* Rand */
		msgLen += sizeof(uint16_t);  /* Ediv */
	}	/* RandEdivInfo */

	msgLen += sizeof(bool_t);  /* AddressIncluded */


	if (req->Keys.AddressIncluded)
	{
		msgLen += sizeof(uint8_t);  /* DeviceAddressType */
		msgLen += 6;  /* DeviceAddress */
	}	/* AddressInfo */

	msgLen += sizeof(bool_t);  /* LeSc */
	msgLen += sizeof(bool_t);  /* Authenticated */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->NvmIndex; idx++;
	pMsg[idx] = req->Keys.LtkIncluded; idx++;

	if (req->Keys.LtkIncluded)
	{
		pMsg[idx] = req->Keys.LtkInfo.LtkSize; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.LtkInfo.Ltk, req->Keys.LtkInfo.LtkSize); idx += req->Keys.LtkInfo.LtkSize;
	}
	pMsg[idx] = req->Keys.IrkIncluded; idx++;

	if (req->Keys.IrkIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->Keys.Irk, 16); idx += 16;
	}

	pMsg[idx] = req->Keys.CsrkIncluded; idx++;

	if (req->Keys.CsrkIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->Keys.Csrk, 16); idx += 16;
	}


	if (req->Keys.LtkIncluded)
	{
		pMsg[idx] = req->Keys.RandEdivInfo.RandSize; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.RandEdivInfo.Rand, req->Keys.RandEdivInfo.RandSize); idx += req->Keys.RandEdivInfo.RandSize;
		FLib_MemCpy(pMsg + idx, &(req->Keys.RandEdivInfo.Ediv), sizeof(req->Keys.RandEdivInfo.Ediv)); idx += sizeof(req->Keys.RandEdivInfo.Ediv);
	}

	if (req->Keys.IrkIncluded)
	{
		pMsg[idx] = req->Keys.AddressIncluded; idx++;
	}


	if (req->Keys.AddressIncluded)
	{
		pMsg[idx] = req->Keys.AddressInfo.DeviceAddressType; idx++;
		FLib_MemCpy(pMsg + idx, req->Keys.AddressInfo.DeviceAddress, 6); idx += 6;
	}
	pMsg[idx] = req->LeSc; idx++;
	pMsg[idx] = req->Authenticated; idx++;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x42, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetChannelMapRequest(GAPSetChannelMapRequest_t *req, uint8_t fsciInterface)
\brief	Set a channel classification in the Controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetChannelMapRequest(GAPSetChannelMapRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x43, (void*)req, sizeof(GAPSetChannelMapRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadChannelMapRequest(GAPReadChannelMapRequest_t *req, uint8_t fsciInterface)
\brief	Read the channel map from a connected peer

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPReadChannelMapRequest(GAPReadChannelMapRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x44, (void*)req, sizeof(GAPReadChannelMapRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetPrivacyModeRequest(GAPSetPrivacyModeRequest_t *req, uint8_t fsciInterface)
\brief	Set the privacy mode for an NVM index

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetPrivacyModeRequest(GAPSetPrivacyModeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x45, (void*)req, sizeof(GAPSetPrivacyModeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ControllerSetScanDupFiltModeRequest(ControllerSetScanDupFiltModeRequest_t *req, uint8_t fsciInterface)
\brief	Configure Duplicate Filtering Behavior

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ControllerSetScanDupFiltModeRequest(ControllerSetScanDupFiltModeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x46, (void*)req, sizeof(ControllerSetScanDupFiltModeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadControllerLocalRPARequest(GAPReadControllerLocalRPARequest_t *req, uint8_t fsciInterface)
\brief	Read Controller Local Resolvable Private Address

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPReadControllerLocalRPARequest(GAPReadControllerLocalRPARequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x47, (void*)req, sizeof(GAPReadControllerLocalRPARequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPCheckNvmIndexRequest(GAPCheckNvmIndexRequest_t *req, uint8_t fsciInterface)
\brief	Verifies if an NVM index is free or contains a bond

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPCheckNvmIndexRequest(GAPCheckNvmIndexRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x49, (void*)req, sizeof(GAPCheckNvmIndexRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetDeviceIdFromConnHandleRequest(GAPGetDeviceIdFromConnHandleRequest_t *req, uint8_t fsciInterface)
\brief	Get the device id corresponding to the given connection handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGetDeviceIdFromConnHandleRequest(GAPGetDeviceIdFromConnHandleRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x4A, (void*)req, sizeof(GAPGetDeviceIdFromConnHandleRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetConnectionHandleFromDeviceId(GAPGetConnectionHandleFromDeviceId_t *req, uint8_t fsciInterface)
\brief	Get the connection handle corresponding to the given device id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGetConnectionHandleFromDeviceId(GAPGetConnectionHandleFromDeviceId_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x4B, (void*)req, sizeof(GAPGetConnectionHandleFromDeviceId_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPAdvIndexChangeRequest(GAPAdvIndexChangeRequest_t *req, uint8_t fsciInterface)
\brief	Configures advertising index type.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPAdvIndexChangeRequest(GAPAdvIndexChangeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x4C, (void*)req, sizeof(GAPAdvIndexChangeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetHostVersion(uint8_t fsciInterface)
\brief	Retrieves Host Version information

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPGetHostVersion(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x4D, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadRemoteVersionInfoRequest(GAPReadRemoteVersionInfoRequest_t *req, uint8_t fsciInterface)
\brief	Reads the version information of a connected peer device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPReadRemoteVersionInfoRequest(GAPReadRemoteVersionInfoRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x4E, (void*)req, sizeof(GAPReadRemoteVersionInfoRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGetConnParamsRequest(GAPGetConnParamsRequest_t *req, uint8_t fsciInterface)
\brief	Gets the connection parameters for the given deviceId

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGetConnParamsRequest(GAPGetConnParamsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x4F, (void*)req, sizeof(GAPGetConnParamsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetExtAdvertisingParametersRequest(GAPSetExtAdvertisingParametersRequest_t *req, uint8_t fsciInterface)
\brief	Sets the advertising parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetExtAdvertisingParametersRequest(GAPSetExtAdvertisingParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x50, (void*)req, sizeof(GAPSetExtAdvertisingParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStartExtAdvertisingRequest(GAPStartExtAdvertisingRequest_t *req, uint8_t fsciInterface)
\brief	Commands the controller to start advertising

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPStartExtAdvertisingRequest(GAPStartExtAdvertisingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x51, (void*)req, sizeof(GAPStartExtAdvertisingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPRemoveAdvertisingSetRequest(GAPRemoveAdvertisingSetRequest_t *req, uint8_t fsciInterface)
\brief	Remove an advertising set from the Controller

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPRemoveAdvertisingSetRequest(GAPRemoveAdvertisingSetRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x52, (void*)req, sizeof(GAPRemoveAdvertisingSetRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStopExtAdvertisingRequest(GAPStopExtAdvertisingRequest_t *req, uint8_t fsciInterface)
\brief	Commands the controller to stop advertising

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPStopExtAdvertisingRequest(GAPStopExtAdvertisingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x53, (void*)req, sizeof(GAPStopExtAdvertisingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPUpdatePeriodicAdvListRequest(GAPUpdatePeriodicAdvListRequest_t *req, uint8_t fsciInterface)
\brief	Commands the controller to update the periodic advertiser list

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPUpdatePeriodicAdvListRequest(GAPUpdatePeriodicAdvListRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* Operation */

	switch (req->Operation)
	{
		case GAPUpdatePeriodicAdvListRequest_Operation_gAddDevice_c:
			msgLen += sizeof(uint8_t);  /* DeviceAddressType */
			msgLen += 6;  /* DeviceAddress */
			msgLen += sizeof(uint8_t);  /* SID */
			break;

		case GAPUpdatePeriodicAdvListRequest_Operation_gRemoveDevice_c:
			msgLen += sizeof(uint8_t);  /* DeviceAddressType */
			msgLen += 6;  /* DeviceAddress */
			msgLen += sizeof(uint8_t);  /* SID */
			break;

		case GAPUpdatePeriodicAdvListRequest_Operation_gRemoveAllDevices_c:
			break;
	}  /* OperationValue */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Operation; idx++;

	switch (req->Operation)
	{
		case GAPUpdatePeriodicAdvListRequest_Operation_gAddDevice_c:
			pMsg[idx] = req->OperationValue.gAddDevice_c.DeviceAddressType; idx++;
			FLib_MemCpy(pMsg + idx, req->OperationValue.gAddDevice_c.DeviceAddress, 6); idx += 6;
			pMsg[idx] = req->OperationValue.gAddDevice_c.SID; idx++;
			break;

		case GAPUpdatePeriodicAdvListRequest_Operation_gRemoveDevice_c:
			pMsg[idx] = req->OperationValue.gRemoveDevice_c.DeviceAddressType; idx++;
			FLib_MemCpy(pMsg + idx, req->OperationValue.gRemoveDevice_c.DeviceAddress, 6); idx += 6;
			pMsg[idx] = req->OperationValue.gRemoveDevice_c.SID; idx++;
			break;

		case GAPUpdatePeriodicAdvListRequest_Operation_gRemoveAllDevices_c:
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x54, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetPeriodicAdvParametersRequest(GAPSetPeriodicAdvParametersRequest_t *req, uint8_t fsciInterface)
\brief	Sets the advertising parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetPeriodicAdvParametersRequest(GAPSetPeriodicAdvParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x55, (void*)req, sizeof(GAPSetPeriodicAdvParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStartPeriodicAdvertisingRequest(GAPStartPeriodicAdvertisingRequest_t *req, uint8_t fsciInterface)
\brief	Start Periodic Advertising for a set Id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPStartPeriodicAdvertisingRequest(GAPStartPeriodicAdvertisingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x56, (void*)req, sizeof(GAPStartPeriodicAdvertisingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPStopPeriodicAdvertisingRequest(GAPStopPeriodicAdvertisingRequest_t *req, uint8_t fsciInterface)
\brief	Stop Periodic Advertising for a set Id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPStopPeriodicAdvertisingRequest(GAPStopPeriodicAdvertisingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x57, (void*)req, sizeof(GAPStopPeriodicAdvertisingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetExtAdvertisingDataRequest(GAPSetExtAdvertisingDataRequest_t *req, uint8_t fsciInterface)
\brief	Sets the extended advertising and extended scan response data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetExtAdvertisingDataRequest(GAPSetExtAdvertisingDataRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x58, (void*)req, sizeof(GAPSetExtAdvertisingDataRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetPeriodicAdvertisingDataRequest(GAPSetPeriodicAdvertisingDataRequest_t *req, uint8_t fsciInterface)
\brief	Sets the extended advertising and extended scan response data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetPeriodicAdvertisingDataRequest(GAPSetPeriodicAdvertisingDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* Handle */
	msgLen += sizeof(bool_t);  /* UpdateDID */
	msgLen += sizeof(uint8_t);  /* NbOfAdStructures */

	for (uint32_t i = 0; i < req->PeriodicAdvertisingData.NbOfAdStructures; i++)
	{
		msgLen += sizeof(uint8_t);  /* Length */
		msgLen += sizeof(uint8_t);  /* Type */
		msgLen += req->PeriodicAdvertisingData.AdStructures[i].Length;  /* Data */
	}	/* AdStructures */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Handle; idx++;
	pMsg[idx] = req->UpdateDID; idx++;
	pMsg[idx] = req->PeriodicAdvertisingData.NbOfAdStructures; idx++;

	for (uint32_t i = 0; i < req->PeriodicAdvertisingData.NbOfAdStructures; i++)
	{
		pMsg[idx] = req->PeriodicAdvertisingData.AdStructures[i].Length; idx++;
		pMsg[idx] = req->PeriodicAdvertisingData.AdStructures[i].Type; idx++;
		FLib_MemCpy(pMsg + idx, req->PeriodicAdvertisingData.AdStructures[i].Data, req->PeriodicAdvertisingData.AdStructures[i].Length); idx += req->PeriodicAdvertisingData.AdStructures[i].Length;
	}

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x59, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvCreateSyncRequest(GAPPeriodicAdvCreateSyncRequest_t *req, uint8_t fsciInterface)
\brief	Synchronize with a periodic advertiser

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvCreateSyncRequest(GAPPeriodicAdvCreateSyncRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x5A, (void*)req, sizeof(GAPPeriodicAdvCreateSyncRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvTerminateSyncRequest(GAPPeriodicAdvTerminateSyncRequest_t *req, uint8_t fsciInterface)
\brief	Stop reception of the periodic advertising identified by the SID parameter

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvTerminateSyncRequest(GAPPeriodicAdvTerminateSyncRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x5B, (void*)req, sizeof(GAPPeriodicAdvTerminateSyncRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPGenerateDHKeyV2Request(GAPGenerateDHKeyV2Request_t *req, uint8_t fsciInterface)
\brief	Generate DH Key

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPGenerateDHKeyV2Request(GAPGenerateDHKeyV2Request_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x65, (void*)req, sizeof(GAPGenerateDHKeyV2Request_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPModifySleepClockAccuracyRequest(GAPModifySleepClockAccuracyRequest_t *req, uint8_t fsciInterface)
\brief	Modify Sleep Clock Accuracy

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPModifySleepClockAccuracyRequest(GAPModifySleepClockAccuracyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x66, (void*)req, sizeof(GAPModifySleepClockAccuracyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ControllerConfigureAdvCodingSchemeRequest(ControllerConfigureAdvCodingSchemeRequest_t *req, uint8_t fsciInterface)
\brief	Configure Advertising Coding Scheme

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ControllerConfigureAdvCodingSchemeRequest(ControllerConfigureAdvCodingSchemeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x5C, (void*)req, sizeof(ControllerConfigureAdvCodingSchemeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetConnectionlessCteTransmitParametersRequest(GAPSetConnectionlessCteTransmitParametersRequest_t *req, uint8_t fsciInterface)
\brief	Set Connectionless CTE Transmit Parameters for an advertising set

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetConnectionlessCteTransmitParametersRequest(GAPSetConnectionlessCteTransmitParametersRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* Handle */
	msgLen += sizeof(uint8_t);  /* CteLength */
	msgLen += sizeof(uint8_t);  /* CteType */
	msgLen += sizeof(uint8_t);  /* CteCount */
	msgLen += sizeof(uint8_t);  /* SwitchingPatternLength */
	msgLen += req->SwitchingPatternLength;  /* AntennaIds */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Handle; idx++;
	pMsg[idx] = req->CteLength; idx++;
	pMsg[idx] = req->CteType; idx++;
	pMsg[idx] = req->CteCount; idx++;
	pMsg[idx] = req->SwitchingPatternLength; idx++;
	FLib_MemCpy(pMsg + idx, req->AntennaIds, req->SwitchingPatternLength); idx += req->SwitchingPatternLength;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x5D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableConnectionlessCteTransmitRequest(GAPEnableConnectionlessCteTransmitRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable Connectionless CTE Transmit for an advertising set

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableConnectionlessCteTransmitRequest(GAPEnableConnectionlessCteTransmitRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x5E, (void*)req, sizeof(GAPEnableConnectionlessCteTransmitRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableConnectionlessIqSamplingRequest(GAPEnableConnectionlessIqSamplingRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable Connectionless IQ sampling for an advertising train

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableConnectionlessIqSamplingRequest(GAPEnableConnectionlessIqSamplingRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* SyncHandle */
	msgLen += sizeof(uint8_t);  /* IqSamplingEnable */
	msgLen += sizeof(uint8_t);  /* SlotDurations */
	msgLen += sizeof(uint8_t);  /* MaxSampledCtes */
	msgLen += sizeof(uint8_t);  /* SwitchingPatternLength */
	msgLen += req->pIqSamplingParams.SwitchingPatternLength;  /* AntennaIds */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->SyncHandle), sizeof(req->SyncHandle)); idx += sizeof(req->SyncHandle);
	pMsg[idx] = req->pIqSamplingParams.IqSamplingEnable; idx++;
	pMsg[idx] = req->pIqSamplingParams.SlotDurations; idx++;
	pMsg[idx] = req->pIqSamplingParams.MaxSampledCtes; idx++;
	pMsg[idx] = req->pIqSamplingParams.SwitchingPatternLength; idx++;
	FLib_MemCpy(pMsg + idx, req->pIqSamplingParams.AntennaIds, req->pIqSamplingParams.SwitchingPatternLength); idx += req->pIqSamplingParams.SwitchingPatternLength;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x5F, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetConnectionCteReceiveParametersRequest(GAPSetConnectionCteReceiveParametersRequest_t *req, uint8_t fsciInterface)
\brief	Set CTE Receive Parameters for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetConnectionCteReceiveParametersRequest(GAPSetConnectionCteReceiveParametersRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint8_t);  /* IqSamplingEnable */
	msgLen += sizeof(uint8_t);  /* SlotDurations */
	msgLen += sizeof(uint8_t);  /* SwitchingPatternLength */
	msgLen += req->pReceiveParams.SwitchingPatternLength;  /* AntennaIds */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->pReceiveParams.IqSamplingEnable; idx++;
	pMsg[idx] = req->pReceiveParams.SlotDurations; idx++;
	pMsg[idx] = req->pReceiveParams.SwitchingPatternLength; idx++;
	FLib_MemCpy(pMsg + idx, req->pReceiveParams.AntennaIds, req->pReceiveParams.SwitchingPatternLength); idx += req->pReceiveParams.SwitchingPatternLength;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x60, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetConnectionCteTransmitParametersRequest(GAPSetConnectionCteTransmitParametersRequest_t *req, uint8_t fsciInterface)
\brief	Set CTE Transmit Parameters for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetConnectionCteTransmitParametersRequest(GAPSetConnectionCteTransmitParametersRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += 1;  /* 0x04|0x02|0x01 */  /* CteTypes */
	msgLen += sizeof(uint8_t);  /* SwitchingPatternLength */
	msgLen += req->pTransmitParams.SwitchingPatternLength;  /* AntennaIds */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	pMsg[idx] = req->pTransmitParams.CteTypes; idx++;
	pMsg[idx] = req->pTransmitParams.SwitchingPatternLength; idx++;
	FLib_MemCpy(pMsg + idx, req->pTransmitParams.AntennaIds, req->pTransmitParams.SwitchingPatternLength); idx += req->pTransmitParams.SwitchingPatternLength;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x61, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableConnectionCteRequestRequest(GAPEnableConnectionCteRequestRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable CTE Request procedure for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableConnectionCteRequestRequest(GAPEnableConnectionCteRequestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x62, (void*)req, sizeof(GAPEnableConnectionCteRequestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableConnectionCteResponseRequest(GAPEnableConnectionCteResponseRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable sending CTE Responses for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableConnectionCteResponseRequest(GAPEnableConnectionCteResponseRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x63, (void*)req, sizeof(GAPEnableConnectionCteResponseRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadAntennaInformationRequest(uint8_t fsciInterface)
\brief	Read Antenna Information

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAPReadAntennaInformationRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x48, 0x64, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t WritePublicDeviceAddressRequest(WritePublicDeviceAddressRequest_t *req, uint8_t fsciInterface)
\brief	Store the Public Device Address parameters to Flash

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t WritePublicDeviceAddressRequest(WritePublicDeviceAddressRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x48, (void*)req, sizeof(WritePublicDeviceAddressRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvReceiveEnableRequest(GAPPeriodicAdvReceiveEnableRequest_t *req, uint8_t fsciInterface)
\brief	Enable reports for the periodic advertising train identified by the Sync_Handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvReceiveEnableRequest(GAPPeriodicAdvReceiveEnableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x67, (void*)req, sizeof(GAPPeriodicAdvReceiveEnableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvReceiveDisableRequest(GAPPeriodicAdvReceiveDisableRequest_t *req, uint8_t fsciInterface)
\brief	Disable reports for the periodic advertising train identified by the Sync_Handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvReceiveDisableRequest(GAPPeriodicAdvReceiveDisableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x68, (void*)req, sizeof(GAPPeriodicAdvReceiveDisableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvSyncTransferRequest(GAPPeriodicAdvSyncTransferRequest_t *req, uint8_t fsciInterface)
\brief	Instruct the Controller to send synchronization information about the periodic advertising train identified by the sync handle to a connected device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvSyncTransferRequest(GAPPeriodicAdvSyncTransferRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x69, (void*)req, sizeof(GAPPeriodicAdvSyncTransferRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPeriodicAdvSetInfoTransferRequest(GAPPeriodicAdvSetInfoTransferRequest_t *req, uint8_t fsciInterface)
\brief	Instruct the Controller to send synchronization information about the periodic advertising to a connected device

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPeriodicAdvSetInfoTransferRequest(GAPPeriodicAdvSetInfoTransferRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6A, (void*)req, sizeof(GAPPeriodicAdvSetInfoTransferRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetPeriodicAdvSyncTransferParamsRequest(GAPSetPeriodicAdvSyncTransferParamsRequest_t *req, uint8_t fsciInterface)
\brief	Specify how the Controller will process periodic advertising synchronization information received from the device identified by the device id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetPeriodicAdvSyncTransferParamsRequest(GAPSetPeriodicAdvSyncTransferParamsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6B, (void*)req, sizeof(GAPSetPeriodicAdvSyncTransferParamsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetDefaultPeriodicAdvSyncTransferParamsRequest(GAPSetDefaultPeriodicAdvSyncTransferParamsRequest_t *req, uint8_t fsciInterface)
\brief	Specify the initial value for the mode, skip, timeout, and Constant Tone Extension type (set by the Gap_SetPeriodicAdvSyncTransferParams command) to be used for all subsequent connections over the LE transport.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetDefaultPeriodicAdvSyncTransferParamsRequest(GAPSetDefaultPeriodicAdvSyncTransferParamsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6C, (void*)req, sizeof(GAPSetDefaultPeriodicAdvSyncTransferParamsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnhancedReadTransmitPowerLevelRequest(GAPEnhancedReadTransmitPowerLevelRequest_t *req, uint8_t fsciInterface)
\brief	Read local current and maximum tx power levels for a certain connection and PHY

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnhancedReadTransmitPowerLevelRequest(GAPEnhancedReadTransmitPowerLevelRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6D, (void*)req, sizeof(GAPEnhancedReadTransmitPowerLevelRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPReadRemoteTransmitPowerLevelRequest(GAPReadRemoteTransmitPowerLevelRequest_t *req, uint8_t fsciInterface)
\brief	Read remote tx power for a certain connection and PHY

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPReadRemoteTransmitPowerLevelRequest(GAPReadRemoteTransmitPowerLevelRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6E, (void*)req, sizeof(GAPReadRemoteTransmitPowerLevelRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPSetPathLossReportingParametersRequest(GAPSetPathLossReportingParametersRequest_t *req, uint8_t fsciInterface)
\brief	Set path loss threshold reporting parameters for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPSetPathLossReportingParametersRequest(GAPSetPathLossReportingParametersRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x6F, (void*)req, sizeof(GAPSetPathLossReportingParametersRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnablePathLossReportingRequest(GAPEnablePathLossReportingRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable path loss threshold reporting for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnablePathLossReportingRequest(GAPEnablePathLossReportingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x70, (void*)req, sizeof(GAPEnablePathLossReportingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEnableTransmitPowerReportingRequest(GAPEnableTransmitPowerReportingRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable tx power reporting for a certain connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEnableTransmitPowerReportingRequest(GAPEnableTransmitPowerReportingRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x71, (void*)req, sizeof(GAPEnableTransmitPowerReportingRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEattConnectionRequest(GAPEattConnectionRequest_t *req, uint8_t fsciInterface)
\brief	Open up to 5 Enhanced ATT bearers.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEattConnectionRequest(GAPEattConnectionRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x72, (void*)req, sizeof(GAPEattConnectionRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEattConnectionAccept(GAPEattConnectionAccept_t *req, uint8_t fsciInterface)
\brief	Open up to 5 Enhanced ATT bearers.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEattConnectionAccept(GAPEattConnectionAccept_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x73, (void*)req, sizeof(GAPEattConnectionAccept_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEattReconfigureRequest(GAPEattReconfigureRequest_t *req, uint8_t fsciInterface)
\brief	Change the MTU of up to 5 Enhanced ATT bearers.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEattReconfigureRequest(GAPEattReconfigureRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* DeviceId */
	msgLen += sizeof(uint16_t);  /* MTU */
	msgLen += sizeof(uint16_t);  /* MPS */
	msgLen += sizeof(uint8_t);  /* NoOfBearers */
	msgLen += req->NoOfBearers;  /* BearerIds */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->DeviceId; idx++;
	FLib_MemCpy(pMsg + idx, &(req->MTU), sizeof(req->MTU)); idx += sizeof(req->MTU);
	FLib_MemCpy(pMsg + idx, &(req->MPS), sizeof(req->MPS)); idx += sizeof(req->MPS);
	pMsg[idx] = req->NoOfBearers; idx++;
	FLib_MemCpy(pMsg + idx, req->BearerIds, req->NoOfBearers); idx += req->NoOfBearers;

	/* Send the request */
	FSCI_transmitPayload(0x48, 0x74, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEattSendCreditsRequest(GAPEattSendCreditsRequest_t *req, uint8_t fsciInterface)
\brief	Open up to 5 Enhanced ATT bearers.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEattSendCreditsRequest(GAPEattSendCreditsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x75, (void*)req, sizeof(GAPEattSendCreditsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPEattDisconnectRequest(GAPEattDisconnectRequest_t *req, uint8_t fsciInterface)
\brief	Disconnect an enhanced EATT bearer.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPEattDisconnectRequest(GAPEattDisconnectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x76, (void*)req, sizeof(GAPEattDisconnectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t ControllerSetConnNotificationModeRequest(ControllerSetConnNotificationModeRequest_t *req, uint8_t fsciInterface)
\brief	Configures Controller Notification Mode

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t ControllerSetConnNotificationModeRequest(ControllerSetConnNotificationModeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x77, (void*)req, sizeof(ControllerSetConnNotificationModeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t DisablePrivacyPerAdvSetRequest(DisablePrivacyPerAdvSetRequest_t *req, uint8_t fsciInterface)
\brief	Configure an advertising set to ignore the privacy setting

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t DisablePrivacyPerAdvSetRequest(DisablePrivacyPerAdvSetRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x78, (void*)req, sizeof(DisablePrivacyPerAdvSetRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeSetSchedulerPriorityRequest(GAPLeSetSchedulerPriorityRequest_t *req, uint8_t fsciInterface)
\brief	Configure Duplicate Filtering Behavior

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeSetSchedulerPriorityRequest(GAPLeSetSchedulerPriorityRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x79, (void*)req, sizeof(GAPLeSetSchedulerPriorityRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPLeSetHostFeatureRequest(GAPLeSetHostFeatureRequest_t *req, uint8_t fsciInterface)
\brief	Set or clear a bit controlled by the Host in the Link Layer FeatureSet

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPLeSetHostFeatureRequest(GAPLeSetHostFeatureRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x7B, (void*)req, sizeof(GAPLeSetHostFeatureRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAPPlatformRegisterCallbackRequest(GAPPlatformRegisterCallbackRequest_t *req, uint8_t fsciInterface)
\brief	Register error callback for platform functions

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAPPlatformRegisterCallbackRequest(GAPPlatformRegisterCallbackRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x48, 0x7C, (void*)req, sizeof(GAPPlatformRegisterCallbackRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* GAP_ENABLE */

#if APP_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t APPThroughputTestRequest(APPThroughputTestRequest_t *req, uint8_t fsciInterface)
\brief	Start a throughput test

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t APPThroughputTestRequest(APPThroughputTestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x49, 0x01, (void*)req, sizeof(APPThroughputTestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* APP_ENABLE */

#if FSCI_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t FSCICPUResetRequest(uint8_t fsciInterface)
\brief	Reset CPU

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t FSCICPUResetRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0xA3, 0x08, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t FSCIGetNumberOfFreeBuffersRequest(uint8_t fsciInterface)
\brief	Get the number of free buffers

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t FSCIGetNumberOfFreeBuffersRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0xA3, 0x09, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t FSCIAllowDeviceToSleepRequest(FSCIAllowDeviceToSleepRequest_t *req, uint8_t fsciInterface)
\brief	FSCI-AllowDeviceToSleep.Request description

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t FSCIAllowDeviceToSleepRequest(FSCIAllowDeviceToSleepRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0xA3, 0x70, (void*)req, sizeof(FSCIAllowDeviceToSleepRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t FSCIMemAllocTestRequest(FSCIMemAllocTestRequest_t *req, uint8_t fsciInterface)
\brief	Add possibility that for a number of times or a period of time a memory block to not be allocated for a specific location identified with a link register interval.

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t FSCIMemAllocTestRequest(FSCIMemAllocTestRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0xA3, 0xFC, (void*)req, sizeof(FSCIMemAllocTestRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t FSCIGetWakeupReasonRequest(uint8_t fsciInterface)
\brief	FSCI-GetWakeupReason.Request description

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t FSCIGetWakeupReasonRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0xA3, 0x72, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t FSCIGetNbuVersionRequest(uint8_t fsciInterface)
\brief	FSCI-GetNbuVersion.Request description

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t FSCIGetNbuVersionRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0xA3, 0xB3, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* FSCI_ENABLE */

#if NVM_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t FSCINVGetNvVPSizeRequest(uint8_t fsciInterface)
\brief	Get the NVM Virtual Page Size

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t FSCINVGetNvVPSizeRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0xA7, 0xE7, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* NVM_ENABLE */

#if CS_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t CSModeSelectRequest(CSModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable CS

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSModeSelectRequest(CSModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x00, (void*)req, sizeof(CSModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSInitRequest(uint8_t fsciInterface)
\brief	Register the CS-to-Controller callback

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t CSInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4A, 0x01, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSReadLocalSupportedCapabilitiesRequest(uint8_t fsciInterface)
\brief	LE CS Read Local Supported Capabilities Command

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t CSReadLocalSupportedCapabilitiesRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4A, 0x02, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSReadRemoteSupportedCapabilitiesRequest(CSReadRemoteSupportedCapabilitiesRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Read Remote Supported Capabilities Command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSReadRemoteSupportedCapabilitiesRequest(CSReadRemoteSupportedCapabilitiesRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x03, (void*)req, sizeof(CSReadRemoteSupportedCapabilitiesRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSSecurityEnableRequest(CSSecurityEnableRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Security Enable Command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSSecurityEnableRequest(CSSecurityEnableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x04, (void*)req, sizeof(CSSecurityEnableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSSetDefaultSettingsRequest(CSSetDefaultSettingsRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Set Default Settings Command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSSetDefaultSettingsRequest(CSSetDefaultSettingsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x05, (void*)req, sizeof(CSSetDefaultSettingsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSReadRemoteFAETableRequest(CSReadRemoteFAETableRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Read Remote Local FAE Table command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSReadRemoteFAETableRequest(CSReadRemoteFAETableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x06, (void*)req, sizeof(CSReadRemoteFAETableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSWriteRemoteFAETableRequest(CSWriteRemoteFAETableRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Write Remote Local FAE Table command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSWriteRemoteFAETableRequest(CSWriteRemoteFAETableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x07, (void*)req, sizeof(CSWriteRemoteFAETableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSCreateConfigRequest(CSCreateConfigRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Create Config command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSCreateConfigRequest(CSCreateConfigRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x08, (void*)req, sizeof(CSCreateConfigRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSRemoveConfigRequest(CSRemoveConfigRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Remove Config command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSRemoveConfigRequest(CSRemoveConfigRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x09, (void*)req, sizeof(CSRemoveConfigRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSSetChannelClassificationRequest(CSSetChannelClassificationRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Set Channel Classification command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSSetChannelClassificationRequest(CSSetChannelClassificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x0A, (void*)req, sizeof(CSSetChannelClassificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSSetProcedureParamsRequest(CSSetProcedureParamsRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Set Procedure Parameters command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSSetProcedureParamsRequest(CSSetProcedureParamsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x0B, (void*)req, sizeof(CSSetProcedureParamsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSProcedureEnableRequest(CSProcedureEnableRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Procedure Enable command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSProcedureEnableRequest(CSProcedureEnableRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x0C, (void*)req, sizeof(CSProcedureEnableRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSTestRequest(CSTestRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Test command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSTestRequest(CSTestRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* MainModeType */
	msgLen += sizeof(uint8_t);  /* MainModeRepetition */
	msgLen += sizeof(uint8_t);  /* SubModeType */
	msgLen += sizeof(uint8_t);  /* Mode0Steps */
	msgLen += sizeof(uint8_t);  /* Role */
	msgLen += sizeof(uint8_t);  /* RTTType */
	msgLen += sizeof(uint8_t);  /* CSSyncPhy */
	msgLen += sizeof(uint8_t);  /* CSSYNCAntennaSelection */
	msgLen += sizeof(req->SubeventLen);  /* SubeventLen */
	msgLen += sizeof(uint16_t);  /* SubeventInterval */
	msgLen += sizeof(uint8_t);  /* MaxNumSubevents */
	msgLen += sizeof(uint8_t);  /* TransmitPowerLevel */
	msgLen += sizeof(uint8_t);  /* TIP1time */
	msgLen += sizeof(uint8_t);  /* TIP2time */
	msgLen += sizeof(uint8_t);  /* TFCStime */
	msgLen += sizeof(uint8_t);  /* TPMtime */
	msgLen += sizeof(uint8_t);  /* TSWtime */
	msgLen += sizeof(uint8_t);  /* ToneAntennaConfig */
	msgLen += sizeof(bool_t);  /* CompanionSignalEnable */
	msgLen += sizeof(uint8_t);  /* SNRCtrlInitiator */
	msgLen += sizeof(uint8_t);  /* SNRCtrlReflector */
	msgLen += sizeof(uint16_t);  /* DRBG_Nonce */
	msgLen += sizeof(uint8_t);  /* ChannelMapRepetition */
	msgLen += sizeof(uint16_t);  /* OverrideConfig */
	msgLen += sizeof(uint8_t);  /* OverrideParametersLength */
	msgLen += req->OverrideParametersLength;  /* OverrideParametersData */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->MainModeType; idx++;
	pMsg[idx] = req->MainModeRepetition; idx++;
	pMsg[idx] = req->SubModeType; idx++;
	pMsg[idx] = req->Mode0Steps; idx++;
	pMsg[idx] = req->Role; idx++;
	pMsg[idx] = req->RTTType; idx++;
	pMsg[idx] = req->CSSyncPhy; idx++;
	pMsg[idx] = req->CSSYNCAntennaSelection; idx++;
	FLib_MemCpy(pMsg + idx, req->SubeventLen, sizeof(req->SubeventLen)); idx += sizeof(req->SubeventLen);
	FLib_MemCpy(pMsg + idx, &(req->SubeventInterval), sizeof(req->SubeventInterval)); idx += sizeof(req->SubeventInterval);
	pMsg[idx] = req->MaxNumSubevents; idx++;
	pMsg[idx] = req->TransmitPowerLevel; idx++;
	pMsg[idx] = req->TIP1time; idx++;
	pMsg[idx] = req->TIP2time; idx++;
	pMsg[idx] = req->TFCStime; idx++;
	pMsg[idx] = req->TPMtime; idx++;
	pMsg[idx] = req->TSWtime; idx++;
	pMsg[idx] = req->ToneAntennaConfig; idx++;
	pMsg[idx] = req->CompanionSignalEnable; idx++;
	pMsg[idx] = req->SNRCtrlInitiator; idx++;
	pMsg[idx] = req->SNRCtrlReflector; idx++;
	FLib_MemCpy(pMsg + idx, &(req->DRBG_Nonce), sizeof(req->DRBG_Nonce)); idx += sizeof(req->DRBG_Nonce);
	pMsg[idx] = req->ChannelMapRepetition; idx++;
	FLib_MemCpy(pMsg + idx, &(req->OverrideConfig), sizeof(req->OverrideConfig)); idx += sizeof(req->OverrideConfig);
	pMsg[idx] = req->OverrideParametersLength; idx++;
	FLib_MemCpy(pMsg + idx, req->OverrideParametersData, req->OverrideParametersLength); idx += req->OverrideParametersLength;

	/* Send the request */
	FSCI_transmitPayload(0x4A, 0x0D, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSTestEndRequest(uint8_t fsciInterface)
\brief	LE CS Test End command

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t CSTestEndRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4A, 0x0E, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t CSWriteCachedRemoteCapabilitiesRequest(CSWriteCachedRemoteCapabilitiesRequest_t *req, uint8_t fsciInterface)
\brief	LE CS Write Cached Remote Capabilities command

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t CSWriteCachedRemoteCapabilitiesRequest(CSWriteCachedRemoteCapabilitiesRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4A, 0x0F, (void*)req, sizeof(CSWriteCachedRemoteCapabilitiesRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

#endif  /* CS_ENABLE */

#if GAPHANDOVER_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GapHandoverModeSelectRequest(GapHandoverModeSelectRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable Gap Handover

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverModeSelectRequest(GapHandoverModeSelectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x00, (void*)req, sizeof(GapHandoverModeSelectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverGetDataSizeRequest(GapHandoverGetDataSizeRequest_t *req, uint8_t fsciInterface)
\brief	Returns the size in octets of the Handover Data for the given peer device id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverGetDataSizeRequest(GapHandoverGetDataSizeRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x01, (void*)req, sizeof(GapHandoverGetDataSizeRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverGetDataRequest(GapHandoverGetDataRequest_t *req, uint8_t fsciInterface)
\brief	Copies the data used by the Handover feature for the given peer device id

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverGetDataRequest(GapHandoverGetDataRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x02, (void*)req, sizeof(GapHandoverGetDataRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverSetDataRequest(GapHandoverSetDataRequest_t *req, uint8_t fsciInterface)
\brief	Sets the data used by the Handover feature

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverSetDataRequest(GapHandoverSetDataRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x03, (void*)req, sizeof(GapHandoverSetDataRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverGetTimeRequest(uint8_t fsciInterface)
\brief	Gets timing information from Link Layer to be used in Handover process

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GapHandoverGetTimeRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4B, 0x04, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverSuspendTransmitRequest(GapHandoverSuspendTransmitRequest_t *req, uint8_t fsciInterface)
\brief	Suspends TX for the given connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverSuspendTransmitRequest(GapHandoverSuspendTransmitRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x05, (void*)req, sizeof(GapHandoverSuspendTransmitRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverResumeTransmitRequest(GapHandoverResumeTransmitRequest_t *req, uint8_t fsciInterface)
\brief	Resumes TX for the given connection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverResumeTransmitRequest(GapHandoverResumeTransmitRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x06, (void*)req, sizeof(GapHandoverResumeTransmitRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverAnchorNotificationRequest(GapHandoverAnchorNotificationRequest_t *req, uint8_t fsciInterface)
\brief	Enables anchor notifications

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverAnchorNotificationRequest(GapHandoverAnchorNotificationRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x07, (void*)req, sizeof(GapHandoverAnchorNotificationRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverAnchorSearchStartRequest(GapHandoverAnchorSearchStartRequest_t *req, uint8_t fsciInterface)
\brief	Starts anchor search with given parameters

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverAnchorSearchStartRequest(GapHandoverAnchorSearchStartRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x08, (void*)req, sizeof(GapHandoverAnchorSearchStartRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverConnectRequest(GapHandoverConnectRequest_t *req, uint8_t fsciInterface)
\brief	Connect to a peer device initialized through the Connection Handover procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverConnectRequest(GapHandoverConnectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x09, (void*)req, sizeof(GapHandoverConnectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverDisconnectRequest(GapHandoverDisconnectRequest_t *req, uint8_t fsciInterface)
\brief	Disconnect from a peer device whose connection was handed over

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverDisconnectRequest(GapHandoverDisconnectRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x0A, (void*)req, sizeof(GapHandoverDisconnectRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverInitRequest(uint8_t fsciInterface)
\brief	Initialize Handover

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GapHandoverInitRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4B, 0x0B, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverTimeSyncTransmitRequest(GapHandoverTimeSyncTransmitRequest_t *req, uint8_t fsciInterface)
\brief	Start transmitting for time sync procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverTimeSyncTransmitRequest(GapHandoverTimeSyncTransmitRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x0C, (void*)req, sizeof(GapHandoverTimeSyncTransmitRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverTimeSyncReceiveRequest(GapHandoverTimeSyncReceiveRequest_t *req, uint8_t fsciInterface)
\brief	Start receiving for time sync procedure

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverTimeSyncReceiveRequest(GapHandoverTimeSyncReceiveRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x0D, (void*)req, sizeof(GapHandoverTimeSyncReceiveRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverAnchorSearchStopRequest(GapHandoverAnchorSearchStopRequest_t *req, uint8_t fsciInterface)
\brief	Stops anchor search for the given connection handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverAnchorSearchStopRequest(GapHandoverAnchorSearchStopRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x0E, (void*)req, sizeof(GapHandoverAnchorSearchStopRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverUpdateConnParamsRequest(GapHandoverUpdateConnParamsRequest_t *req, uint8_t fsciInterface)
\brief	Update the channel map and/or phy used in the current anchor monitoring process

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverUpdateConnParamsRequest(GapHandoverUpdateConnParamsRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4B, 0x11, (void*)req, sizeof(GapHandoverUpdateConnParamsRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GapHandoverSetLlPendingDataRequest(GapHandoverSetLlPendingDataRequest_t *req, uint8_t fsciInterface)
\brief	Sets the LL data pending on the source device to be transmitted after handover connect

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GapHandoverSetLlPendingDataRequest(GapHandoverSetLlPendingDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* ConnHandle */
	msgLen += sizeof(uint16_t);  /* DataSize */
	msgLen += req->DataSize;  /* Data */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->ConnHandle), sizeof(req->ConnHandle)); idx += sizeof(req->ConnHandle);
	FLib_MemCpy(pMsg + idx, &(req->DataSize), sizeof(req->DataSize)); idx += sizeof(req->DataSize);
	FLib_MemCpy(pMsg + idx, req->Data, req->DataSize); idx += req->DataSize;

	/* Send the request */
	FSCI_transmitPayload(0x4B, 0x12, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* GAPHANDOVER_ENABLE */

#if GAP2_ENABLE
/*!*************************************************************************************************
\fn		mem_status_t GAP2CtrlCmdGenericHciCmdRequest(GAP2CtrlCmdGenericHciCmdRequest_t *req, uint8_t fsciInterface)
\brief	 Generic wrapper function to send HCI commands

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2CtrlCmdGenericHciCmdRequest(GAP2CtrlCmdGenericHciCmdRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint16_t);  /* Opcode */
	msgLen += sizeof(uint8_t);  /* PayloadLength */
	msgLen += req->PayloadLength;  /* Payload */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	FLib_MemCpy(pMsg + idx, &(req->Opcode), sizeof(req->Opcode)); idx += sizeof(req->Opcode);
	pMsg[idx] = req->PayloadLength; idx++;
	FLib_MemCpy(pMsg + idx, req->Payload, req->PayloadLength); idx += req->PayloadLength;

	/* Send the request */
	FSCI_transmitPayload(0x4C, 0x00, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t Gap2CtrlCmdGetDebugInfoRequest(Gap2CtrlCmdGetDebugInfoRequest_t *req, uint8_t fsciInterface)
\brief	Read the debug information provided by the NBU

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t Gap2CtrlCmdGetDebugInfoRequest(Gap2CtrlCmdGetDebugInfoRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x01, (void*)req, sizeof(Gap2CtrlCmdGetDebugInfoRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2SetExtAdvertisingDecisionDataRequest(GAP2SetExtAdvertisingDecisionDataRequest_t *req, uint8_t fsciInterface)
\brief	Requests the controller to set the decision data for specified advertising handle

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2SetExtAdvertisingDecisionDataRequest(GAP2SetExtAdvertisingDecisionDataRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* Handle */
	msgLen += sizeof(bool_t);  /* DecisionKeyIncluded */

	if (req->DecisionKeyIncluded)
	{
		msgLen += 16;
	}  /* DecisionKey */

	msgLen += sizeof(bool_t);  /* RandomPartIncluded */


	if (req->RandomPartIncluded)
	{
		msgLen += 3;
	}  /* RandomPart */

	msgLen += sizeof(bool_t);  /* DecisionDataIncluded */

	if (req->DecisionDataIncluded)
	{
		msgLen += 1;
	}  /* dataLength */

	msgLen += req->dataLength;  /* decisionData */
	msgLen += sizeof(bool_t);  /* resolvableTagPresent */


	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Handle; idx++;
	pMsg[idx] = req->DecisionKeyIncluded; idx++;

	if (req->DecisionKeyIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->DecisionKey, 16); idx += 16;
	}


	if (req->DecisionKeyIncluded)
	{
		pMsg[idx] = req->RandomPartIncluded; idx++;
	}


	if (req->RandomPartIncluded)
	{
		FLib_MemCpy(pMsg + idx, req->RandomPart, sizeof(req->RandomPart)); idx += sizeof(req->RandomPart);
	}

	pMsg[idx] = req->DecisionDataIncluded; idx++;

	if (req->DecisionDataIncluded)
	{
		pMsg[idx] = req->dataLength; idx++;
	}

	FLib_MemCpy(pMsg + idx, req->decisionData, req->dataLength); idx += req->dataLength;

	if (req->DecisionDataIncluded)
	{
		pMsg[idx] = req->resolvableTagPresent; idx++;
	}


	/* Send the request */
	FSCI_transmitPayload(0x4C, 0x02, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2SetDecisionInstructionsRequest(GAP2SetDecisionInstructionsRequest_t *req, uint8_t fsciInterface)
\brief	Requests the controller to set the decision instructions

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2SetDecisionInstructionsRequest(GAP2SetDecisionInstructionsRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* numTests */

	for (uint32_t i = 0; i < req->numTests; i++)
	{
		msgLen += sizeof(uint8_t);  /* testGroup */
		msgLen += sizeof(uint8_t);  /* passCriteria */
		msgLen += sizeof(uint8_t);  /* relevantField */

		switch (req->DecisionInstructions[i].relevantField)
		{
			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ResolvableTag_c:
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_AdvMode_c:
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_RSSI_c:
				msgLen += sizeof(uint8_t);  /* RSSImin */
				msgLen += sizeof(uint8_t);  /* RSSImax */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_PathLoss_c:
				msgLen += sizeof(uint8_t);  /* PathLossmin */
				msgLen += sizeof(uint8_t);  /* PathLossmax */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_AdvAddress_c:
				msgLen += sizeof(uint8_t);  /* AdvAcheck */

				switch (req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.AdvAcheck)
				{
					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAinFilterAcceptList_c:
						break;

					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAmatchAddress1_c:
						msgLen += sizeof(uint8_t);  /* address1Type */
						msgLen += 6;  /* address1 */
						break;

					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAmatchAddress1orAddress2_c:
						msgLen += sizeof(uint8_t);  /* address1Type */
						msgLen += 6;  /* address1 */
						msgLen += sizeof(uint8_t);  /* address2Type */
						msgLen += 6;  /* address2 */
						break;
				}				/* advAddress */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_1Byte_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_2Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_3Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_4Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_5Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_6Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_7Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_8Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_1Byte_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_2Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_3Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_4Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_5Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_6Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_7Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_8Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_1Byte_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_2Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_3Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_4Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_5Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_6Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_7Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_8Bytes_c:
				msgLen += 8;  /* arbitraryDataMask */
				msgLen += 8;  /* arbitraryDataTarget */
				break;
		}		/* testParameters */
	}	/* DecisionInstructions */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->numTests; idx++;

	for (uint32_t i = 0; i < req->numTests; i++)
	{
		pMsg[idx] = req->DecisionInstructions[i].testGroup; idx++;
		pMsg[idx] = req->DecisionInstructions[i].passCriteria; idx++;
		pMsg[idx] = req->DecisionInstructions[i].relevantField; idx++;

		switch (req->DecisionInstructions[i].relevantField)
		{
			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ResolvableTag_c:
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_AdvMode_c:
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_RSSI_c:
				pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_RSSI_c.RSSImin; idx++;
				pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_RSSI_c.RSSImax; idx++;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_PathLoss_c:
				pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_PathLoss_c.PathLossmin; idx++;
				pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_PathLoss_c.PathLossmax; idx++;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_AdvAddress_c:
				pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.AdvAcheck; idx++;

				switch (req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.AdvAcheck)
				{
					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAinFilterAcceptList_c:
						break;

					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAmatchAddress1_c:
						pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1_c.address1Type; idx++;
						FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1_c.address1, 6); idx += 6;
						break;

					case GAP2SetDecisionInstructionsRequest_DecisionInstructions_testParameters_gDIRF_AdvAddress_c_AdvAcheck_gDIAAC_AdvAmatchAddress1orAddress2_c:
						pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1orAddress2_c.address1Type; idx++;
						FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1orAddress2_c.address1, 6); idx += 6;
						pMsg[idx] = req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1orAddress2_c.address2Type; idx++;
						FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_AdvAddress_c.advAddress.gDIAAC_AdvAmatchAddress1orAddress2_c.address2, 6); idx += 6;
						break;
				}
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_1Byte_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_1Byte_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_1Byte_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_2Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_2Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_2Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_3Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_3Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_3Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_4Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_4Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_4Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_5Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_5Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_5Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_6Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_6Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_6Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_7Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_7Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_7Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfExactly_8Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_8Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfExactly_8Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_1Byte_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_1Byte_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_1Byte_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_2Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_2Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_2Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_3Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_3Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_3Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_4Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_4Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_4Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_5Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_5Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_5Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_6Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_6Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_6Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_7Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_7Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_7Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtLeast_8Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_8Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtLeast_8Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_1Byte_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_1Byte_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_1Byte_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_2Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_2Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_2Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_3Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_3Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_3Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_4Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_4Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_4Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_5Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_5Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_5Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_6Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_6Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_6Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_7Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_7Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_7Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;

			case GAP2SetDecisionInstructionsRequest_DecisionInstructions_relevantField_gDIRF_ArbitraryDataOfAtMost_8Bytes_c:
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_8Bytes_c.arbitraryDataMask, 8); idx += 8;
				FLib_MemCpy(pMsg + idx, req->DecisionInstructions[i].testParameters.gDIRF_ArbitraryDataOfAtMost_8Bytes_c.arbitraryDataTarget, 8); idx += 8;
				break;
		}
	}

	/* Send the request */
	FSCI_transmitPayload(0x4C, 0x03, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2SetExtAdvertisingParametersV2Request(GAP2SetExtAdvertisingParametersV2Request_t *req, uint8_t fsciInterface)
\brief	Sets the advertising parameters with coding selection

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2SetExtAdvertisingParametersV2Request(GAP2SetExtAdvertisingParametersV2Request_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x04, (void*)req, sizeof(GAP2SetExtAdvertisingParametersV2Request_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t Gap2CtrlCmdGetDebugInfo2Request(Gap2CtrlCmdGetDebugInfo2Request_t *req, uint8_t fsciInterface)
\brief	Read the second debug information provided by the NBU

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t Gap2CtrlCmdGetDebugInfo2Request(Gap2CtrlCmdGetDebugInfo2Request_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x05, (void*)req, sizeof(Gap2CtrlCmdGetDebugInfo2Request_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2EcdhP256ComputeA2BKeyRequest(GAP2EcdhP256ComputeA2BKeyRequest_t *req, uint8_t fsciInterface)
\brief	Enable or disable Sec Lib

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2EcdhP256ComputeA2BKeyRequest(GAP2EcdhP256ComputeA2BKeyRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x06, (void*)req, sizeof(GAP2EcdhP256ComputeA2BKeyRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2EcdhP256FreeA2BKeyDataRequest(GAP2EcdhP256FreeA2BKeyDataRequest_t *req, uint8_t fsciInterface)
\brief	Free the A2B key allocated memory

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2EcdhP256FreeA2BKeyDataRequest(GAP2EcdhP256FreeA2BKeyDataRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x07, (void*)req, sizeof(GAP2EcdhP256FreeA2BKeyDataRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2ExportA2BBlobRequest(GAP2ExportA2BBlobRequest_t *req, uint8_t fsciInterface)
\brief	Obtain a A2B encrypted key blob for the given key data

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2ExportA2BBlobRequest(GAP2ExportA2BBlobRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* KeyType */

	switch (req->KeyType)
	{
		case GAP2ExportA2BBlobRequest_KeyType_gSecPlainText_c:
			msgLen += 16;  /* gSecPlainText_c */
			break;

		case GAP2ExportA2BBlobRequest_KeyType_gSecElkeBlob_c:
			msgLen += 40;  /* gSecElkeBlob_c */
			break;

		case GAP2ExportA2BBlobRequest_KeyType_gSecLtkElkeBlob_c:
			msgLen += 40;  /* gSecLtkElkeBlob_c */
			break;
	}  /* Key */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->KeyType; idx++;

	switch (req->KeyType)
	{
		case GAP2ExportA2BBlobRequest_KeyType_gSecPlainText_c:
			FLib_MemCpy(pMsg + idx, req->Key.gSecPlainText_c, 16); idx += 16;
			break;

		case GAP2ExportA2BBlobRequest_KeyType_gSecElkeBlob_c:
			FLib_MemCpy(pMsg + idx, req->Key.gSecElkeBlob_c, 40); idx += 40;
			break;

		case GAP2ExportA2BBlobRequest_KeyType_gSecLtkElkeBlob_c:
			FLib_MemCpy(pMsg + idx, req->Key.gSecLtkElkeBlob_c, 40); idx += 40;
			break;
	}

	/* Send the request */
	FSCI_transmitPayload(0x4C, 0x08, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2ImportA2BBlobRequest(GAP2ImportA2BBlobRequest_t *req, uint8_t fsciInterface)
\brief	Obtain key information based on a A2B key blob

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2ImportA2BBlobRequest(GAP2ImportA2BBlobRequest_t *req, uint8_t fsciInterface)
{
	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	FSCI_transmitPayload(0x4C, 0x09, (void*)req, sizeof(GAP2ImportA2BBlobRequest_t), fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2EcdhP256GenerateKeysRequest(uint8_t fsciInterface)
\brief	Generate a public/private key pair and return the public key data

\return	mem_status_t			kStatus_MemSuccess
***************************************************************************************************/
mem_status_t GAP2EcdhP256GenerateKeysRequest(uint8_t fsciInterface)
{
	FSCI_transmitPayload(0x4C, 0x0A, NULL, 0, fsciInterface);
	return kStatus_MemSuccess;
}

/*!*************************************************************************************************
\fn		mem_status_t GAP2LeChannelOverrideRequest(GAP2LeChannelOverrideRequest_t *req, uint8_t fsciInterface)
\brief	Set channels for adv/scan/init

\return	mem_status_t			kStatus_MemSuccess, kStatus_MemAllocError, kStatus_MemFreeError
							kStatus_MemUnknownError if req is NULL
***************************************************************************************************/
mem_status_t GAP2LeChannelOverrideRequest(GAP2LeChannelOverrideRequest_t *req, uint8_t fsciInterface)
{
	uint8_t *pMsg = NULL;
	uint16_t msgLen = 0, idx = 0;

	/* Sanity check */
	if (!req)
	{
		return kStatus_MemUnknownError;
	}

	/* Compute the size of the request */
	msgLen += sizeof(uint8_t);  /* Mode */
	msgLen += sizeof(uint8_t);  /* channelListLength */
	msgLen += req->channelListLength;  /* channelList */

	/* Allocate memory for the marshalled payload */
	pMsg = MEM_BufferAlloc(msgLen);
	if (!pMsg)
	{
		return kStatus_MemAllocError;
	}

	/* Serialize */
	pMsg[idx] = req->Mode; idx++;
	pMsg[idx] = req->channelListLength; idx++;
	FLib_MemCpy(pMsg + idx, req->channelList, req->channelListLength); idx += req->channelListLength;

	/* Send the request */
	FSCI_transmitPayload(0x4C, 0x0B, pMsg, msgLen, fsciInterface);
	MEM_BufferFree(pMsg);
	return kStatus_MemSuccess;
}

#endif  /* GAP2_ENABLE */
