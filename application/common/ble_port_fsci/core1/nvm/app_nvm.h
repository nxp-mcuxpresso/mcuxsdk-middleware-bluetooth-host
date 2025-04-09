#include "ble_general.h"

/*! *********************************************************************************
*\fn        bleResult_t App_NcpNvmInit(void)
*\brief     NCP NVM initialization function.
*
*\retval    gBleAlreadyInitialized_c    NCP NVM already initialized
*\retval    gBleOsError_c               Fail to create OSA event
*\retval    gBleUnexpectedError_c       Error occured while sending a request to the host
*\retval    gBleSuccess_c               Operation successfull
********************************************************************************** */
bleResult_t App_NcpNvmInit(void);

/*! *********************************************************************************
*\fn        void App_NvmIdle(void)
*\brief     Send BLE NVM requests to Host to save modified data sets.
*
*\retval    void
********************************************************************************** */
void App_NvmIdle(void);

/*! *********************************************************************************
*\fn            void App_HandleBleNvmEvent(uint16_t size, uint8_t *pData)
*\brief         Handle raw NVM commands received from Host on ISR.
*
*\param[in]     size    Host NVM notification size.
*\param[in]     pData   Host NVM notification to be processed.
*
*\retval    void
********************************************************************************** */
void App_HandleBleNvmEvent(uint16_t size, uint8_t *pData);