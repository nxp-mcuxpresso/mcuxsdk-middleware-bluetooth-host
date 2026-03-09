/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* NXP Proprietary
*
* This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or
* by downloading, installing, activating and/or otherwise using the software, you are
* agreeing that you have read, and that you agree to comply with and are bound by,
* such license terms. If you do not agree to be bound by the applicable license terms,
* then you may not retain, install, activate or otherwise use the software.
********************************************************************************** */
/************************************************************************************
 *************************************************************************************
 * Include
 *************************************************************************************
 ************************************************************************************/
#include "ble_port_fsci.h"
#include "FsciInterface.h"
#include "ncp_app.h"
#include "fsci_ble.h"
#include "app_localization.h"
#include "app_localization_algo.h"
#include "rpmsg_config.h"
#include "ble_port_fsci_op.h"

/*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/
/* Helper function to fragment local and remote CS data and send it over FSCI to core 0 */
static void sendMeasurementResultHelper
(
    deviceId_t deviceId,
    void* pData
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/

/*! *********************************************************************************
*\fn        bleResult_t App_NcpAppFsciInit(void)
*
*\brief     Host Shell initialization function.
*
*\retval    gBleAlreadyInitialized_c    Module already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_NcpAppFsciInit(void)
{
    static bool_t initialized = FALSE;
    bleResult_t result = gBleSuccess_c;

    if (initialized == TRUE)
    {
        result = gBleAlreadyInitialized_c;
    }
    else
    {
        gFsciStatus_t status = gFsciSuccess_c;

        /* Initialize FSCI for core 0 communication */
        status = FSCI_RegisterOpGroup(gFsciNcpAppOpcodeGroup_c,
                                      gFsciMonitorMode_c,
                                      NULL, NULL, mFsciInterfaceId);

        if (status != gFsciSuccess_c)
        {
            result = gBleUnexpectedError_c;
        }
    }

    return result;
}

/*! *********************************************************************************
 * \brief  Algo-related cleanup. Executes on core 0.
 *
 * \param[in]    deviceId    Device ID.
 *
 * \return       none
********************************************************************************** */
void AppLocalizationAlgo_ResetPeer(deviceId_t deviceId)
{
    uint8_t* pBuff = MEM_BufferAlloc(sizeof(deviceId_t));

    if (pBuff != NULL)
    {
        pBuff[0] = deviceId;

        /* Send message */
        FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppSendDisconnectNotifOpCode_c,
                             pBuff, sizeof(deviceId_t), gFsciInterface_c);

        (void)MEM_BufferFree(pBuff);
    }  
}

/*! *********************************************************************************
 * \brief  Send local and remote localization data to Core 0 applications.
 *
 * \param[in]    pData       Pointer to the localization data
 * \param[in]    isLocal     TRUE if this is local data, FALSE for remote data
 *
 * \return       none
********************************************************************************** */
void AppLocalizationAlgo_RunMeasurement
(
    deviceId_t deviceId,
    rasMeasurementData_t *pLocalData,
    rasMeasurementData_t *pPeerData,
    uint8_t role,
    localizationAlgoResult_t *pResult
)
{
    (void)pResult;
    (void)role;

    appCsRes_t res;
    uint8_t* pBuff = MEM_BufferAlloc(sizeof(appLocalization_rangeCfg_t) + sizeof(deviceId_t) + sizeof(uint8_t));

    if (pBuff != NULL)
    {
        /* Send mRangeSettings for algorithm timings */
        pBuff[0] = deviceId;
        pBuff[1] = mGlobalRangeSettings.role;
        FLib_MemCpy(pBuff + 1 + 1, &mRangeSettings[deviceId], sizeof(appLocalization_rangeCfg_t));
        /* Send message */
        FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppSendRangeSettingsOpCode_c,
                             pBuff, (sizeof(appLocalization_rangeCfg_t) + sizeof(deviceId_t) + sizeof(uint8_t)), gFsciInterface_c);

        (void)MEM_BufferFree(pBuff);

        /* Send local data */
        res.isLocal = TRUE;
        res.pData = pLocalData;
        sendMeasurementResultHelper(deviceId, &res);

        /* Send remote data */
        res.isLocal = FALSE;
        res.pData = pPeerData;
        pPeerData->dataIndex = pPeerData->totalSentRcvDataIndex;
        sendMeasurementResultHelper(deviceId, &res);
    }
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
 * \brief  Send local and remote localization data to Core 0 applications.
 *
 * \param[in]    pData       Pointer to the localization data
 * \param[in]    isLocal     TRUE if this is local data, FALSE for remote data
 *
 * \return       none
********************************************************************************** */
static void sendMeasurementResultHelper
(
    deviceId_t deviceId,
    void* pData
)
{
   appCsRes_t *pRes = (appCsRes_t*)pData;
   uint32_t dataSize = (uint32_t)gFsciMaxPayloadLen_c - 1U;
   uint32_t dataLen = 0U;
   uint16_t dataHeaderSize = (sizeof(rasMeasurementData_t) - sizeof(uint8_t*));
   /* Maximum FSCI payload size - leave out one octet for the checksum */
   ncpIqTransferMsgType_t eventType;
   uint8_t* pBuff = NULL;

   /* Fragment data according to RPMSG buffer size */
   uint8_t* pEventData = MEM_BufferAlloc(gFsciMaxPayloadLen_c);

   /* Send message to core 0 */
   if (pEventData != NULL)
   {
       uint8_t algoValue = AppLocalization_GetAlgorithm();
       /* Send message */
       FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppSetAlgoTypeIDOpCode_c,
                                 &algoValue, sizeof(uint8_t), gFsciInterface_c);
        
       while (dataLen < sizeof(csAppData_t))
       {
          /* Reset buffer */
          FLib_MemSet(pEventData, 0U, gFsciMaxPayloadLen_c);
          dataSize = (uint32_t)gFsciMaxPayloadLen_c - 1U;
          pBuff = pEventData;

          if (dataLen == 0U)
          {
              /* This is the first packet - send as much data as the RPMSG buffer can fit */
              pRes->isLocal == TRUE ? (eventType = gIQLocalTrStart_c) : (eventType = gIQRemoteTrStart_c);
              /* Set event tpe */
              pBuff[0] = (uint8_t)eventType;
              pBuff++;
              pBuff[0] = deviceId;
              pBuff++;
              dataSize -= 2U;
              /* Copy rasMeasurementData_t header data */
              FLib_MemCpy(pBuff, pRes->pData, dataHeaderSize);
              dataSize -= dataHeaderSize;
              /* Copy CS Result Data */
              FLib_MemCpy(pBuff + dataHeaderSize, pRes->pData->pData, dataSize);
              /* set total data size and send message */
              dataLen += dataSize;

              /* Send message */
              FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppBleCSDataOpCode_c,
                                   pEventData, gFsciMaxPayloadLen_c, gFsciInterface_c);
          }
          else if ((sizeof(csAppData_t) - dataLen) < (uint32_t)gFsciMaxPayloadLen_c)
          {
              /* This is the last packet - send the remaing data */
              pRes->isLocal == TRUE ? (eventType = gIQLocalTrEnd_c) : (eventType = gIQRemoteTrEnd_c);
              /* Set event tpe */
              pBuff[0] = (uint8_t)eventType;
              pBuff++;
              pBuff[0] = deviceId;
              pBuff++;

              /* Fill the remaining valid data */
              FLib_MemCpy(pBuff, pRes->pData->pData + dataLen, (sizeof(csAppData_t) - dataLen));
              pBuff += (sizeof(csAppData_t) - dataLen);

              /* Fill the remaining data */
              uint32_t remainingDataLen =   (uint32_t)gFsciMaxPayloadLen_c - sizeof(eventType) -1U - (sizeof(csAppData_t) - dataLen);
              FLib_MemSet(pBuff, 0U, remainingDataLen);

              dataLen += ((uint16_t)sizeof(csAppData_t) - dataLen);

              /* Send message */
              FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppBleCSDataOpCode_c,
                                   pEventData, gFsciMaxPayloadLen_c, gFsciInterface_c);
          }
          else
          {
              /* This is an intermediary packet - send as much data as the RPMSG buffer can fit */
              pRes->isLocal == TRUE ? (eventType = gIQLocalTrCont_c) : (eventType = gIQRemoteTrCont_c);
              /* Set event tpe */
              pBuff[0] = (uint8_t)eventType;
              pBuff++;
              pBuff[0] = deviceId;
              pBuff++;
              dataSize -= 2U;
              /* Fill data and set message len */
              FLib_MemCpy(pBuff, pRes->pData->pData + dataLen, dataSize);
              dataLen += dataSize;

              /* Send message */
              FSCI_transmitPayload(gFsciNcpAppOpcodeGroup_c, gAppBleCSDataOpCode_c,
                                   pEventData, gFsciMaxPayloadLen_c, gFsciInterface_c);
          }
       }

       (void)MEM_BufferFree(pEventData);
   }
}
