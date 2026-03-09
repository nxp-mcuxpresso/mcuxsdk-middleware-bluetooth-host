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
#include "host_app.h"
#include "fsci_ble.h"
#include "app_localization.h"
#include "app_localization_algo.h"
#include "loc_reader_host.h"
#include "rpmsg_config.h"
#include "fwk_platform_lcl.h"

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
static void App_FsciBleAppCbHandler
(
    void*       pData,
    void*       param,
    uint32_t    fsciInterface
);

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
*\fn        bleResult_t App_HostAppFsciInit(void)
*
*\brief     Host Shell initialization function.
*
*\retval    gBleAlreadyInitialized_c    Module already initialized
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_HostAppFsciInit(void)
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
        initialized = TRUE;

        /* Initialize FSCI for core 0 communication */
        status = FSCI_RegisterOpGroup(gFsciNcpAppOpcodeGroup_c,
                                      gFsciMonitorMode_c,
                                      App_FsciBleAppCbHandler, NULL, mFsciInterfaceId);

        if (status != gFsciSuccess_c)
        {
            result = gBleUnexpectedError_c;
        }
    }

    return result;
}

/************************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
************************************************************************************/

static void App_FsciBleAppCbHandler
(
    void*       pData,
    void*       param,
    uint32_t    fsciInterface
)
{
    clientPacket_t* pClientPacket   = (clientPacket_t*)pData;
    uint8_t*        pBuffer         = &pClientPacket->structured.payload[0];

    switch (pClientPacket->structured.header.opCode)
    {
        case gAppSetAlgoTypeIDOpCode_c:
        {
            uint8_t algoNumber;
            fsciBleGetUint8ValueFromBuffer(algoNumber, pBuffer);
            AppLocalization_SetAlgorithm(algoNumber);
        }
        break;

        case gAppBleCSDataOpCode_c:
        {
            ncpIqTransferMsgType_t eventType;
            uint32_t dataSize = gFsciMaxPayloadLen_c;
            uint8_t *pRecvData = MEM_BufferAlloc(dataSize);

            if (pRecvData != NULL)
            {
                /* Extract event type */
                fsciBleGetEnumValueFromBuffer(eventType, pBuffer, ncpIqTransferMsgType_t);
                fsciBleGetArrayFromBuffer(pRecvData, pBuffer, (dataSize - sizeof(ncpIqTransferMsgType_t)));
                BleApp_HandleNcpMsg(eventType, pRecvData);
                (void)MEM_BufferFree(pRecvData);
            }
        }
        break;

        case gAppSendRangeSettingsOpCode_c:
        {
            deviceId_t deviceId;

            /* Copy the received range settings for the given peer */
            fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);

            fsciBleGetUint8ValueFromBuffer(mGlobalRangeSettings.role, pBuffer);

            /* Do not overwrite CS Algo buffer */
            fsciBleGetArrayFromBuffer(&mRangeSettings[deviceId], pBuffer, sizeof(appLocalization_rangeCfg_t) - sizeof(void *));
        }
        break;

        case gAppSendDisconnectNotifOpCode_c:
        {
            deviceId_t deviceId;

            /* Copy the received range settings for the given peer */
            fsciBleGetDeviceIdFromBuffer(&deviceId, &pBuffer);
            AppLocalizationAlgo_ResetPeer(deviceId);
        }
        break;

        default:
        {
            ; /* skip unknown event */
        }
        break;
    }

    (void)MEM_BufferFree(pData);
}
