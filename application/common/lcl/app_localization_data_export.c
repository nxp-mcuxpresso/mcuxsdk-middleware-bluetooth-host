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
#include "app_localization_data_export.h"
#include "fsl_component_mem_manager.h"
#include "fwk_platform_ble.h"

#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 0)
/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/
/* Used by char conversion routines */
const static char hexchar[] = "0123456789ABCDEF";
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
const static char basechar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
#endif

static uint32_t WrsCRC32 = 0xFFFFFFFFU;
static bool FlagCRCData = FALSE;

const static uint32_t crc32_table[128] =
   {0x7F1C607BU, 0x77073096U, 0xEE0E612CU, 0x990951BAU, 0x076DC419U, 0x706AF48FU, 0xc4f4e977U, 0x556755d2U,
    0xE963A535U, 0x9E6495A3U, 0x0EDB8832U, 0x79DCB8A4U, 0xE0D5E91EU, 0x97D2D988U, 0xa1d38a68U, 0x03b2c522U,
    0x09B64C2BU, 0x7EB17CBDU, 0xE7B82D07U, 0x90BF1D91U, 0x1DB71064U, 0x6AB020F2U, 0xd68ddc0fU, 0xf4077f02U,
    0xF3B97148U, 0x84BE41DEU, 0x1ADAD47DU, 0x6DDDE4EBU, 0xF4D4B551U, 0x83D385C7U, 0x19fa9c3fU, 0x3c3a60f3U,
    0x136C9856U, 0x646BA8C0U, 0xFD62F97AU, 0x8A65C9ECU, 0x14015C4FU, 0x63066CD9U, 0x38a7d00cU, 0xc98fcf5aU,
    0xFA0F3D63U, 0x8D080DF5U, 0x3B6E20C8U, 0x4C69105EU, 0xD56041E4U, 0xA2677172U, 0x2f7dc879U, 0xa0e7c527U,
    0x3C03E4D1U, 0x4B04D447U, 0xD20D85FDU, 0xA50AB56BU, 0x35B5A8FAU, 0x42B2986CU, 0x7f40c232U, 0x7b92ad99U,
    0xDBBBC9D6U, 0xACBCF940U, 0x32D86CE3U, 0x45DF5C75U, 0xDCD60DCFU, 0xABD13D59U, 0xdab5f1f1U, 0x530c7f63U,
    0x26D930ACU, 0x51DE003AU, 0xC8D75180U, 0xBFD06116U, 0x21B4F4B5U, 0x56B3C423U, 0x8cf2ab15U, 0x9e9a17b0U,
    0xCFBA9599U, 0xB8BDA50FU, 0x2802B89EU, 0x5F058808U, 0xC60CD9B2U, 0xB10BE924U, 0x6b515118U, 0xc6e8d3f3U,
    0x2F6F7C87U, 0x58684C11U, 0xC1611DABU, 0xB6662D3DU, 0x76DC4190U, 0x01DB7106U, 0xc17ecbb8U, 0x64a39a0cU,
    0x98D220BCU, 0xEFD5102AU, 0x71B18589U, 0x06B6B51FU, 0x9FBFE4A5U, 0xE8B8D433U, 0xcb61dd84U, 0x5ea83c5bU,
    0x7807C9A2U, 0x0F00F934U, 0x9609A88EU, 0xE10E9818U, 0x7F6A0DBBU, 0x086D3D2DU, 0x5f4a7a0dU, 0x4c9a4a16U,
    0x91646C97U, 0xE6635C01U, 0x6B6B51F4U, 0x1C6C6162U, 0x856530D8U, 0x4F6A0D6BU, 0x460ce7fdU, 0x94d0c2abU,
    0xa4c52f20U, 0x8fe19d75U, 0x6b19278dU, 0x14d77aa7U, 0xf0e83a51U, 0x24f965efU, 0x1c610e61U, 0x17a495ccU,
    0x62f20859U, 0x0b4789dbU, 0x3d3ebd17U, 0x080db3a8U, 0x90b294c7U, 0xb136eae1U, 0xc77ef2c2U, 0x8bc2dc2dU};

/* calculates the CRC-8 checksum of print buffer data */
static uint32_t CRC32(uint32_t WrsDataCRC32, const unsigned char *buf, size_t  size)
{
    uint8_t byte;
    for (size_t i = 0; i < size; i++) {
        byte = buf[i];
        WrsDataCRC32 = (WrsDataCRC32 >> 8) ^ crc32_table[((WrsDataCRC32 & 0xFFU) / 2U) ^ byte];
    }
    return WrsDataCRC32;
}

/*! *********************************************************************************
* \brief        Translate given data to hex or apply given translation function.
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
* \param[in]    convert     Translate function to apply to data
********************************************************************************** */
static void cli_sprint_hex8b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint8_t units,
    uint32_t (* convert)(uint8_t **dataIn)
)
{
    uint32_t u;
    uint8_t val;
    if (dataOut == NULL)
    {
        return;
    }
    for (u = 0; u < units; u++)
    {
        if (convert != NULL)
        {
            val = (uint8_t)convert(&dataIn);
        }
        else
        {
            val = *dataIn;
            dataIn++;
        }
        *dataOut++ = (uint8_t)hexchar[val>>4U];
        *dataOut++ = (uint8_t)hexchar[val&0xFU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Translate given data to hex.
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
* \param[in]    to_4bits   Translation function
********************************************************************************** */
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
static void cli_sprint_hex4b(uint8_t *dataOut,
                      uint8_t *dataIn,
                      uint8_t units,
                      uint32_t (* to_4bits)(uint8_t **dataIn))
{
    uint32_t u;
    if (dataOut == NULL)
    {
        return;
    }
    for (u = 0; u < units; u++)
    {
        uint32_t val;

        if (to_4bits == NULL)
        {
            val = (uint32_t)(*dataIn) & 0xFU;
            /* Default implementation assumes dataIn as uint8_t array */
            dataIn++;
        }
        else
        {
            /* dataIn managed by translation fuinction */
            val = to_4bits(&dataIn);
        }
        *dataOut++ = (uint8_t)hexchar[val&0xFU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Print 12 bits out of 3 bytes (CS IQ) shifted by 'shift',
*               every 'incr' bytes - 'units' times
*
* \param[in]    dataOut    Pointer to destination buffer
* \param[in]    dataIn     Pointer to source buffer
* \param[in]    units      Data length
* \param[in]    incr       Increment value
* \param[in]    shift      Shift value
********************************************************************************** */
static void cli_sprint_base64_12b_c
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint8_t units,
    uint8_t incr,
    uint8_t shift
)
{
    uint32_t u;
    uint32_t val = 0U;
    if (dataOut == NULL)
    {
        return;
    }
    for (u = 0; u < units; u++)
    {
        FLib_MemCpy(&val, dataIn, 3);
        val >>= shift;
        val &= 0xfffU; /* keep 12bits */
        val ^= 0x800U; /* invert sign bit (offset by 2048) for easier encoding/decoding */
        dataIn += incr;
        /* base64 encoding */
        *dataOut++ = (uint8_t)basechar[(val>>6)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val)&0x3FU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Print bytes in hex every 'incr' bytes - 'units' times.
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
********************************************************************************** */
static void cli_sprint_hex8b_c(uint8_t *dataOut, uint8_t *dataIn, uint8_t units, uint32_t incr)
{
    uint32_t u;
    if (dataOut == NULL)
    {
        return;
    }
    for (u = 0; u < units; u++)
    {
        *dataOut++ = (uint8_t)hexchar[(*dataIn)>>4U];
        *dataOut++ = (uint8_t)hexchar[(*dataIn)&0xFU];
        dataIn += incr;
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        ToF - print node decision
*
* \param[in]    fp      Estimated average distance [m]
* \param[in]    sr      Success rate [%]
********************************************************************************** */
static void tof_print_node_decision(int32_t fp, uint8_t sr)
{
    uint32_t sign;
    uint16_t qInt, qFrac;

    if (fp > 0)
    {
      sign = 1;
    }
    else
    {
      sign = 0;
      fp = -fp;
    }

    /* Round to nearest integer */
    fp += 3277;
    qInt = (uint16_t)((uint32_t)fp >> 16U);

    qFrac = (uint16_t)((((uint32_t)fp & 0x0000FFFFU)*10U) >> 16U);

    (void)printf("result:{ad:%s%d.%u,sr:%u},", (sign > 0U)?"":"-", qInt, qFrac, sr);
}

/*! *********************************************************************************
* \brief        Print 24 bits in 4 bytes base64 format - 'units' times
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer (32 bits MSB first)
* \param[in]    units       Data length to translate
* \param[in]    to_24bits   Translation function
********************************************************************************** */
static void cli_sprint_base64_24b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint16_t units,
    uint32_t (* to_24bits)(uint8_t **dataIn)
)
{
    uint32_t u;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = dataIn;
    
    if (dataOut == NULL)
    {
        return;
    }
    for (u = 0; u < units; u++)
    {
        uint32_t val;

        if (to_24bits == NULL)
        {
            /* Default implementation assumes dataIn as uint32_t array */
            val = (*(tmp.v32)) & 0xFFFFFFU;
            tmp.v8 += sizeof(uint32_t);
        }
        else
        {
            val = to_24bits(&tmp.v8);
        }
        *dataOut++ = (uint8_t)basechar[(val>>18)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val>>12)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val>>6)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val)&0x3FU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Convert RTT record from 4 bytes record to 3 bytes value
*               Packet quality is encoded in 4bits MSB
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
static uint32_t convert_rtt_to_24bits(uint8_t **dataIn) 
{
    uint32_t val;
    uint8_t *ptr;

    /* Skip NADM & RSSI */
    (*dataIn) += gCsNadmSize_c + gCsRssiSize_c;
    /* dataIn not aligned on uint32_t boundary, cannot cast */
    ptr = *dataIn;
    val = ((uint32_t)ptr[2])<<16 | ((uint32_t)ptr[1])<<8 | (uint32_t)ptr[0];
    *dataIn += gCsTsSize_c;
    return val;
}

/*! *********************************************************************************
* \brief        RSSI is signed, swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
static uint32_t convert_mode0rssi(uint8_t **dataIn) {
    uint8_t val;

    /* Skip NADM */
    val = (**dataIn) ^ 0x80U;
    (*dataIn)++;
    return val;
}

/*! *********************************************************************************
* \brief        RSSI is signed, swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
static uint32_t convert_rssi(uint8_t **dataIn) {
    uint8_t val;

    /* Skip NADM */
    (*dataIn) += gCsNadmSize_c;
    val = (**dataIn) ^ 0x80U;
    (*dataIn) += gCsRssiSize_c + gCsTsSize_c;  /* Skip RSSI & TS */
    return val;
}

/*! *********************************************************************************
* \brief        Reference power level is signed, swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
static uint32_t convert_refPow(uint8_t **refPow) {
    uint8_t val;

    val = (**refPow) ^ 0x80U;
    (*refPow) += 1;  
    return val;
}

/*! *********************************************************************************
* \brief        Convert NADM record from 1 byte record to 4 bits value
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
static uint32_t convert_nadm_to_4bits(uint8_t **dataIn) {
    uint32_t val;
    uint8_t *ptr;

    ptr = *dataIn;
    /* Compress data from 8 bits to 4 bits. First 4 MSB bits are not used (Unknown NADM 0xFF value will be compressed to 0x0F) */
    val = (uint32_t)(*ptr) & 0x0FU;
    *dataIn += gCsNadmSize_c + gCsRssiSize_c + gCsTsSize_c;
    return val;
}
#endif

static void app_print_cs_measurement(isp_meas_response_t *meas_response, appLocalization_rangeCfg_t *ranging_cfg)
{
    uint8_t *pBuffer;

    (void)printf("cs:{");

    (void)printf("cfg:{");
    uint8_t csRole = mGlobalRangeSettings.role == gCsRoleInitiator_c ? 0U : 1U;
    (void)printf("role:%u,rtyp:%u,rphy:%u, txpwr_delta:%d,", csRole, meas_response->cs_data->rtt_type, meas_response->cs_data->phy, ranging_cfg->txPwrDelta);
    (void)printf(  "fcs:%u,"
                "ip1:%u,"
                "ip2:%u,"
                "tpm:%u,"
                "tsw:%u,"
                "ant:%u",\
                meas_response->cs_data->t_fcs,\
                meas_response->cs_data->t_ip1,\
                meas_response->cs_data->t_ip2,\
                meas_response->cs_data->t_pm,\
                meas_response->cs_data->t_sw,\
                ranging_cfg->ant_cfg_index);
    (void)printf("},");

    /* CS Steps */
    (void)printf("stp:{nb:%u,", meas_response->cs_data->step_nb);

#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1)
    /* Modes */
    (void)printf("md:");
    pBuffer = MEM_BufferAlloc((uint32_t)meas_response->cs_data->step_nb + 4U);
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'");
    }
    else
    {
        cli_sprint_hex4b(pBuffer, meas_response->cs_data->modeMap, (uint8_t)meas_response->cs_data->step_nb, NULL);
        (void)printf("'%s'", (char *)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }

    (void)printf(",");
    /* Remote Modes */
    (void)printf("mdr:");
    pBuffer = MEM_BufferAlloc((uint32_t)meas_response->cs_data->subevtStopIdxRemote[meas_response->cs_data->subevt_nb - 1U] + 4U);
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'");
    }
    else
    {
        cli_sprint_hex4b(pBuffer, meas_response->cs_data->modeMapRemote, (uint8_t)meas_response->cs_data->subevtStopIdxRemote[meas_response->cs_data->subevt_nb - 1U], NULL);
        (void)printf("'%s'", (char *)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }

    (void)printf(",");
    /* Channels */
    (void)printf("ch:");
    pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->step_nb + 4U);
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'");
    } 
    else
    {
        cli_sprint_hex8b(pBuffer, meas_response->cs_data->channelMap, (uint8_t)meas_response->cs_data->step_nb, NULL);
        (void)printf("'%s'", (char*)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }

    (void)printf(",");
#endif
    
    /* CS proc startAclCnt */
    (void)printf("acl:%u,", meas_response->cs_data->startAclCnt);
    
    /* Connection interval */
    (void)printf("ci:%u,", meas_response->cs_data->conn_interval);
    
    /* Subevent interval */
    (void)printf("si:%u,", meas_response->cs_data->subevt_interval);
    
    /* Subevt number */
    (void)printf("senb:%u,", meas_response->cs_data->subevt_nb);
    
    /* Main mode repeat */
    (void)printf("mmdrp:%u,", meas_response->cs_data->main_mode_repeat);
    
    /* Main mode type */
    (void)printf("mmdt:%u,", meas_response->cs_data->main_mode_type);
    
    /* Sub-mode type */
    (void)printf("smdt:%u,", meas_response->cs_data->sub_mode_type);
    
    /* Event counter for each subevent */
    (void)printf("evt:");
    pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'");
    } 
    else
    {
        cli_sprint_hex8b(pBuffer, meas_response->cs_data->subevtConnEvent, meas_response->cs_data->subevt_nb, NULL);
        (void)printf("'%s'", (char*)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }

    (void)printf("}");
    (void)printf("},");
}
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
static void app_mciq_print_node_data(mciq_data_t *mciq_data)
{
    uint8_t m, n_ap;
    mciq_data_t *data = mciq_data;
    uint8_t *pBuffer = MEM_BufferAlloc(2U * (uint32_t)data->nbSteps + 4U);

    if (pBuffer == NULL)
    {
        return;
    }

    n_ap = data->n_ap;
    (void)printf("i:[");
    for (m = 0; m < n_ap; m++)
    {
        /* CS IQs are 2 * 12 bits compressed, I is located in 12bits MSB, all antenna paths packed by step */
        cli_sprint_base64_12b_c(pBuffer, &data->iq[(gCsIqSize_c+gCsTqiSize_c)*m], (uint8_t)data->nbSteps, (gCsIqSize_c+gCsTqiSize_c)*n_ap, 12);
        (void)printf("'%s',", (char*)pBuffer);
    }
    (void)printf("],q:[");
    for (m = 0; m < n_ap; m++)
    {
        /* CS IQs are 2 * 12 bits, Q is located in 12bits LSB, all antenna paths packed by step */
        cli_sprint_base64_12b_c(pBuffer, &data->iq[(gCsIqSize_c+gCsTqiSize_c)*m], (uint8_t)data->nbSteps, (gCsIqSize_c+gCsTqiSize_c)*n_ap, 0);
        (void)printf("'%s',", (char*)pBuffer);
    }
    (void)printf("],tqi:["); /* Tone quality indicator, byte */
    for (m = 0; m < n_ap; m++)
    {
        cli_sprint_hex8b_c(pBuffer, &data->iq[(gCsIqSize_c+gCsTqiSize_c)*m+gCsIqSize_c], (uint8_t)data->nbSteps, (gCsIqSize_c+gCsTqiSize_c) * (uint32_t)n_ap);
        (void)printf("'%s',", (char*)pBuffer);
    }
    (void)MEM_BufferFree(pBuffer);
    (void)printf("],");
}
#endif

static void app_mciq_measurement_print(isp_meas_response_t *meas_response, engine_response_t *engine_response, appLocalization_rangeCfg_t *ranging_cfg)
{
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
    mciq_data_t *mdata;
#endif
    uint32_t nb_steps, n_ap;

    mciq_data_t *mciq_data_init = &meas_response->mciq_data[0];
    mciq_data_t *mciq_data_refl = &meas_response->mciq_data[1];

    /* In test mode, one of reflector or intiator nbSteps is null, keep the relevant one */
    nb_steps = (mciq_data_init->nbSteps != 0U) ? mciq_data_init->nbSteps : mciq_data_refl->nbSteps;
    n_ap = (mciq_data_init->n_ap != 0U) ? mciq_data_init->n_ap : mciq_data_refl->n_ap;
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
    mdata = mciq_data_init;
#endif
    (void)printf("mciq:{cfg:{n_ap:%u,n_stp:%u},", n_ap, nb_steps);

    if (engine_response->is_valid)
    {
        (void)printf("result:{vf:%u", engine_response->mciq_result.nb_valid_freq);
#if (defined(gAppUseRADEAlgorithm_d) && (gAppUseRADEAlgorithm_d == 1))
        int integ, decimal; /* compute printf float manually to avoid linking "large" printf lib */
        float integFl;

        integ = (int)engine_response->mciq_result.rade_dist;
        integFl = (engine_response->mciq_result.rade_dist - (float)integ) * 100.0f;
        decimal = (int)integFl;
        if (engine_response->mciq_result.rade_dist < 0.0f)
        {
          decimal *= (-1);
        }
        (void)printf(",rade:%d.%02d", integ, decimal);

        integ = (int)engine_response->mciq_result.rade_dist_trk;
        integFl = (engine_response->mciq_result.rade_dist_trk - (float)integ) * 100.0f;
        decimal = (int)integFl;
        if (engine_response->mciq_result.rade_dist_trk < 0.0f)
        {
          decimal *= (-1);
        }
        (void)printf(",rade_trk:%d.%02d", integ, decimal);

        integ = (int)engine_response->mciq_result.rade_dqi;
        integFl = (engine_response->mciq_result.rade_dqi - (float)integ) * 100.0f;
        decimal = (int)integFl;
        if (engine_response->mciq_result.rade_dqi < 0.0f)
        {
          decimal *= (-1);
        }
        (void)printf(",rade_dqi:%d.%02d", integ, decimal);
        (void)printf(",raderr:%d", engine_response->mciq_result.rade_error_flag);
#endif
        (void)printf("},");
    }

#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1) 
    if (mdata->nbSteps != 0U)
    {
        (void)printf("init:{"); 
        app_mciq_print_node_data(mdata);
        
        /* Reference power level */
        (void)printf("rpl:");
        uint8_t *pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b(pBuffer, (uint8_t *)meas_response->cs_data->subevtRefPowerLevelInit, meas_response->cs_data->subevt_nb, convert_refPow);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }
        
        /* Subevent done status */
        (void)printf("sts:");
        uint8_t *subevtDoneStatus_init = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtDoneStatusLocal : meas_response->cs_data->subevtDoneStatusRemote;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtDoneStatus_init, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }
        
        /* Subevent abort reason */
        (void)printf("abt:");
        uint8_t *subevtAbortReason_init = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtAbortReasonLocal : meas_response->cs_data->subevtAbortReasonRemote;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtAbortReason_init, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }

        /* subevt */
        (void)printf("se:");
        uint8_t *subevtStopIdx_init = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtStopIdxLocal : meas_response->cs_data->subevtStopIdxRemote;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtStopIdx_init, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }
        
        (void)printf("},");
    }

    mdata = mciq_data_refl;
    if (mdata->nbSteps != 0U)
    {
        (void)printf("refl:{");
        app_mciq_print_node_data(mdata);
        
        (void)printf("rpl:");
        uint8_t *pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b(pBuffer, (uint8_t *)meas_response->cs_data->subevtRefPowerLevelRefl, meas_response->cs_data->subevt_nb, convert_refPow);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }
        
        (void)printf("sts:");
        uint8_t *subevtDoneStatus_refl = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtDoneStatusRemote : meas_response->cs_data->subevtDoneStatusLocal;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtDoneStatus_refl, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }

        (void)printf("abt:");
        uint8_t *subevtAbortReason_refl = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtAbortReasonRemote : meas_response->cs_data->subevtAbortReasonLocal;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtAbortReason_refl, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }

        /* subevt */
        (void)printf("se:");
        uint8_t *subevtStopIdx_refl = mGlobalRangeSettings.role == gCsRoleInitiator_c ? meas_response->cs_data->subevtStopIdxRemote : meas_response->cs_data->subevtStopIdxLocal;
        pBuffer = MEM_BufferAlloc(2U * (uint32_t)meas_response->cs_data->subevt_nb + 4U);
        if (pBuffer == NULL)
        {
            (void)printf("'NA:oom'");
        }
        else
        {
            cli_sprint_hex8b_c(pBuffer, (uint8_t *)subevtStopIdx_refl, (uint8_t)meas_response->cs_data->subevt_nb, 1U);
            (void)printf("'%s',", (char*)pBuffer);
            (void)MEM_BufferFree(pBuffer);
        }
        
        (void)printf("},");
    }
    
#endif

    (void)printf("},");
}

#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1)
static void app_tof_print_node_data(tof_data_t *data)
{
    uint8_t *pBuffer = MEM_BufferAlloc(4U * (uint32_t)data->nbSteps + 1U);
    if (pBuffer == NULL)
    {
        return;
    }
    /* RTT are 18 bits + 4 error bits (3 bytes), resulting in 4 base64-encoded-bytes */
    cli_sprint_base64_24b(pBuffer, (uint8_t *)data->ts, data->nbSteps, convert_rtt_to_24bits);
    (void)printf("d:'%s',", pBuffer);
    (void)MEM_BufferFree(pBuffer);

    /* RTT RSSI */
    (void)printf("r:");
    pBuffer = MEM_BufferAlloc(2U * (uint32_t)data->nbSteps + 4U);
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'");
    }
    else
    {
        cli_sprint_hex8b(pBuffer, (uint8_t *)data->ts, (uint8_t)data->nbSteps, convert_rssi);
        (void)printf("'%s',", (char*)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }

    /* Packet NADM */
    (void)printf("nadm:");
    pBuffer = MEM_BufferAlloc((uint32_t)data->nbSteps + 4U); /* One byte (4bits) per NADM info */
    if (pBuffer == NULL)
    {
        (void)printf("'NA:oom'"); /* report out of memory */
    }
    else
    {
        cli_sprint_hex4b(pBuffer, (uint8_t *)data->ts, (uint8_t)data->nbSteps, convert_nadm_to_4bits);
        (void)printf("'%s'", (char*)pBuffer);
        (void)MEM_BufferFree(pBuffer);
    }
}

static void app_tof_measurement_print(isp_meas_response_t *meas_response, engine_response_t *engine_response, appLocalization_rangeCfg_t *ranging_cfg)
{
    tof_data_t *tdata;
    uint32_t nb_steps;

    tof_data_t *tof_data_init = &meas_response->tof_data[0];
    tof_data_t *tof_data_refl = &meas_response->tof_data[1];

    /* In test mode, one of reflector or intiator nbSteps is null, keep the relevant one */
    nb_steps = (tof_data_init->nbSteps != 0U) ? tof_data_init->nbSteps:tof_data_refl->nbSteps;

    tdata = tof_data_init;
    (void)printf("tof:{cfg:{n_stp:%d},", nb_steps);

    if (engine_response->is_valid)
    {
        tof_print_node_decision(engine_response->tof_result.dm_ad, engine_response->tof_result.dm_sr);
    }

    if (tdata->nbSteps != 0U)
    {
        (void)printf("init:{");
        app_tof_print_node_data(tdata);
        (void)printf("},");
    }

    tdata = tof_data_refl;
    if (tdata->nbSteps != 0U)
    {
        (void)printf("refl:{");
        app_tof_print_node_data(tdata);
        (void)printf("},");
    }
    (void)printf("},");
}

static void mode0_print_node_data(cs_data_t *cs_data, mode0_data_t *mode0Data)
{
    uint8_t *pBuffer;
    int8_t mode0Rssi[gMaxNumCsStepsMode0_c];
    for (uint32_t i = 0; i < cs_data->mode0_nb; i++)
    {
        mode0Rssi[i] = mode0Data->rssi;
    }
    /* Mode0 RSSI */
    (void)printf("r:");
    pBuffer = MEM_BufferAlloc(cs_data->mode0_nb * sizeof(mode0_data_t) + 4);
    if (pBuffer == NULL) {
        (void)printf("'NA:oom'");
    } else {
        cli_sprint_hex8b(pBuffer, (uint8_t *)mode0Rssi, cs_data->mode0_nb, convert_mode0rssi);
        (void)printf("'%s'", (char*)pBuffer);
        MEM_BufferFree(pBuffer);
    }
    (void)printf(",");
}

static void app_mode0_measurement_print(cs_data_t *cs_data)
{
    mode0_data_t *data;
    uint32_t nb_steps = cs_data->mode0_nb;

    (void)printf("md0:{cfg:{n_stp:%d},", nb_steps);

    (void)printf("init:{");
    data = mGlobalRangeSettings.role == gCsRoleInitiator_c ? cs_data->mode0Data : &(cs_data->mode0Data[gMaxNumCsStepsMode0_c]);
    if(data != NULL) {
        mode0_print_node_data(cs_data, data);
    }

    (void)printf("},refl:{");
    data = mGlobalRangeSettings.role == gCsRoleInitiator_c ? &(cs_data->mode0Data[gMaxNumCsStepsMode0_c]) : cs_data->mode0Data;
    if(data != NULL) {
        mode0_print_node_data(cs_data, data);
    }
    (void)printf("}},");
}

#endif

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*************Code to support toolchain's printf, scanf *******************************/
/* These function is used to support ARM_GCC, KDS, Atollic toolchains to printf */
#if (defined(__GNUC__)) || defined(__IAR_SYSTEMS_ICC__)

#if (defined(__GNUC__))
size_t _write(int handle, const unsigned char *buffer, size_t size)
#elif defined(__IAR_SYSTEMS_ICC__)
size_t __write(int handle, const unsigned char *buffer, size_t size)
#endif
{
    if (buffer == NULL)
    {
        /*
         * This means that we should flush internal buffers.  Since we don't we just return.
         * (Remember, "handle" == -1 means that all handles should be flushed.)
         */
#if (defined(__GNUC__))
        return ((size_t)-1);
#else
        return 0;
#endif
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2))
    {
#if (defined(__GNUC__))
        return ((size_t)-1);
#else
        return 0;
#endif
    }

    if (FlagCRCData == TRUE)
    {
      /* calculates the CRC-8 checksum of print buffer data */
      WrsCRC32 = CRC32(WrsCRC32, buffer, size);
    }

    /* Send data. */
    shell_data_export_writeN((const char *)buffer, (uint32_t)size);

    return size;
}

/* These function fputc and fgetc is used to support KEIL toolchain to printf and scanf*/
#else
#error "Must be compiled with NEWLIB libraries"
#endif

void app_print_cs_data(isp_meas_response_t *meas_response, engine_response_t *engine_response, appLocalization_rangeCfg_t *ranging_cfg)
{
    uint64_t now_time = PLATFORM_GetTimeStamp();
    FlagCRCData = TRUE;
    WrsCRC32 = 0xFFFFFFFFU;
    /* Flush the output buffer to avoid the CRC is calculated on the previous output */
#if (defined(__GNUC__))
    fflush(stdout);
#endif
    (void)printf("items:[");

    {
        (void)printf("{");
        
        app_print_cs_measurement(meas_response, ranging_cfg);
        app_mode0_measurement_print(meas_response->cs_data);
        if (meas_response->mciq_data[0].iq != NULL)
        {
            app_mciq_measurement_print(meas_response, engine_response, ranging_cfg);
        }
#if defined(gAppLocDataExport_d) && (gAppLocDataExport_d > 1)
        if (meas_response->tof_data[0].ts != NULL)
        {
            app_tof_measurement_print(meas_response, engine_response, ranging_cfg);
        }
#endif
        (void)printf("PrintTimeUs:%u", (uint32_t)(PLATFORM_GetTimeStamp() - now_time));

        (void)printf("},");
    }
    (void)printf("]");

    /* Flush the output buffer to ensure all data is involved in the CRC calculation */
#if (defined(__GNUC__))
    fflush(stdout);
#endif
    FlagCRCData = FALSE;
    (void)printf("\nCRC32:%08x", WrsCRC32);
    (void)printf("\nmarker:[DONE]\n");
}
#endif
