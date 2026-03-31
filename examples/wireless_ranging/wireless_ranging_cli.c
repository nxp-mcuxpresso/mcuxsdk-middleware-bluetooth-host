/*
 * Copyright 2020-2026 NXP
 *
 * NXP Proprietary
 *
 * This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or
 * by downloading, installing, activating and/or otherwise using the software, you are
 * agreeing that you have read, and that you agree to comply with and are bound by,
 * such license terms. If you do not agree to be bound by the applicable license terms,
 * then you may not retain, install, activate or otherwise use the software.
 */

/************************************************************************************
 *************************************************************************************
 *  Includes
 *************************************************************************************
 ************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wformat"
#endif

#ifdef INCLUDE_EMBEDDED_COMPUTATION_SUPPORT
#include "dm_phasebased.h"
#define FLOAT_32_T_DEFINED /* Avoids conflicts with nxp2p4_xcvr.h */
#define FLOAT_64_T_DEFINED /* Avoids conflicts with nxp2p4_xcvr.h */
#endif

#include "fsl_port.h"
#include "fsl_component_button.h"
#include "fsl_component_serial_manager.h"
#include "fsl_component_mem_manager.h"
#include "fsl_adapter_reset.h"
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
#include "HWParameter.h"
#endif
#include "app.h"

#include "board.h"
#include "nxp2p4_xcvr.h"
#include "fwk_platform.h"
#include "fwk_platform_ics.h"
#include "fwk_platform_lcl.h"

#include "wireless_ranging_cli.h"
#include "wireless_ranging.h"
#include "wireless_ranging_service.h"
#include "isp_interface.h"
#if (defined(KW37A4_SERIES) || defined(KW37Z4_SERIES) || defined(KW38A4_SERIES) || defined(KW38Z4_SERIES) || defined(KW39A4_SERIES))
#include "fsl_rcm.h"
#elif (defined(KW45B41Z83_SERIES) || defined(K32W232H_SERIES) || defined(K32W1480_SERIES) || \
       defined(MCXW716A_SERIES) || defined(MCXW716C_SERIES) || defined(KW47B42ZB7_cm33_core0_SERIES) || defined(MCXW727C_cm33_core0_SERIES) || defined(KW47_core0_SERIES) || \
       defined(KW43B43ZC7_SERIES) || defined(MCXW70AC_SERIES))
#include "fsl_cmc.h"
#else
/*to support other platform*/
#endif
#include "wireless_ranging_service.h"
#include "isp_debug_helpers.h"
#include "isp_ranging_engine.h"
#include "isp_measurement.h"
#include "app_version.h"
#include "cmdparse.h"

/************************************************************************************
 *************************************************************************************
 *  Macros
 *************************************************************************************
 ************************************************************************************/

#define IQ_SIZE_MEM (4U) /* Size in bytes for an IQ record as produced by XCVR */

#define HEX_TO_INT(byte) { \
    if (byte >= 48U && byte <= 57U) { byte = byte - 48U; }\
        else if (byte >= 97U && byte <= 102U) { byte = byte - 97U + 10U; } \
        else if (byte >= 65U && byte <=70U) { byte = byte - 65U + 10U; } \
        else { /* No action needed */ } }
/* Changed chars to their ASCII value for MISRA 10.2 & 10.3 & 10.4 */

/************************************************************************************
 *************************************************************************************
 *  Public Prototypes
 *************************************************************************************
 ************************************************************************************/
uint64_t parse_hex64(char *str);
void cli_uart_rx_cb
(
    void *callbackParam,
    serial_manager_callback_message_t *message,
    serial_manager_status_t status
);
void cli_cmd_range(int argc, char *argv[]);
void cli_cmd_test(int argc, char *argv[]);
void cli_cmd_setup(int argc, char *argv[]);

void cli_cmd_misc_xtal_trim(int argc, char *argv[]);
void cli_cmd_misc_cw_trim(int argc, char *argv[]);
#ifdef INCLUDE_EMBEDDED_COMPUTATION_SUPPORT
void cli_cmd_misc_rtp_algo(int argc, char *argv[]);
void cli_cmd_misc_rtp_algo_config(int argc, char *argv[]);
#endif

void cli_cmd_communication_role(int argc, char *argv[]);
void cli_cmd_communication_interval(int argc, char *argv[]);
void cli_cmd_communication_local(int argc, char *argv[]);
void cli_cmd_communication_remote(int argc, char *argv[]);
void cli_cmd_communication_connected(int argc, char *argv[]);
void cli_cmd_parameter_ch_list_gen(int argc, char *argv[]);
void cli_cmd_parameter_ch_list_set(int argc, char *argv[]);
void cli_cmd_parameter_ch_map(int argc, char *argv[]);
void cli_cmd_parameter_cs_algo(int argc, char *argv[]);
void cli_cmd_parameter_main_mode_nb(int argc, char *argv[]);
void cli_cmd_parameter_mode0_nb(int argc, char *argv[]);
void cli_cmd_parameter_mode_type(int argc, char *argv[]);
void cli_cmd_parameter_repeat(int argc, char *argv[]);
void cli_cmd_parameter_subevent_len(int argc, char *argv[]);
void cli_cmd_parameter_role(int argc, char *argv[]);
void cli_cmd_parameter_rtt_phy(int argc, char *argv[]);
void cli_cmd_parameter_rtt_type(int argc, char *argv[]);
void cli_cmd_parameter_timings_intervals(int argc, char *argv[]);
void cli_cmd_parameter_timings_tpm(int argc, char *argv[]);
void cli_cmd_parameter_timings_tsw(int argc, char *argv[]);
void cli_cmd_parameter_ant_cfg(int argc, char *argv[]);
void cli_cmd_parameter_ant_type(int argc, char *argv[]);
void cli_cmd_parameter_tx_pwr(int argc, char *argv[]);
void cli_cmd_parameter_debug(int argc, char *argv[]);
void cli_cmd_parameter_pn_seq(int argc, char *argv[]);
void cli_cmd_parameter_tone_ext(int argc, char *argv[]);

void cli_cmd_system_reset(int argc, char *argv[]);
void cli_cmd_system_factory(int argc, char *argv[]);
void cli_cmd_system_store(int argc, char *argv[]);
void cli_cmd_system_debug(int argc, char *argv[]);
void cli_cmd_system_verbosity(int argc, char *argv[]);
void cli_cmd_system_unique_id(int argc, char *argv[]);
void cli_cmd_system_version(int argc, char *argv[]);
void cli_cmd_system_build_info(int argc, char *argv[]);
void cli_cmd_system_events(int argc, char *argv[]);
void cli_cmd_system_timing(int argc, char *argv[]);
void cli_cmd_system_baudrate(int argc, char *argv[]);
void cli_cmd_system_output_format(int argc, char *argv[]);
void cli_cmd_system_calibrate(int argc, char *argv[]);

void cli_sprint_hex8b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint32_t units,
    uint32_t (* convert)(uint8_t **dataIn)
);
void cli_sprint_hex8b_reverse(uint8_t *dataOut, uint8_t *dataIn, uint32_t units);
bool_t cli_generate_ch_list(void);

void wr_xtal_trim_cw(bool_t sense);
static void cli_mciq_measurement_print
(
    measurement_parameters_t *mparams,
    mciq_data_t *mciq_data_init,
    mciq_data_t *mciq_data_refl,
    engine_response_t *engine_response
);
static void cli_tof_measurement_print
(
    measurement_parameters_t *mparams,
    tof_data_t *tof_data_init,
    tof_data_t *tof_data_refl,
    engine_response_t *engine_response
);

/************************************************************************************
 *************************************************************************************
 *  Public memory declarations
 *************************************************************************************
 ************************************************************************************/

/* Used by char conversion routines */
const char hexchar[] = "0123456789ABCDEF";
const char basechar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*isp cli serial manager handle*/
serial_handle_t  gAppSerMgrIf;
/*isp cli serial manager read handle*/
static SERIAL_MANAGER_READ_HANDLE_DEFINE(s_ispCliReadHandle);
/*isp cli serial manager write handle*/
SERIAL_MANAGER_WRITE_HANDLE_DEFINE(g_ispCliWriteHandle);

board_info_t local_bid;
const uint32_t crc32_table[128] =
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
uint32_t WrsCRC32 = 0xFFFFFFFFU;
bool FlagCRCData = 0;

const command_t sub_misc_tbl[] = {
    {
        "xtal_trim [<0...63>] | Set/show XTAL trim value",
        {cli_cmd_misc_xtal_trim},
        eFlag1Args | eFlagRead
    },
    {
        "cw_trim [0|1] | Start/Stop 2440MHz constant wave Tx (used for XTAL trimming)",
        {cli_cmd_misc_cw_trim},
        (int)eFlag1Args
    },
#ifdef INCLUDE_EMBEDDED_COMPUTATION_SUPPORT
    {
      "rtp_algo [<0...7>] | Set/show RTP embedded algorithm bitmap to be run (bit0:CDE, bit2:RADE)",
        {cli_cmd_misc_rtp_algo},
        eFlag1Args | eFlagRead
    },
    {
        "rtp_algo_config [<1(CDE)> [<param1> <param2>]] | Set/show RTP embedded algorithm configuration",
        {cli_cmd_misc_rtp_algo_config},
        eFlag3Args | eFlagRead
    },
#endif
    {0, {0}, 0} /* last entry must be all zeros */
};

const command_t sub_timings_tbl[] = {
    {
        "intervals <150|120|100|80|60|50|40> <145|80|60|50|40|30> [ <145|80|60|50|40|30> ] | Set/show CS interval timings T_FCS, T_IP1, T_IP2 (us)",
        {cli_cmd_parameter_timings_intervals},
        eFlag1Args | eFlagRead
    },
    {
        "t_pm <10|20|40> | Set/show CS T_PM timing (us)",
        {cli_cmd_parameter_timings_tpm},
        eFlag1Args | eFlagRead
    },
    {
        "t_sw <2|4|10> | Set/show CS T_SW timing (us)",
        {cli_cmd_parameter_timings_tsw},
        eFlag1Args | eFlagRead
    },
    {0, {0}, 0} /* last entry must be all zeros */
    };

const command_t sub_ch_list_tbl[] = {
    {
        "set [<0...78>,..,<0...78> [<repeat>]] | Set/show channel list",
        {cli_cmd_parameter_ch_list_set},
        eFlag1Args | eFlagRead
    },
    {
        "generate <0...78> <0...78> [rand] | Generate channel list from first and last CS channels",
        {cli_cmd_parameter_ch_list_gen},
        (int)eFlag1Args
    },
    {0, {0}, 0} /* last entry must be all zeros */
    };

const command_t sub_parameter_tbl[] = {
    {
        "ch_list | Channel list commands (test mode only)",
        {.child = sub_ch_list_tbl},
        (int)eFlagCommandList
    },
    {
        "ch_map [<bitmask> [<repeat>]] | Set/show channel map as 10 hexa octets (79 bits used)",
        {cli_cmd_parameter_ch_map},
        eFlag1Args | eFlagRead
    },
    {
        "cs_algo [<0|1> [<0|1>] [<2...8>]] | Set/show channel selection algo type (0=3b, 1=3c), and for #3c: shape (0=hat, 1=X) and jump",
        {cli_cmd_parameter_cs_algo},
        eFlag1Args | eFlagRead
    },
    {
        /* max and repetition numbers are not used for CS Test command */
        "main_mode_nb [<1...160> <1...160> <0...3>] | Set/show numbers of CS main mode steps (min, max, repetition)",
        {cli_cmd_parameter_main_mode_nb},
        eFlag1Args | eFlagRead
    },
    {
        "mode0_nb [<1...3>] | Set/show number of CS mode0 steps",
        {cli_cmd_parameter_mode0_nb},
        eFlag1Args | eFlagRead
    },
    {
        "mode_type [<1...3> [<1...3>]] | Set/show main mode and sub-mode types",
        {cli_cmd_parameter_mode_type},
        eFlag1Args | eFlagRead
    },
    {
        "repeat [<1..65535>] [<0..65535>] | Set/show number of procedure repetitions and procedure interval",
        {cli_cmd_parameter_repeat},
        eFlag1Args | eFlagRead | eFlagHidden
    },
    {
        "subevent_len [<1..4000000>] | Set/show subevent lenght [us]",
        {cli_cmd_parameter_subevent_len},
        eFlag1Args | eFlagRead | eFlagHidden
    },
    {
        "role [<initiator,reflector>] | Set/show CS role",
        {cli_cmd_parameter_role},
        eFlag1Args | eFlagRead
    },
    {
        "rtt_phy [<0|1>] | Set/show RTT phy rate (0=1Mbps, 1=2Mbps, 2=BT2.0)",
        {cli_cmd_parameter_rtt_phy},
        eFlag1Args | eFlagRead
    },
    {
        "rtt_type [<0|6>] | Set/show RTT type",
        {cli_cmd_parameter_rtt_type},
        eFlag1Args | eFlagRead
    },
    {
        "tx_pwr [<-12...MAX>] | Set/show TX power in dBm of CS measurement. Range is -12dBm to max chip capability",
        {cli_cmd_parameter_tx_pwr},
        eFlag1Args | eFlagRead
    },
    {
        "debug [<0-255>] | Set/show debug options (REPORT_DBG_INFO<2>, IQ_AVERAGE_DISABLE<1>, IQ_DETAILS<0>)",
        {cli_cmd_parameter_debug},
        eFlag1Args | eFlagRead
    },
    {
        "ant_cfg [<0-7> [<0...23|255>]] | Set/show antenna config index and (test mode only) permutation index",
        {cli_cmd_parameter_ant_cfg},
        eFlag1Args | eFlagRead
    },
    {
      "ant_type [<0-5> ] | Set/show antenna board type: 0:none, 1:X-FR-ANTDIV SMA, 2:X-FR-ANTDIV printed, 3:X-FR-ANTDIV (4 antenna), 4: LOC (2 antenna)",
        {cli_cmd_parameter_ant_type},
        eFlag1Args | eFlagRead
    },
    {
        "pn_seq [<AA initiator> [<AA reflector>]] | Set/show PN sequences (must be a hex number, test mode only)",
        {cli_cmd_parameter_pn_seq},
        eFlag1Args | eFlagRead
    },
    {
        "tone_ext [<0...4>] | Set/show TP tone extension (test mode only)",
        {cli_cmd_parameter_tone_ext},
        eFlag1Args | eFlagRead
    },
    {
        "timings | Timings commands",
        {.child = sub_timings_tbl},
        (int)eFlagCommandList
    },
    {0, {0}, 0} /* last entry must be all zeros */
};

const command_t sub_communication_tbl[] = {
    {
        "connected | Show connection status",
        {cli_cmd_communication_connected},
        (int)eFlagRead
    },
    {
        "interval [<#slots>] | Set/show BLE connection interval duration. Unit 1.25ms",
        {cli_cmd_communication_interval},
        eFlag1Args | eFlagRead | eFlagHidden
    },
    {
        "local | Get local BLE address",
        {cli_cmd_communication_local},
        (int)eFlagRead
    },
    {
        "remote [<ble-address>]* | Set/show allowed remote ble-address list (0x0 allows any address; must be a hexadecimal number)",
        {cli_cmd_communication_remote},
        eFlag1Args | eFlagRead
    },
    {
        "role [<central,peripheral,none>] | Set/show communication role",
        {cli_cmd_communication_role},
        eFlag1Args | eFlagRead
    },
    {0, {0}, 0} /* last entry must be all zeros */
    };

const command_t sub_system_tbl[] = {
    {
        "build_info | Show SW build information",
        {cli_cmd_system_build_info},
        (int)eFlagArgsOpt
    },
    {
        "debug [0|1] | Enable debug features (DTEST...)",
        {cli_cmd_system_debug},
        eFlag1Args | eFlagRead
    },
    {
        "verbosity [<0...255>] | Set/show verbosity flags (1=Info, 2=Debug, 4=MeasurementInfo, 8=MeasurementData, 16=MeasurementDebug, 32=BoardInfo, 64=Profiling)",
        {cli_cmd_system_verbosity},
        eFlag1Args | eFlagRead
    },
    {
        "unique_id | Show Unique Id",
        {cli_cmd_system_unique_id},
        (int)eFlagRead
    },
    {
        "version | Show SW version number",
        {cli_cmd_system_version},
        (int)eFlagArgsOpt
    },
    {
        "reset | Reset MCU",
        {cli_cmd_system_reset},
        (int)eFlagArgsOpt
    },
    {
        "store | Store all parameters",
        {cli_cmd_system_store},
        (int)eFlagArgsOpt
    },
    {
        "factory | Reload and store factory defaults",
        {cli_cmd_system_factory},
        (int)eFlagArgsOpt
    },
    {
        "events | Show events",
        {cli_cmd_system_events},
        (int)eFlagArgsOpt
    },
    {
        "timing | Show BLE timing",
        {cli_cmd_system_timing},
        (int)eFlagArgsOpt
    },
    {
        "baudrate <value> | Set/show baudrate of serial interface (default: 115200, volatile)",
        {cli_cmd_system_baudrate},
        eFlag1Args | eFlagRead
    },
    {
        "output_format | Show output format",
        {cli_cmd_system_output_format},
        (int)eFlagRead
    },
    {
        "calibrate [<MCIQ dist> <ToF dist> [store]] | Set/Show zero-distance calibration, distances are in meters, Q10",
        {cli_cmd_system_calibrate},
        (int)eFlag3Args
    },
    {0, {0}, 0} /* last entry must be all zeros */
    };

const command_t menu_tbl[] = {
    {
        "range [<ble-address>] | Run CS procedure measurement with given peer",
        {cli_cmd_range},
        (int)eFlagArgsOpt
    },
    {
        "test [<tx,rx>] | Run CS test mode measurement",
        {cli_cmd_test},
        (int)eFlagArgsOpt
    },
    {
        "setup [<ble-address>] | Run CS setup (from Central only)",
        {cli_cmd_setup},
        (int)eFlagArgsOpt
    },
    {
        "communication [options] | Communication commands",
        {.child = sub_communication_tbl},
        (int)eFlagCommandList
    },
    {
        "parameter [options] | Parameter commands",
        {.child = sub_parameter_tbl},
        (int)eFlagCommandList
    },
    {
        "system [options] | System commands",
        {.child = sub_system_tbl},
        (int)eFlagCommandList
    },
    {
        "misc [options] | Miscellaneous commands",
        {.child = sub_misc_tbl},
        (int)eFlagCommandList
    },
    {0, {0}, 0} /* last entry must be all zeros */
};

/************************************************************************************
 *************************************************************************************
 *  Public functions
 *************************************************************************************
 ************************************************************************************/

/*************Code to support toolchain's printf, scanf *******************************/
/* These function __write and __read is used to support ARM_GCC, KDS, Atollic toolchains to printf and scanf*/
#if (defined(__GNUC__)) || defined(__IAR_SYSTEMS_ICC__)

#if(defined(__GNUC__))
size_t _write(int handle, const unsigned char *buffer, size_t size)
#elif defined(__IAR_SYSTEMS_ICC__)
size_t __write(int handle, const unsigned char *buffer, size_t size)
#endif
{
    if (buffer == 0)
    {
        /*
         * This means that we should flush internal buffers.  Since we don't we just return.
         * (Remember, "handle" == -1 means that all handles should be flushed.)
         */
#if(defined(__GNUC__))
        return ((size_t)-1);
#else
        return 0;
#endif
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2))
    {
        return ((size_t)-1);
    }
    if(FlagCRCData == 1)
    {
      /* calculates the CRC-8 checksum of print buffer data */
      WrsCRC32 = CRC32(WrsCRC32, buffer, size);
    }
    /* Send data. */
    SerialManager_WriteBlocking((serial_write_handle_t)g_ispCliWriteHandle, (uint8_t *)buffer, (uint32_t)size);

    return size;
}

#if(defined(__GNUC__))
size_t _read(int handle, unsigned char *buffer, size_t size)
#elif defined(__IAR_SYSTEMS_ICC__)
size_t __read(int handle, unsigned char *buffer, size_t size)
#endif
{
    uint16_t l = 0U;

    /* This function only reads from "standard in", for all other file  handles it returns failure. */
    if (handle != 0)
    {
        return ((size_t)-1);
    }

    do {
        (void)SerialManager_TryRead(s_ispCliReadHandle, buffer, size, (uint32_t *)&l);
    } while(l == 0U);

    return l;
}

/* These function fputc and fgetc is used to support KEIL toolchain to printf and scanf*/
#else
#error "Must be compiled with NEWLIB libraries"
#endif

/*! *********************************************************************************
* \brief        Parse hex string
*
* \param[in]    str          String to parse
********************************************************************************** */
uint64_t parse_hex64(char *str)
{
    uint64_t val;
    char *hex;
    val = 0;
    if(str[0] != '0' || str[1] != 'x')
    {
        return val;
    }
    hex = (char*)&str[2];
    while ((uint8_t)*hex !=  0U) {
        /* get current character then increment */
        uint8_t byte = (uint8_t)(*hex++);
        /* transform hex character to the 4bit equivalent number, using the ascii table indexes */
        HEX_TO_INT(byte);
        /* shift 4 to make space for new digit, and add the 4 bits of the new digit */
        val = (val << 4U) | (byte & 0xFU);
    }
    return val;
}

/*! *********************************************************************************
* \brief        Parse hex string - little endian
*               Number of produced byte is strlen(str)/2
*
* \param[in]    str          String to parse
* \param[in]    out          Resulting string
********************************************************************************** */
void vparse_hex64_little_endian(char *str, uint8_t *out)
{
    uint32_t len = strlen(str);
    out += len / 2U - 1U;
    while (((uint8_t)*str != 0U) && ((uint8_t)*(str+1) != 0U))
    {
        /* get current character then increment */
        uint8_t msb = (uint8_t)(*str++);
        uint8_t lsb = (uint8_t)(*str++);
        /* transform hex character to the 4bit equivalent number, using the ascii table indexes */
        HEX_TO_INT(msb);
        HEX_TO_INT(lsb);
        *out = (msb << 4U) | (lsb & 0xFU);
        out--;
    }
}

/*! *********************************************************************************
* \brief        Initialize CLI
********************************************************************************** */
void isp_cli_powerup(void)
{
    serial_manager_status_t ret;

    /*BOARD_InitSerialManager((serial_handle_t)gSerMgrIf); is called in APP_InitHardware()*/
    gAppSerMgrIf = gSerMgrIf;

    /*open low power write handle*/
    ret = SerialManager_OpenWriteHandle((serial_handle_t)gAppSerMgrIf, (serial_write_handle_t)g_ispCliWriteHandle);
    assert(kStatus_SerialManager_Success == ret);

    ret = SerialManager_OpenReadHandle((serial_handle_t)gAppSerMgrIf, (serial_read_handle_t)s_ispCliReadHandle);
    assert(kStatus_SerialManager_Success == ret);
    ret = SerialManager_InstallRxCallback((serial_read_handle_t)s_ispCliReadHandle, cli_uart_rx_cb, NULL);
    assert(kStatus_SerialManager_Success == ret);
    /* remove warning in release build */
    (void)ret;

    /* Check if parameter seem valid -> load defaults if not */
    if (gRangeSettings->ch_nb == 0xFFU)
    {
        cli_cmd_system_factory(0, 0);
    }

    CLI_PRINTF("\n\nNXP Wireless Ranging Application\n\n");
    cli_cmd_system_build_info(0, NULL);

#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
#if (defined(KW45B41Z83_SERIES) || defined(K32W232H_SERIES) || defined(K32W1480_SERIES) || defined(MCXW716A_SERIES) || defined(MCXW716C_SERIES) || defined(KW47B42ZB7_cm33_core0_SERIES) || defined(MCXW727C_cm33_core0_SERIES) || defined(KW47_core0_SERIES) || defined(KW43B43ZC7_SERIES) || defined(MCXW70AC_SERIES))
    uint32_t tmp = CMC_GetSystemResetStatus(CMC0);
    if ((tmp & (uint32_t)kCMC_Watchdog0Reset) != 0U)
    {
        CLI_PRINTF("WATCHDOG caused reset.\n");
    }
    else if((tmp & (uint32_t)kCMC_PinReset) != 0U)
    {
        CLI_PRINTF("PIN caused reset.\n");
    }
    else if((tmp & (uint32_t)kCMC_PORReset) != 0U)
    {
        CLI_PRINTF("POR caused reset.\n");
    }
    else if((tmp & (uint32_t)kCMC_SoftwareReset) != 0U)
    {
        CLI_PRINTF("SW caused reset.\n");
    }
    else
    {
        /* No action needed */
    }
#else
    uint32_t tmp = RCM_GetPreviousResetSources(RCM);
    if (tmp & kRCM_SourceWdog)
        CLI_PRINTF("WATCHDOG caused reset.\n");
    else if(tmp & kRCM_SourcePin)
        CLI_PRINTF("PIN caused reset.\n");
    else if(tmp & kRCM_SourcePor)
        CLI_PRINTF("POR caused reset.\n");
    else if(tmp & kRCM_SourceSw)
        CLI_PRINTF("SW caused reset.\n");
#endif
#endif /* FPGA_TARGET */
}

/*! *********************************************************************************
* \brief        Set/show XTAL trim value
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_misc_xtal_trim(int argc, char *argv[])
{
#if !defined(FPGA_TARGET) || (FPGA_TARGET == 0)
    uint8_t inp;
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
    hardwareParameters_t *pHWParams = NULL;
    (void)NV_ReadHWParameters(&pHWParams);
#endif
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp <= 63U)
        {
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
            pHWParams->xtalTrim = inp;
#endif
            PLATFORM_SetXtal32MhzTrim(inp, FALSE);
        }
    }

    CLI_PRINTF("xtal_trim: %u\n", (unsigned int)PLATFORM_GetXtal32MhzTrim(FALSE));
#else
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
#endif
}

/*! *********************************************************************************
* \brief        Start/Stop 2440MHz constant wave Tx (used for XTAL trimming)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_misc_cw_trim(int argc, char *argv[])
{
    static int cwRunning = 0;
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp == 1U)
        {
            if (gCommunicationSettings->role != (gapRole_t)0xff)
            {
                CLI_PRINTF("BLE communication role must be set to 'None' to run this feature.\n");
            }
            else
            {
                if (cwRunning == 0)
                {
                    RADIO_CTRL->LL_CTRL = (uint32_t)XCVR_ACTIVE_LL_GENFSK;
                    wr_xtal_trim_cw(TRUE);
                    cwRunning = 1;
                }
            }
        }
        else
        {
            if (cwRunning == 1)
            {
                wr_xtal_trim_cw(FALSE);
                RADIO_CTRL->LL_CTRL = 0;
                cwRunning = 0;
            }
        }
    }
    CLI_PRINTF("2440MHz constant wave is %srunning.\n", cwRunning != 0 ? "" : "not ");
    CLI_PRINTF("xtal_trim: %u\n", (unsigned int)XCVR_GetXtalTrim());
}

#ifdef INCLUDE_EMBEDDED_COMPUTATION_SUPPORT
/*! *********************************************************************************
* \brief        Set/show RTP embedded algorithm to be run
*               (bit0:CDE, bit2:RADE)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_misc_rtp_algo(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);

        if ((inp & ~(eMciqAlgoEmbedCDE | eMciqAlgoEmbedRADE)) == 0U)
        {
            gLocalSettings->mciq_algo_flags = inp;
        }
    }
    CLI_PRINTF("rtp_algo: %u\n", gLocalSettings->mciq_algo_flags);
}

/*! *********************************************************************************
* \brief        Set/show RTP embedded algorithm configuration
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_misc_rtp_algo_config(int argc, char *argv[])
{
    uint16_t inp;
    if(argc == 1)
    {
        gLocalSettings->cde_threshold = DM_CDE_THRESHOLD_DEFAULT;
        gLocalSettings->cde_div_threshold = DM_CDE_THRESHOLD_DIVERSITY_DEFAULT;
    }
    else if (argc == 3)
    {
        inp = (uint16_t)atoi(argv[0]);
        if (inp == 1U) /* CDE config */
        {
            inp = (uint16_t)atoi(argv[1]);
            gLocalSettings->cde_threshold = inp;
            inp = (uint16_t)atoi(argv[2]);
            gLocalSettings->cde_div_threshold = inp;
        }
    }
    else
    {
        /* No action needed */
    }
    CLI_PRINTF("rtp_algo_config: algo=1 th=%u div_th=%u\n", gLocalSettings->cde_threshold, gLocalSettings->cde_div_threshold);
}
#endif

/*! *********************************************************************************
* \brief        Run CS test mode measurement
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_test(int argc, char *argv[])
{
    isp_configuration_range_t range_params;
    int didRun = 1;

    switch(gCommunicationSettings->role) {
        case gGapCentral_c:
            CLI_PRINTF("Did not run. Communication role must be set to None!\n");
            didRun = 0;
            break;
        case gGapPeripheral_c:
            CLI_PRINTF("Did not run. Communication role must be set to None!\n");
            didRun = 0;
            break;
        default:
            /* No action required */
            break;
    }
    
    if (didRun == 0)
    {
        return;
    }
    else
    {
        /* No action needed */
    }

    /* Re-generate channel list in case some impacting parameters have been changed */
    (void)cli_generate_ch_list();

    range_params = *gRangeSettings;

    /* Enable test mode for this measurement */
    range_params.test_mode = 1;
    gLocalSettings->verbosity |= (uint8_t)eVerbosityMeasurementInfo | (uint8_t)eVerbosityMeasurementData;

    /* For usability, we keep the ability to averrid role in test command */
    if (argc == 1) {
        range_params.role = (argv[0][0] != 'r') ? 0U : 1U;
    }
    if (measurement_init(&range_params))
    {
        wrs_StartProcedure(CS_TEST_DEVICEID);
    }
}

/*! *********************************************************************************
* \brief        Run CS procedure measurement with given peer
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_range(int argc, char *argv[])
{
    deviceId_t peerId = 0; /* TODO: make the addr mandatory ? for now assume first connected device */
    int didRun = 1;
    
    switch(gCommunicationSettings->role) {
        case gGapCentral_c:
            break;
        case gGapPeripheral_c:
            break;
         default:
            CLI_PRINTF("Did not run. Communication role must be set!\n");
            didRun = 0;
            break;
    }
    
    if (didRun == 0)
    {
        return;
    }
    else
    {
        /* No action needed */
    }
    
    if (argc < 2) {
        if (argc == 2) {
            uint64_t address = 0;
            /* Remote device can be specified via address or via index in address list */
            if (FLib_MemCmp(argv[1], "0x", 2))
            {
                address = parse_hex64(argv[1]);
            }
            else
            {
                uint32_t idx = (uint32_t)atoi(argv[1]);
                if(idx < gCommunicationSettings->address_list_len)
                {
                    FLib_MemCpy(&address, (uint8_t*)gCommunicationSettings->address_list[idx], gcBleDeviceAddressSize_c);
                }
            }
            peerId = BleApp_address_2_device_id((uint8_t*)&address);
        }
        if (!BleInfo_IsConnected() || peerId == gInvalidDeviceId_c) {
            CLI_PRINTF("Did not run. Connection must be established!\n");
            return;
        }
        /* The device starting the range request is the client */
        wrs_SetRole(peerId, eRoleClient);
        if (measurement_init(gRangeSettings))
        {
            wrs_StartConfig(peerId);
        }
    }
}

/*! *********************************************************************************
* \brief        Run CS setup (from Central only)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_setup(int argc, char *argv[])
{
    uint8_t peerId = 0; /* TODO: make the addr mandatory ? for now assume first connected device */
    int didRun = 1;
    
    switch(gCommunicationSettings->role) {
        case gGapCentral_c:
            break;
        case gGapPeripheral_c:
            CLI_PRINTF("Did not run. Communication role must be Central!\n");
            didRun = 0;
            break;
         default:
            CLI_PRINTF("Did not run. Communication role must be set!\n");
            didRun = 0;
            break;
    }
    
    if (didRun == 0)
    {
        return;
    }
    else
    {
        /* No action needed */
    }
    
    if (argc < 2) {
        if (argc == 2) {
            uint64_t address = 0;
            /* Remote device can be specified via address or via index in address list */
            if (FLib_MemCmp(argv[1], "0x", 2))
            {
                address = parse_hex64(argv[1]);
            }
            else
            {
                uint32_t idx = (uint32_t)atoi(argv[1]);
                if(idx < gCommunicationSettings->address_list_len)
                {
                    FLib_MemCpy(&address, (uint8_t*)gCommunicationSettings->address_list[idx], gcBleDeviceAddressSize_c);
                }
            }
            peerId = BleApp_address_2_device_id((uint8_t*)&address);
        }
        if (!BleInfo_IsConnected() || peerId == gInvalidDeviceId_c) {
            CLI_PRINTF("Did not run. Connection must be established!\n");
            return;
        }
        wrs_StartSetup(peerId);
    }
}

/*! *********************************************************************************
* \brief        Set/show communication role
*               Always show current value, parse when argument is available
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_communication_role(int argc, char *argv[])
{
    if(argc == 1) {
        switch(argv[0][0]) {
            case 'c':
            case 'C':
                gCommunicationSettings->role = gGapCentral_c;
                break;
            case 'p':
            case 'P':
                gCommunicationSettings->role = gGapPeripheral_c;
                break;
            default:
                gCommunicationSettings->role = (gapRole_t)0xff;
                break;
        }
    }

    CLI_PRINTF("role: ");
    switch(gCommunicationSettings->role) {
        case gGapCentral_c: CLI_PRINTF("central\n"); break;
        case gGapPeripheral_c: CLI_PRINTF("peripheral\n"); break;
        default: CLI_PRINTF("none\n"); break;
    }
}

/*! *********************************************************************************
* \brief        Set/show connection interval duration (in 1.25 slots)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */

void cli_cmd_communication_interval(int argc, char *argv[])
{
    uint32_t inp;
    if(argc == 1) {
        inp = (uint32_t)atoi(argv[0]);
        if ((inp >= 6U) && (inp < 3200U)) {
            gCommunicationSettings->conn_int = inp;
        }
    }

    CLI_PRINTF("connection interval: %d\n", gCommunicationSettings->conn_int);
}

/*! *********************************************************************************
* \brief        Get local BLE address
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_communication_local(int argc, char *argv[])
{
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
    hardwareParameters_t *pHWParams = NULL;
    (void) NV_ReadHWParameters(&pHWParams);
    uint8_t *ptr = pHWParams->bluetooth_address;
    CLI_PRINTF("local: 0x%02X%02X%02X%02X%02X%02X\n", ptr[5], ptr[4], ptr[3], ptr[2], ptr[1], ptr[0]);
#endif
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
}

/*! *********************************************************************************
* \brief        Set/show allowed remote ble-address list
*               (0x0 allows any address; must be a hexadecimal number)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_communication_remote(int argc, char *argv[])
{
    uint64_t inp;
    uint8_t mPeerId, *ptr;
    if (argc > 0)
    {
        FLib_MemSet(gCommunicationSettings->address_list, 0, sizeof(gCommunicationSettings->address_list));
        for(mPeerId = 0; mPeerId < (uint8_t)argc && mPeerId < (uint8_t)gLclMaxConnections_c; mPeerId++)
        {
            inp = parse_hex64(argv[mPeerId]);
            FLib_MemCpy(gCommunicationSettings->address_list[mPeerId], &inp, gcBleDeviceAddressSize_c);
            gCommunicationSettings->address_list_len = mPeerId + 1U;
        }
    }
    CLI_PRINTF("remote: ");
    for(mPeerId = 0; mPeerId < gCommunicationSettings->address_list_len; mPeerId++)
    {
        ptr = gCommunicationSettings->address_list[mPeerId];
        CLI_PRINTF("0x%02X%02X%02X%02X%02X%02X ", ptr[5], ptr[4], ptr[3], ptr[2], ptr[1], ptr[0]);
    }
    CLI_PRINTF("\n");
}

/*! *********************************************************************************
* \brief        Show connection status
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_communication_connected(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    int8_t val = 0;
    deviceId_t peerId = 0;

    /* A connection is ready for range requests when BLE connection is established and CS security procedure has ended */
    if (wrs_ConnIsReady(peerId)) { /* TODO: manage sevral connections */
        val = BleInfo_GetConnections(TRUE);
    }
    CLI_PRINTF("connected: %d\n", val);
}

/*! *********************************************************************************
* \brief        Re-generate channel list
********************************************************************************** */
bool_t cli_generate_ch_list(void)
{
    uint32_t ch_start = gRangeSettings->ch_start;
    uint32_t ch_stop = gRangeSettings->ch_stop;

    if ((ch_stop - ch_start + 1) >= CS_STEP_NB_MAX_PER_SE) {
        return FALSE;
    }
    
    /* Do not re-generate channel list if in manual mode */
    if (!gRangeSettings->ch_list_auto) {
        return FALSE;
    }

    gRangeSettings->ch_list_auto = 1;
    gRangeSettings->ch_nb = 0;

    /* Generate temporary channel list */
    for (int i = 0; i < ch_stop - ch_start + 1; i++) {
        if (gRangeSettings->ch_map[i/8] & (1 << i%8)) {
            gRangeSettings->ch_list[gRangeSettings->ch_nb] = ch_start + i;
            gRangeSettings->ch_nb++;
        }
    }
    return TRUE;
}

/*! *********************************************************************************
* \brief        Print channel list
********************************************************************************** */
void cli_print_ch_list(void)
{
    if (gRangeSettings->ch_list_auto) {
        CLI_PRINTF("ch_list: auto(%d,%d,%s) (size=%d) ",
                   gRangeSettings->ch_start, gRangeSettings->ch_stop,
                   gRangeSettings->ch_isrand?"rand":"norand", gRangeSettings->ch_nb);
    } else {
        CLI_PRINTF("ch_list: manual (size=%d) ", gRangeSettings->ch_nb);
    }
    for (uint8_t i=0; i < gRangeSettings->ch_nb; i++) {
        CLI_PRINTF("%d", gRangeSettings->ch_list[i]);
        if (i < gRangeSettings->ch_nb - 1U) {
            CLI_PRINTF(",");
        }
    }
    CLI_PRINTF("\n");
}

/*! *********************************************************************************
* \brief        Generate channel list from first and last CS channels
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_ch_list_gen(int argc, char *argv[])
{
    uint8_t inp;

    if(argc < 2 || argc > 3 || ((argc == 3) && (argv[2][0] != 'r')))
    {
        CLI_PRINTF("Invalid number of arguments.\n");
    } else {
        /* Remember the fact that the channel list must be maintained automatically */
        gRangeSettings->ch_list_auto = TRUE;
        if (argc == 3)
        {
            gRangeSettings->ch_isrand = TRUE;
        }
        inp = (uint8_t)atoi(argv[0]);
        if(inp <= XCVR_CHAN_MAX)
        {
            gRangeSettings->ch_start = inp;
        }
        inp = (uint8_t)atoi(argv[1]);
        if(inp <= XCVR_CHAN_MAX)
        {
            gRangeSettings->ch_stop = inp;
        }

        if ((gRangeSettings->ch_start > XCVR_CHAN_MAX) || (gRangeSettings->ch_stop > XCVR_CHAN_MAX) ||
            (gRangeSettings->ch_start > gRangeSettings->ch_stop) || ((gRangeSettings->mode0_nb + gRangeSettings->ch_stop - gRangeSettings->ch_start + 1U) > CS_STEP_NB_MAX_PER_SE)) {
                /* Not a valid config, do not apply it */
        } else {
            (void)cli_generate_ch_list();
        }
    }
    cli_print_ch_list();
}

/*! *********************************************************************************
* \brief        Set/show channel list
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_ch_list_set(int argc, char *argv[])
{
    int repeat = 1;

    if (argc == 2) {
        repeat = atoi(argv[1]);
    }
    if ((argc == 1 || argc == 2) && (repeat > 0))
    {
        char *str = argv[0];
        const char sep[] = ",";
        char *token;

        /* Remember the fact that the channel list has been set manually */
        gRangeSettings->ch_list_auto = FALSE;
        /* walk through provided channels (decimal, comma-separated) */
        gRangeSettings->ch_nb = 0;
        token = strtok(str, sep);
        while( token != NULL ) {
            gRangeSettings->ch_list[gRangeSettings->ch_nb] = (uint8_t)atoi(token);
            if (gRangeSettings->ch_list[gRangeSettings->ch_nb] > XCVR_CHAN_MAX)
            {
                break;
            }
            token = strtok(NULL, sep);
            gRangeSettings->ch_nb ++;
        }
        if (gRangeSettings->ch_nb * (uint8_t)repeat < CS_STEP_NB_MAX_PER_SE) {
            int ch_nb = (int)(gRangeSettings->ch_nb);
            gRangeSettings->ch_nb *= (uint8_t)repeat;
            while (repeat > 1) {
                repeat --;
                for (int i = 0; i < ch_nb; i++) {
                    gRangeSettings->ch_list[repeat * ch_nb + i] = gRangeSettings->ch_list[i];
                }
            }
        }
    }

    cli_print_ch_list();
}

/*! *********************************************************************************
* \brief        Set/show channel map as 10 hexa octets (79 bits used)
*               ch_map 0x09080706050403020100 [<repeat>]
*               LSB represents the CS channel index 0 and bit 78 represents CS channel index 78
*               Use param ch_map 0x1FFFFFFFFFFFFC7FFFFC to mask advertising channels
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_ch_map(int argc, char *argv[])
{
    int repeat = 1;
    uint8_t string[2*CS_CH_MAP_LEN+3];

    if (argc == 2) {
        repeat = atoi(argv[1]);
    }
    if ((argc == 1 || argc == 2) && (repeat > 0))
    {
        char *hex = argv[0]; /* Should start with 0x */
        int len = (int)strlen(hex);
        if ((len == CS_CH_MAP_LEN*2+2) && (FLib_MemCmp(hex, "0x", 2))) {
            vparse_hex64_little_endian(hex + 2, gRangeSettings->ch_map);
            gRangeSettings->ch_map_repeat = (uint8_t)repeat;
        }
    }
    cli_sprint_hex8b_reverse(string, gRangeSettings->ch_map, CS_CH_MAP_LEN);
    CLI_PRINTF("ch_map: 0x%s, %d\n", string, gRangeSettings->ch_map_repeat);

    /* Update channel list */
    (void)cli_generate_ch_list();
}

/*! *********************************************************************************
* \brief        Set/show channel selection algorithm
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_cs_algo(int argc, char *argv[])
{
    uint8_t inp;
    if((argc >= 1) && (argc <= 3))
    {
        inp = (uint8_t)atoi(argv[0]);
        gRangeSettings->ch_sel_algo = inp;
        
        if (argc >= 2)
        {
            inp = (uint8_t)atoi(argv[1]);
            if (inp <= 1U)
            {
                gRangeSettings->ch_sel_shape = inp;
            }
        }
        if (argc >= 3)
        {
            inp = (uint8_t)atoi(argv[2]);
            if (inp >= 2U && inp <= 8U)
            {
                gRangeSettings->ch_sel_jump = inp;
            }
        }
    } 
    else if (argc > 0) 
    {
        CLI_PRINTF("Invalid number of arguments.\n");
    }
    else
    {
        /* No action needed */
    }
    CLI_PRINTF("cs_algo: type=%d, shape=%d jump=%d\n",
               gRangeSettings->ch_sel_algo, gRangeSettings->ch_sel_shape, gRangeSettings->ch_sel_jump);
}

/*! *********************************************************************************
* \brief        Set/show numbers of CS main mode steps (min, max, repetition)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_main_mode_nb(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 3)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp >= 1U && inp <= 160U && gRangeSettings->main_mode_min != inp)
        {
            gRangeSettings->main_mode_min = inp;
        }
        inp = (uint8_t)atoi(argv[1]);
        if (inp >= 1U && inp <= 160U && gRangeSettings->main_mode_max != inp)
        {
            gRangeSettings->main_mode_max = inp;
        }
        inp = (uint8_t)atoi(argv[2]);
        if (inp <= CS_MAIN_MODE_REPEAT_MAX && gRangeSettings->main_mode_repeat != inp)
        {
            gRangeSettings->main_mode_repeat = inp;
        }
    } 
    else if (argc > 0) 
    {
        CLI_PRINTF("Invalid number of arguments.\n");
    } 
    else
    {
        /* No action needed */
    }
    CLI_PRINTF("main_mode_nb: min=%d, max=%d, repeat=%d\n",
               gRangeSettings->main_mode_min,
               gRangeSettings->main_mode_max,
               gRangeSettings->main_mode_repeat);
}

/*! *********************************************************************************
* \brief        Set/show number of CS mode0 steps
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_mode0_nb(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if(inp >= 1U && inp <= 3U && gRangeSettings->mode0_nb != inp)
        {
            gRangeSettings->mode0_nb = inp;
        }
    }
    CLI_PRINTF("mode0_nb=%d\n", gRangeSettings->mode0_nb);
}

/*! *********************************************************************************
* \brief        Set/show main mode and sub-mode types
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_mode_type(int argc, char *argv[])
{
    uint8_t inp;
    if (argc == 1 || argc == 2)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp >= 1U && inp <= 3U)
        {
            gRangeSettings->main_mode_type = inp;
        }
        if (argc == 2)
        {
            inp = (uint8_t)atoi(argv[1]);
            if (inp >= 1U && inp <= 3U)
            {
                gRangeSettings->sub_mode_type = inp;
            }
        } else {
            /* sub mode type is unused */
            gRangeSettings->sub_mode_type = 0xFF;
        }
    }
    CLI_PRINTF("mode_type: mainmode=%d, submode=%d\n",
               gRangeSettings->main_mode_type, gRangeSettings->sub_mode_type);
}

/*! *********************************************************************************
* \brief        Set/show procedure repetition number
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_repeat(int argc, char *argv[])
{
    uint16_t inp;
    if (argc >= 1 && argc <=2)
    {
        inp = (uint16_t)atoi(argv[0]);
        if (inp >= 1U) {
            gRangeSettings->max_proc_count = inp;
        }
        if (argc == 2)
        {
            inp = (uint16_t)atoi(argv[1]);
            gRangeSettings->proc_interval = inp;
        }
    }
    CLI_PRINTF("repeat: %u %u\n", gRangeSettings->max_proc_count, gRangeSettings->proc_interval);
}

/*! *********************************************************************************
* \brief        Set/show subevent length
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_subevent_len(int argc, char *argv[])
{
    uint32_t inp;
    if (argc == 1)
    {
        inp = (uint32_t)atoi(argv[0]);
        if (inp >= 1250U && inp <= 4000000U) {
            gRangeSettings->subevent_len = inp;
        }
    }
    CLI_PRINTF("subevent_len: %u\n", gRangeSettings->subevent_len);
}

/*! *********************************************************************************
* \brief        Set/show antenna config index and (test mode only) permutation index
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_ant_cfg(int argc, char *argv[])
{
    uint8_t inp;
    if (argc >= 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp <= 7U)
        {
            gRangeSettings->ant_cfg_index = inp;
        }
        if (gRangeSettings->ant_cfg_index != 0U)
        {
            if (argc >= 2)
            {
                inp = (uint8_t)atoi(argv[1]);
                if ((inp <= 23U) || (inp == 255U))
                {
                    gRangeSettings->ant_perm_index = inp;
                }
            }
            if (argc == 3)
            {
                inp = (uint8_t)atoi(argv[2]);
                if (((inp > 0U) && (inp <= 2U)) || (inp == 0xFEU) || (inp == 0xFFU))
                {
                    gRangeSettings->ant_CS_SYNC = inp;
                }
            }
        }
    }
    CLI_PRINTF("ant_cfg: index=%d, perm=%d, cs_sync=%d\n", gRangeSettings->ant_cfg_index, gRangeSettings->ant_perm_index, gRangeSettings->ant_CS_SYNC);
}

/*! *********************************************************************************
* \brief        Set/show antenna board type:
*               0:none, 1:X-FR-ANTDIV SMA, 2:X-FR-ANTDIV printe
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_ant_type(int argc, char *argv[])
{
    uint8_t inp;
    if (argc >= 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp <= 6U)
        {
            /* update config to NBU and instal RF_GPO PIN mux */
            /* KW47 and more series */
            if (inp > 0)
            {
                /* Mux RF_GPO pins to control antenna diversity RF switch */
                PLATFORM_InitLclGpioDebug(false);
            }
            else
            {
                /* Restore controller debug capacity */
                PLATFORM_InitLclGpioDebug(true);
            }
            if (measurement_send_vs_config(gRangeSettings))
            {
                gRangeSettings->ant_type = inp;
            }
#if  defined(gAppEnableDebugData) && (gAppEnableDebugData == 1)
            (void)measurement_send_vs_debug(gRangeSettings);
#endif
        }
    }
    CLI_PRINTF("ant_type=%d\n", gRangeSettings->ant_type);
}

/*! *********************************************************************************
* \brief        Set/show CS role
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_role(int argc, char *argv[])
{
    if(argc == 1)
    {
        switch(argv[0][0])
        {
            case 'i':
            case 'I':
                gRangeSettings->role = 0;
                break;
            case 'r':
            case 'R':
                gRangeSettings->role = 1;
                break;
            default:
                /* No action required */
                break;
        }
    }
    else
    {
        /* No action needed */
    }

    CLI_PRINTF("role: ");
    switch(gRangeSettings->role)
    {
        case 0: CLI_PRINTF("initiator\n"); break;
        default: CLI_PRINTF("reflector\n"); break;
    }
}

/*! *********************************************************************************
* \brief        Set/show RTT phy rate (0=1Mbps, 1=2Mbps)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_rtt_phy(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if(inp <= 2U)
        {
            gRangeSettings->rtt_phy = inp;
        }
    }
    CLI_PRINTF("rtt_phy: %d\n", gRangeSettings->rtt_phy);
}

/*! *********************************************************************************
* \brief        Set/show RTT type
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_rtt_type(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp <= 6U)
        {
            gRangeSettings->rtt_type = inp;
        }
    }
    else
    {
        /* No action needed */
    }
    CLI_PRINTF("rtt_type: %d\n", gRangeSettings->rtt_type);
}

/*! *********************************************************************************
* \brief        Set/show TP tone extension (test mode only)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_tone_ext(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if(inp <= 4U)
        {
            gRangeSettings->t_pm_tone_ext = inp;
        }
    }
    CLI_PRINTF("tone_ext: %d\n", gRangeSettings->t_pm_tone_ext);
}

/*! *********************************************************************************
* \brief        Set/show PN sequences (must be a hex number, test mode only)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_pn_seq(int argc, char *argv[])
{
    uint64_t inp;
    if(argc >= 1)
    {
        inp = parse_hex64(argv[0]);
        FLib_MemCpy(gRangeSettings->initiator_AA, &inp, sizeof(gRangeSettings->initiator_AA));
    }
    if(argc == 1)
    {
        FLib_MemCpy(gRangeSettings->reflector_AA, &inp, sizeof(gRangeSettings->reflector_AA));
    }
    else if(argc == 2)
    {
        inp = parse_hex64(argv[1]);
        FLib_MemCpy(gRangeSettings->reflector_AA, &inp, sizeof(gRangeSettings->reflector_AA));
    }
    else
    {
        /* No action needed */
    }
    CLI_PRINTF("pn_seq: 0x%02X%02X%02X%02X 0x%02X%02X%02X%02X\n", gRangeSettings->initiator_AA[3], gRangeSettings->initiator_AA[2], gRangeSettings->initiator_AA[1], gRangeSettings->initiator_AA[0],
               gRangeSettings->reflector_AA[3], gRangeSettings->reflector_AA[2], gRangeSettings->reflector_AA[1], gRangeSettings->reflector_AA[0]);
}

/*! *********************************************************************************
* \brief        Set/show CS interval timings T_FCS, T_IP1, T_IP2 (us)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_timings_intervals(int argc, char *argv[])
{
    uint8_t inp1, inp2, inp3;
    if(argc == 2 || argc == 3)
    {
        inp1 = (uint8_t)atoi(argv[0]); /* T_FCS */
        inp2 = (uint8_t)atoi(argv[1]); /* T_IP1 */
        if (argc == 3) {
            inp3 = (uint8_t)atoi(argv[2]); /* T_IP2 */
        } else {
            /* if not specified, T_IP2 equals T_IP1 */
            inp3 = inp2;
        }
        /* Check allowed values */
        if ((inp1 == 150U || inp1 == 120U || inp1 == 100U || inp1 == 80U || inp1 == 60U || inp1 == 50U || inp1 == 40U) &&
            (inp2 == 145U || inp2 == 80U || inp2 == 60U || inp2 == 50U || inp2 == 40U || inp2 == 30U) &&
            (inp3 == 145U || inp3 == 80U || inp3 == 60U || inp3 == 50U || inp3 == 40U || inp3 == 30U))
        {
            gRangeSettings->t_fcs = inp1;
            gRangeSettings->t_ip1 = inp2;
            gRangeSettings->t_ip2 = inp3;
        } else {
            CLI_PRINTF("Invalid input.\n");
        }
    }
    CLI_PRINTF("timings: T_FCS %d, T_IP1 %d, T_IP2 %d\n", gRangeSettings->t_fcs, gRangeSettings->t_ip1, gRangeSettings->t_ip2);
}

/*! *********************************************************************************
* \brief        Set/show CS T_PM timing (us)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_timings_tpm(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp == 10U || inp == 20U || inp == 40U)
        {
            gRangeSettings->t_pm = inp;
        }
    }
    CLI_PRINTF("t_pm: %d\n", gRangeSettings->t_pm);
}

/*! *********************************************************************************
* \brief        Set/show CS T_SW timing (us)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_timings_tsw(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp == 2U || inp == 4U || inp == 10U)
        {
            gRangeSettings->t_sw_local = inp;
        }
    }
    CLI_PRINTF("t_sw: %d\n", gRangeSettings->t_sw_local);
}

/*! *********************************************************************************
* \brief        Set/show TX power in dBm of ranging measurement (test mode only)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_tx_pwr(int argc, char *argv[])
{
    int8_t inp;
    if(argc == 1)
    {
        inp = (int8_t)atoi(argv[0]);
        if(inp <= XCVR_TX_PWR_MAX && inp >= XCVR_TX_PWR_MIN && gRangeSettings->tx_pwr != inp)
        {
            gRangeSettings->tx_pwr = inp;
        }
    }
    CLI_PRINTF("tx_pwr: %d\n", gRangeSettings->tx_pwr);
}

/*! *********************************************************************************
* \brief        Set/show disable debug options
*               (REPORT_DBG_INFO<2>, IQ_AVERAGE_DISABLE<1>, IQ_DETAILS<0>)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_parameter_debug(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        gRangeSettings->debug = inp;
    }
    CLI_PRINTF("debug: %u\n", gRangeSettings->debug);
}

/*! *********************************************************************************
* \brief        Show unique id
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_unique_id(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    CLI_PRINTF("unique_id: 0x%02X%02X%02X%02X%02X%02X\n",
               local_bid.uid[5], local_bid.uid[4], local_bid.uid[3],
               local_bid.uid[2], local_bid.uid[1], local_bid.uid[0]);
}

/*! *********************************************************************************
* \brief        Enable debug features (DTEST...)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_debug(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        if (inp == 0U || inp == 1U) {
            gLocalSettings->debug = inp;
            if (inp == 1U) {
#if (defined(gAppLedCnt_c) && (gAppLedCnt_c > 0))
                BleApp_SetLED(mLedOff);
#endif
            }
            isp_soc_debug_set(inp);
        }
    }
    CLI_PRINTF("debug: %d\n", gLocalSettings->debug);
}

/*! *********************************************************************************
* \brief        Set/show verbosity flags (1=Info, 2=Debug, 4=MeasurementInfo,
*               8=MeasurementData, 16=MeasurementDebug, 32=BoardInfo, 64=Profiling)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_verbosity(int argc, char *argv[])
{
    uint8_t inp;
    if(argc == 1)
    {
        inp = (uint8_t)atoi(argv[0]);
        gLocalSettings->verbosity = inp;
    }
    CLI_PRINTF("verbosity: %d\n", gLocalSettings->verbosity);
}

/*! *********************************************************************************
* \brief        Show SW version number
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_version(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    CLI_PRINTF("version: v%u.%u.%u.%u\n", c_app_version.major, c_app_version.minor, c_app_version.patch, c_app_version.build);
    return;
}

/*! *********************************************************************************
* \brief        Show SW build number
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_build_info(int argc, char *argv[])
{
    static NbuInfo_t cli_nbu_info = { 0 };
    static bool nbu_info_received = false;

    (void)argc;
    (void)argv;
    uint8_t hash_string[MAX_SHA_SZ*2];
    if ( !nbu_info_received )
    {
        /* Request NBU Version only the first time and store to local static NbuOnfo_t */
        int st;
        st = PLATFORM_GetNbuInfo(&cli_nbu_info); /* PLATFORM_GetNbuInfo loops waiting for NBU infor to be received */
        if (st == 0)
        {
            nbu_info_received = true;
        }
    }

    CLI_PRINTF("Application:\n");
    CLI_PRINTF("  version    = v%u.%u.%u.%u\n", c_app_version.major, c_app_version.minor, c_app_version.patch, c_app_version.build);
#if defined(gIncludeBuildDate_d) && (gIncludeBuildDate_d == 1U)
    CLI_PRINTF("  Build Date = %s\n", c_app_build_date);
#endif
#ifdef DEBUG
    CLI_PRINTF("  Build Type = Debug\n");
#else
    CLI_PRINTF("  Build Type = Release\n");
#endif
    if(c_app_manual)
        CLI_PRINTF("* Manual (and/or dirty) build! *\n");
    if (nbu_info_received)
    {
        cli_sprint_hex8b(hash_string, cli_nbu_info.repo_digest, MAX_SHA_SZ, NULL);

        CLI_PRINTF("Controller:\n");
        CLI_PRINTF("  GIT Hash   = %s\n", hash_string);
        CLI_PRINTF("  Variant    = %s\n", cli_nbu_info.variant);
        CLI_PRINTF("  Build Type = %s\n", cli_nbu_info.build_type);
    }
    return;
}

/*! *********************************************************************************
* \brief        Show events
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_events(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    isp_soc_eventlog_print();
    return;
}

/*! *********************************************************************************
* \brief        Timing | Show BLE timing
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_timing(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    return;
}

/*! *********************************************************************************
* \brief        Set/show baudrate of serial interface (default: 115200, volatile)
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_baudrate(int argc, char *argv[])
{
    /*no api to set baudrate, you can reinit serial manager*/
    return;
}

/*! *********************************************************************************
* \brief        Show output format
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_output_format(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    CLI_PRINTF("output_format: 4\n");
    return;
}

/*! *********************************************************************************
* \brief        Reset MCU
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_reset(int argc, char *argv[])
{
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */
    HAL_ResetMCU();
}

/*! *********************************************************************************
* \brief        Factory | Reload and store factory defaults
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_factory(int argc, char *argv[])
{
    uint32_t default_initiator_aa = 0x215EE636;
    uint32_t default_reflector_aa = 0x41118D59;
    
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
    hardwareParameters_t *pHWParams = NULL;
    
    (void) NV_ReadHWParameters(&pHWParams);
 
#if (defined(KW37A4_SERIES) || defined(KW37Z4_SERIES) || defined(KW38A4_SERIES) || defined(KW38Z4_SERIES) || defined(KW39A4_SERIES))
    pHWParams->xtalTrim = BOARD_GetXtal32MhzTrim(FALSE);
#else
    pHWParams->xtalTrim = PLATFORM_GetXtal32MhzTrim(FALSE);
#endif
    (void)XCVR_SetXtalTrim(pHWParams->xtalTrim);
#endif /* gPlatformUseHwParameter_d */
    
    argc = argc; /* MISRA rule 2.7 */
    argv = argv; /* MISRA rule 2.7 */

    gLocalSettings->debug = 0;
    gLocalSettings->verbosity = 0;
    gLocalSettings->mciq_algo_flags = eMciqAlgoEmbedCDE;
    gLocalSettings->cde_threshold = DM_CDE_THRESHOLD_DEFAULT;
    gLocalSettings->cde_div_threshold = DM_CDE_THRESHOLD_DIVERSITY_DEFAULT;

    /* Measurements settings */
    gRangeSettings->debug = 0;
    gRangeSettings->max_proc_count = 1;
    gRangeSettings->proc_interval = 0; /* 0 = will be computed by application */
    gRangeSettings->subevent_len = WR_CS_SUBEVT_DURATION_US_MAX; /* special value to signify no fragmentation */
    gRangeSettings->main_mode_type = 2; /* RTP */
    gRangeSettings->sub_mode_type = 1; /* RTT */
    gRangeSettings->main_mode_min = 4;
    gRangeSettings->main_mode_max = 8;
    gRangeSettings->main_mode_repeat = 1;
    gRangeSettings->mode0_nb = 3;
    gRangeSettings->rtt_type = 0; /* coarse */
    gRangeSettings->rtt_phy = 0;  /* 1 Mbps */
    gRangeSettings->role = 0;  /* Initiator */
    gRangeSettings->t_fcs = 150;
    gRangeSettings->t_ip1 = 145;
    gRangeSettings->t_ip2 = 145;
    gRangeSettings->t_pm = 20;
    gRangeSettings->t_sw_local = 2;
    gRangeSettings->t_pm_tone_ext = 0;
    gRangeSettings->tx_pwr = 0; /* 0 dBm by default */
    gRangeSettings->test_mode = 0;
    gRangeSettings->ch_map_repeat = 1;
    gRangeSettings->ch_sel_algo = 0; /* #3b */
    gRangeSettings->ch_sel_shape = 0; /* Hat shape */
    gRangeSettings->ch_sel_jump = 3;
#if defined(BOARD_LOCALIZATION_REVISION_SUPPORT) && (BOARD_LOCALIZATION_REVISION_SUPPORT > 0)
    gRangeSettings->ant_cfg_index = 7;
    gRangeSettings->ant_type = 4;
#else
    gRangeSettings->ant_cfg_index = 0;
    gRangeSettings->ant_type = 0;
#endif
    gRangeSettings->ant_perm_index = 0;
    gRangeSettings->ant_CS_SYNC = 0xFF;  /* propietary round-robin */
    gRangeSettings->ch_list_auto = TRUE;
    gRangeSettings->ch_start = 0;
    gRangeSettings->ch_stop = XCVR_CHAN_MAX;
    gRangeSettings->ch_isrand = FALSE;
    FLib_MemCpy(gRangeSettings->initiator_AA, &default_initiator_aa, sizeof(default_initiator_aa));
    FLib_MemCpy(gRangeSettings->reflector_AA, &default_reflector_aa, sizeof(default_reflector_aa));
    FLib_MemSet(gRangeSettings->ch_map, 0xFF, CS_CH_MAP_LEN);
    (void)cli_generate_ch_list();

    /* Communication */
    gCommunicationSettings->role = (gapRole_t)0xff;
    gCommunicationSettings->conn_int = 24; /* 30ms */
    gCommunicationSettings->address_list_len = 0;
    FLib_MemSet(gCommunicationSettings->address_list, 0, sizeof(gCommunicationSettings->address_list));

    CLI_PRINTF("factory: 1\n");
}

/*! *********************************************************************************
* \brief        Store all parameters
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_store(int argc, char *argv[])
{
#if defined(gPlatformUseHwParameter_d) && (gPlatformUseHwParameter_d > 0)
    hardwareParameters_t *pHWParams = NULL;
    (void) NV_ReadHWParameters(&pHWParams);
    (void)NV_WriteHWParameters();
#endif
    isp_globals_save();
    CLI_PRINTF("store: 1\n");
}

/*! *********************************************************************************
* \brief        Set/Show zero-distance calibration, distances are in meters, Q10
*
* \param[in]    argc         Argument count
* \param[in]    argv         Argument list
********************************************************************************** */
void cli_cmd_system_calibrate(int argc, char *argv[])
{
    uint16_t inp;

    if ((argc == 2) || (argc == 3))
    {
        inp = (uint16_t)atoi(argv[0]);
        local_bid.cal.mciq_zdc = inp;
        inp = (uint16_t)atoi(argv[1]);
        local_bid.cal.tof_zdc = inp;
    }
    CLI_PRINTF("calibrate: MCIQ=%u ToF=%u \n", local_bid.cal.mciq_zdc, local_bid.cal.tof_zdc);

    if (argc == 3)
    {
        if ((argv[2][0] == 's') || (argv[2][0] == 'S'))
        {
            isp_save_calibration_data(&local_bid.cal);
            CLI_PRINTF("saved to Flash.\n");
        }
    }
}

#if (defined(gAppButtonCnt_c) && (gAppButtonCnt_c > 0))
/* SW3 on KW45 EVK */
/*! *********************************************************************************
* \brief        Handler for the first key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
button_status_t BleApp_HandleKeys0(void *buttonHandle, button_callback_message_t *message,void *callbackParam)
{
    switch (message->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
        {
            gapRole_t role = gCommunicationSettings->role;
            /* Cycle through communication states */
            cli_cmd_system_factory(0, NULL);
            if (role == gGapPeripheral_c) {
                gCommunicationSettings->role = gGapCentral_c;
            } else if (role == gGapCentral_c) {
                gCommunicationSettings->role = (gapRole_t)0xff;
            } else {
                gCommunicationSettings->role = gGapPeripheral_c;
            }
            cli_cmd_system_store(0, NULL);
            break;
        }

        case kBUTTON_EventLongPress:
            /* Shortcut to toggle debug mode */
            if (gLocalSettings->debug == 0U) {
                gLocalSettings->debug = 1;
                BleApp_SetLED(mLedOff);
                isp_soc_debug_set(1);
            } else {
                gLocalSettings->debug = 0;
                isp_soc_debug_set(0);
                BleApp_SetLED(mLedIdle);
            }
            break;

        default:
        {
            /* No action required */
            break;
        }
    }
    return kStatus_BUTTON_Success;
}

/*! *********************************************************************************
* \brief        Handler for the second key.
*
* \param[in]    pButtonHandle       Pointer to the button handle.
* \param[in]    pMessage            Pointer to the message.
* \param[in]    pCallbackParam      Pointer to the callback parameters.
********************************************************************************** */
button_status_t BleApp_HandleKeys1(void *buttonHandle, button_callback_message_t *message,void *callbackParam)
{
    switch (message->event)
    {
        case kBUTTON_EventOneClick:
        case kBUTTON_EventShortPress:
            /* No action required */
            break;

        case kBUTTON_EventLongPress:
            /* No action required */
            break;

        default:
            /* No action required */
            break;
    }
    return kStatus_BUTTON_Success;
}
#endif /*gAppButtonCnt_c > 0*/


/*! *********************************************************************************
* \brief        UART Rx Callback.
*
* \param[in]    callbackParam      Pointer to the callback parameters
* \param[in]    message            Pointer to the message.
* \param[in]    status             Status of receive procedure
********************************************************************************** */
void cli_uart_rx_cb
(
    void *callbackParam,
    serial_manager_callback_message_t *message,
    serial_manager_status_t status
)
{
#define CTL_BACKSPACE           ((uint8_t)'\b')
#define CTL_LINEFEED            ((uint8_t)'\n')
    static uint8_t readBuffer[gSerialMgrRxBufSize_c];
    static int readBufferOffset = 0;
    static bool_t in_error = FALSE;
    serial_manager_status_t read_status;
    uint32_t byteCount = 0U;
    uint8_t readData = 0U;

    (void)status;  /* MISRA rule 2.7 */
    (void)message; /* MISRA rule 2.7 */

    do {
        read_status = SerialManager_TryRead(s_ispCliReadHandle, &readData, 1U, &byteCount);
        if ((read_status == kStatus_SerialManager_Success) && (byteCount != 0U)) {
            if ((readData == CTL_BACKSPACE) && (readBufferOffset > 0)) {
                readBufferOffset --;
            } else {
                readBuffer[readBufferOffset] = readData;
                readBufferOffset ++;
            }
            if (readBufferOffset >= gSerialMgrRxBufSize_c) {
                /* If line is too long, just drop it */
                readBufferOffset = 0;
                in_error = TRUE;
            }
            /* End of line */
            if (readData == CTL_LINEFEED) {
                if (in_error) {
                    CLI_PRINTF("Command ignored (too long)\n");
                } else {
                    readBuffer[readBufferOffset - 1] = (uint8_t)'\0';
                    cmdparse((char*)readBuffer, menu_tbl);
                }
                readBufferOffset = 0;
                in_error = FALSE;
            }
        }
    }  while(byteCount != 0U);
}
#undef CTL_BACKSPACE

/*! *********************************************************************************
* \brief        Utility for controling constant wave for trimming (2440MHz).
*
* \param[in]    sense       when set to 1: start constant wave Tx
*                           when set to 0: stop Cw transmition
********************************************************************************** */
void wr_xtal_trim_cw(bool_t sense)
{
    const uint32_t rf_freq = 2440U; /* MHertz */

    if (sense)
    {
        (void)XCVR_ForcePAPower(0x3E);

        /* Set channel / frequency for XTAL calibration via RF antenna */
#if (NXP_RADIO_GEN >= 350)
        (void)XCVR_DftTxCW(rf_freq*1000000U);  /* In Hertz */
#else
        XCVR_OverrideChannel(rf_freq*1000000U);
        XcvrFskNoModTx();
#endif
    }
    else
    {
        /* Restore channel and Tx Power */
#if (NXP_RADIO_GEN >= 350)
        XCVR_DftTxOff();
#else
        XcvrFskIdle();
        XCVR_ReleasePLLOverride();
#endif
        XCVR_ReleasePAPower();
    }
}

/*! *********************************************************************************
* \brief        Translate given data to hex.
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
* \param[in]    to_4bits   Translation function
********************************************************************************** */
void cli_sprint_hex4b(uint8_t *dataOut,
                      uint8_t *dataIn,
                      uint32_t units,
                      uint32_t (* to_4bits)(uint8_t **dataIn))
{
    uint32_t u;
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        uint32_t val;

        if (to_4bits == NULL) {
            val = (*dataIn) & 0xFU;
            /* Default implementation assumes dataIn as uint8_t array */
            dataIn++;
        } else{
            /* dataIn managed by translation fuinction */
            val = to_4bits(&dataIn);
        }
        *dataOut++ = (uint8_t)hexchar[val&0xFU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Translate given data to hex (reverse order).
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
********************************************************************************** */
void cli_sprint_hex8b_reverse(uint8_t *dataOut, uint8_t *dataIn, uint32_t units)
{
    uint32_t u;
    if(dataOut == NULL)
    {
        return;
    }
    dataOut += (units<<1);
    *dataOut-- = 0;
    for(u = 0; u < units; u++)
    {
        *dataOut-- = (uint8_t)hexchar[(*dataIn)&0xFU];
        *dataOut-- = (uint8_t)hexchar[(*dataIn)>>4U];
        dataIn++;
    }
}

/*! *********************************************************************************
* \brief        Translate given data to hex or apply given translation function.
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
* \param[in]    convert     Translate function to apply to data
********************************************************************************** */
void cli_sprint_hex8b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint32_t units,
    uint32_t (* convert)(uint8_t **dataIn)
)
{
    uint32_t u;
    uint8_t val;
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        if (convert != NULL) {
            val = (uint8_t)convert(&dataIn);
        } else {
            val = *dataIn;
            dataIn++;
        }
        *dataOut++ = (uint8_t)hexchar[val>>4U];
        *dataOut++ = (uint8_t)hexchar[val&0xFU];
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
void cli_sprint_hex8b_c(uint8_t *dataOut, uint8_t *dataIn, uint32_t units, uint32_t incr)
{
    uint32_t u;
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        *dataOut++ = (uint8_t)hexchar[(*dataIn)>>4U];
        *dataOut++ = (uint8_t)hexchar[(*dataIn)&0xFU];
        dataIn += incr;
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Translate given data to hex in chunks of 2 bytes (reverse order).
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer
* \param[in]    units       Data length to translate
********************************************************************************** */
void cli_sprint_hex16b(uint8_t *dataOut, uint8_t *dataIn, uint32_t units, uint16_t (* convert)(uint8_t **dataIn))
{
    uint32_t u;
    uint16_t val;
    union {
        uint16_t *v16;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = dataIn;
    
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        if (convert != NULL) 
        {
            val = convert(&tmp.v8);
        } 
        else 
        {
            val = *(tmp.v16);
            tmp.v8 += 2;
        }
        *dataOut++ = (uint8_t)hexchar[(val>>12)&0xFU];
        *dataOut++ = (uint8_t)hexchar[(val>>8)&0xFU];
        *dataOut++ = (uint8_t)hexchar[(val>>4)&0xFU];
        *dataOut++ = (uint8_t)hexchar[(val)&0xFU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Print 12 bits in 2 bytes base64 format - 'units' times
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer (32 bits MSB first)
* \param[in]    units       Data length to translate
* \param[in]    to_12bits   Translation function
********************************************************************************** */
void cli_sprint_base64_12b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint32_t units,
    uint32_t (* to_12bits)(uint8_t **dataIn)
)
{
    uint32_t u;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = dataIn;
    
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        uint32_t val;

        if (to_12bits == NULL) {
            /* Default implementation assumes dataIn as uint32_t array */
            val = (*(tmp.v32)) & 0xFFFFU;
            tmp.v8 += sizeof(uint32_t);
        } else{
            val = to_12bits(&tmp.v8);
        }
        *dataOut++ = (uint8_t)basechar[(val>>6)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val)&0x3FU];
    }
    *dataOut = 0;
}

/*! *********************************************************************************
* \brief        Print 24 bits in 4 bytes base64 format - 'units' times
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer (32 bits MSB first)
* \param[in]    units       Data length to translate
* \param[in]    to_24bits   Translation function
********************************************************************************** */
void cli_sprint_base64_24b
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
    
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        uint32_t val;

        if (to_24bits == NULL) {
            /* Default implementation assumes dataIn as uint32_t array */
            val = (*(tmp.v32)) & 0xFFFFFFU;
            tmp.v8 += sizeof(uint32_t);
        } else{
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
* \brief        Print 30 bits in 5 chars base64 format - 'units' time
*
* \param[in]    dataOut     Pointer to destination buffer
* \param[in]    dataIn      Pointer to source buffer (32 bits MSB first, 2MSB bits are droped)
* \param[in]    units       Data length to translate
* \param[in]    to_30bits   Translation function
********************************************************************************** */
void cli_sprint_base64_30b
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint16_t units,
    uint32_t (* to_30bits)(uint8_t **dataIn)
)
{
    uint32_t u;
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
    {
        uint32_t val;

        assert(to_30bits != NULL);
        val = to_30bits(&dataIn);

        *dataOut++ = (uint8_t)basechar[(val>>24)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val>>18)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val>>12)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val>>6)&0x3FU];
        *dataOut++ = (uint8_t)basechar[(val)&0x3FU];
    }
    *dataOut = 0;
}

/* Convert HARTT stat to 30 bits
 */
/*! *********************************************************************************
* \brief        Convert RTT stat to 30 bits
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_rttstat_to_30bits(uint8_t **dataIn) {
    uint32_t val;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = *dataIn;

    val = *(tmp.v32);
    val &= 0x3FFFFFFFU;
    /* To next record */
    *dataIn += sizeof(rtt_internal_t);
    return val;
}

/*! *********************************************************************************
* \brief        Convert IQ from 4 bytes to 3 bytes
*               The exported value will have to be: offsetted by -2048 and shifted
*               right by 1 bit in order to retreive the original 11 bits signed value.
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_iq_to_24bits(uint8_t **dataIn) {
    uint32_t iq;
    uint32_t val;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = *dataIn;

    val = *(tmp.v32);

    /* HW output is :{rx_dft_iq_out_q[10:0], rx_if_mixer_idx[9:5], rx_dft_iq_out_i[10:0], rx_if_mixer_idx[4:0]} */
    iq = ((val & 0xffe0U) << 8) | ((val & 0xffe00000U) >> 20); /* align to 2x12bit, I: MSB and Q LSB*/
    iq ^= 0x800800U; /* invert sign bits (offset) for easier encoding/decoding */
    *dataIn += IQ_SIZE_MEM;
    return iq;
}

/*! *********************************************************************************
* \brief        Extract if_mixer_idx (10 bits) from 4 bytes IQ - stored on 12bits MSB
*               The exported value will have to be: offsetted by -2048 and shifted
*               right by 2 bits in order to retreive the original 10 bits signed value
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_iq_to_rx_if_mixer_idx_12bits(uint8_t **dataIn) {
    uint32_t idx;
    uint32_t val;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = *dataIn;

    val = *(tmp.v32);
    /* HW output is :{rx_dft_iq_out_q[10:0], rx_if_mixer_idx[9:5], rx_dft_iq_out_i[10:0], rx_if_mixer_idx[4:0]} */
    idx = ((val & 0x001f0000U) >> 9) | ((val & 0x0000001fU) << 2);
    idx ^= 0x800U; /* invert sign bit (offset) for easier encoding decoding */
    *dataIn += IQ_SIZE_MEM;
    return idx;
}

/*! *********************************************************************************
* \brief        Convert RTT record from 4 bytes record to 3 bytes value
*               Packet quality is encoded in 4bits MSB
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_rtt_to_24bits(uint8_t **dataIn) {
    uint32_t val;
    uint8_t *ptr;

    /* Skip NADM & RSSI */
    (*dataIn) += CS_NADM_SIZE + CS_RSSI_SIZE;
    /* dataIn not aligned on uint32_t boundary, cannot cast */
    ptr = *dataIn;
    val = ((uint32_t)ptr[2])<<16 | ((uint32_t)ptr[1])<<8 | (uint32_t)ptr[0];
    *dataIn += CS_TS_SIZE;
    return val;
}

/*! *********************************************************************************
* \brief        Convert NADM record from 1 byte record to 4 bits value
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_nadm_to_4bits(uint8_t **dataIn) {
    uint32_t val;
    uint8_t *ptr;

    ptr = *dataIn;
    /* Compress data from 8 bits to 4 bits. First 4 MSB bits are not used (Unknown NADM 0xFF value will be compressed to 0x0F) */
    val = (*ptr & 0x0F);
    *dataIn += CS_NADM_SIZE + CS_RSSI_SIZE + CS_TS_SIZE;
    return val;
}

/*! *********************************************************************************
* \brief        Convert CFO from 4 bytes record to 3 bytes value
*               The exported value will have to be: offsetted by -2^23 and shifted
*               right by 4 bits in order to retreive the original 20 bits signed value
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_cfo_to_24bits(uint8_t **dataIn) {
    uint32_t val;
    union {
        uint32_t *v32;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = *dataIn;
    /* Skip RSSI */
    (*tmp.v8)++;
    /* Encode 3 bytes for CFO */
    val = *(tmp.v32);
    val <<= 4;
    val ^= 0x800000U; /* invert sign bit (offset) for easier encoding/decoding */
    *tmp.v8 += 3U;
    return val;
}

/*! *********************************************************************************
* \brief        Convert CFO from 2 bytes
*               swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint16_t convert_cfo(uint8_t **dataIn) {
    int16_t val;
    union {
        uint16_t *v16;
        uint8_t *v8;
    } tmp = {0U};
    
    tmp.v8 = *dataIn;
    /* Skip RSSI */
    tmp.v8++;
    /* Encode 2 bytes for CFO (15bits) */
    val = *(tmp.v16);
    /* from SigSpec:
       Frequency compensation value in units of 0.01 ppm (15-bit signed integer)
       Range: -100 ppm (-0xD8F0) to +100 ppm (+0x2710)
     */
    /* Propagate sign bit from 15 bits to 16 bits */
    val <<= 1; val /= 2;
    val ^= 0x8000U; /* invert sign bit (offset) for easier encoding/decoding */
    
    (*dataIn) += 3;
    return val;
}

/*! *********************************************************************************
* \brief        RSSI is signed, swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_rssi_mode0(uint8_t **dataIn) {
    uint8_t val;

    val = (**dataIn) ^ 0x80U;
    (*dataIn) += 3; /* Skip 2 bytes (CFO) */
    return val;
}

/*! *********************************************************************************
* \brief        RSSI is signed, swap sign bit for easier decoding on CLI
*
* \param[in]    dataIn      Pointer to source buffer
********************************************************************************** */
uint32_t convert_rssi(uint8_t **dataIn) {
    uint8_t val;

    /* Skip NADM */
    (*dataIn) += CS_NADM_SIZE;
    val = (**dataIn) ^ 0x80U;
    (*dataIn) += CS_RSSI_SIZE + CS_TS_SIZE;  /* Skip RSSI & TS */
    return val;
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
void cli_sprint_base64_12b_c
(
    uint8_t *dataOut,
    uint8_t *dataIn,
    uint32_t units,
    uint8_t incr,
    uint8_t shift
)
{
    uint32_t u;
    uint32_t val = 0U;
    if(dataOut == NULL)
    {
        return;
    }
    for(u = 0; u < units; u++)
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
* \brief        Utility printf function for variable arguments
*
* \param[in]    fmt_s    Formatting string
********************************************************************************** */
void cli_printf(const char *fmt_s, ...)
{
    va_list args;

    va_start (args, fmt_s);
    (void)vprintf (fmt_s, args);
    va_end (args);
}

/*! *********************************************************************************
* \brief        Utility printf function for variable arguments
*
* \param[in]    level    Verbosity level
* \param[in]    fmt_s    Formatting string
********************************************************************************** */
void cli_dprintf(uint8_t level, const char *fmt_s, ...)
{
    va_list args;

    if((gLocalSettings->verbosity & level) == 0U)
    {
        return;
    }
    if((level & (uint8_t)eVerbosityDebug) != 0U)
    {
        (void)printf("DBG: ");
    }
    va_start (args, fmt_s);
    (void)vprintf (fmt_s, args);
    va_end (args);
}

/************************************************************************************
 *************************************************************************************
 *  Private functions
 *************************************************************************************
 ************************************************************************************/
/*! *********************************************************************************
* \brief        Print a fixedpoint uint32_t
*
* \param[in]    label           Label
* \param[in]    qNum            Number to print
* \param[in]    fpPrecision     Number of fractionary bits
* \param[in]    printPrecision  Number of fractional digit expected in the output string
********************************************************************************** */
static void mciq_print_fp_labelled
(
    char *label,
    int32_t qNum,
    uint32_t fpPrecision,
    uint32_t printPrecision
)
{
    uint32_t qInt;
    uint32_t printPrecisionMultiplier = 1U;
    uint32_t qNumUn;
    uint64_t qFrac;
    int qSign;
    uint32_t fracMask = 0xFFFFFFFFU >> ((sizeof(uint32_t)*8U) - fpPrecision);

    for (uint32_t i=0; i<printPrecision; i++) {
        printPrecisionMultiplier *= 10U;
    }

    if (qNum >= 0)
    {
        qSign = 1;
    }
    else
    {
        qSign = -1;
        /* Get 2's complement (absolute value of number) */
        qNumUn = (0xFFFFFFFFUL ^ (uint32_t)qNum) + 1U;
        qNum = (int32_t)qNumUn;
    }
    qInt = (uint32_t)qNum >> fpPrecision;
    qFrac = ((uint64_t)printPrecisionMultiplier *((uint64_t)qNum & (uint64_t)fracMask)) >> (uint64_t)fpPrecision;

    (void)printf("%s%s%d.%0*u", (label != NULL)?label:"", (qSign > 0)?"":"-", qInt, (int)printPrecision, (uint32_t)qFrac);
}

/*! *********************************************************************************
* \brief        Print postprocessing information
*
* \param[in]    result      Result node
* \param[in]    n_pairs     Number of pairs
* \param[in]    algo_flags  Algorithm flags
********************************************************************************** */
static void mciq_print_node_postprocessing
(
    mciq_result_t *result,
    uint32_t n_pairs,
    uint8_t algo_flags
)
{
    (void)printf("result:{vf:%u", result->nb_valid_freq);
    if ((algo_flags & eMciqAlgoEmbedCDE) != 0U)
    {
        mciq_print_fp_labelled(",cde:", result->cde_fp, 10, 2);
        mciq_print_fp_labelled(",cqi:", result->cde_dqi, 14, 3);

        /* If debug infos are requested */
        if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementDebug) != 0U) && (n_pairs > 1U))
        {
            uint32_t m;
            char str_tmp[10];

            for (m=0; m<n_pairs; m++)
            {
                (void)sprintf(str_tmp, ",dcde%d:", m);
                mciq_print_fp_labelled(str_tmp, result->cde_dist_array[m], 10, 2);
                (void)sprintf(str_tmp, ",dcqi%d:", m);
                mciq_print_fp_labelled(str_tmp, result->cde_dqi_array[m], 14, 3);
                (void)printf(",dvf%d:%u", m, result->cde_nb_valid[m]);
            }
        }
    }

    if ((algo_flags & eMciqAlgoEmbedRADE) != 0U)
    {
        int integ, decimal; /* compute printf float manually to avoid linking "large" printf lib */
        float integFl;

        integ = (int)result->rade_dist;
        integFl = (result->rade_dist - (float)integ) * 100.0f;
        decimal = (int)integFl;
        if (integ < 0)
        {
          decimal *= (-1);
        }
        (void)printf(",rade:%d.%02d", integ, decimal);
        
        integ = (int)result->rade_dist_trk;
        integFl = (result->rade_dist_trk - (float)integ) * 100.0f;
        decimal = (int)integFl;
        if (integ < 0)
        {
          decimal *= (-1);
        }
        (void)printf(",rade_trk:%d.%02d", integ, decimal);
        
        integ = (int)result->rade_dqi;
        integFl = (result->rade_dqi - (float)integ) * 1000.0f;
        decimal = (int)integFl;
        if (integ < 0)
        {
          decimal *= (-1);
        }
        (void)printf(",rade_dqi:%d.%03d", integ, decimal);
        (void)printf(",raderr:%d", result->rade_error_flag);
    }
    (void)printf("},");
}

/*! *********************************************************************************
* \brief        Print IQ node data
*
* \param[in]    mparams     Measurement parameters
* \param[in]    data        IQ data
********************************************************************************** */
static void mciq_print_node_data(measurement_parameters_t *mparams, mciq_data_t *data)
{
    uint8_t m, n_ap;
    uint8_t *pBuffer = MEM_BufferAlloc(2 * data->nbSteps + 4);

    if(pBuffer == NULL)
        return;

    n_ap = data->n_ap;
    (void)printf("i:[");
    for(m = 0; m < n_ap; m++)
    {
        /* CS IQs are 2 * 11 bits compressed, I is located in 12bits MSB, all antenna paths packed by step */
        cli_sprint_base64_12b_c(pBuffer, &data->iq[(IQ_SIZE+TQI_SIZE)*m], data->nbSteps, (IQ_SIZE+TQI_SIZE)*n_ap, 12);
        (void)printf("'%s',", (char*)pBuffer);
    }
    (void)printf("],q:[");
    for(m = 0; m < n_ap; m++)
    {
        /* CS IQs are 2 * 12 bits, Q is located in 12bits LSB, all antenna paths packed by step */
        cli_sprint_base64_12b_c(pBuffer, &data->iq[(IQ_SIZE+TQI_SIZE)*m], data->nbSteps, (IQ_SIZE+TQI_SIZE)*n_ap, 0);
        (void)printf("'%s',", (char*)pBuffer);
    }
    (void)printf("],tqi:["); /* Tone quality indicator, byte */
    for(m = 0; m < n_ap; m++)
    {
        cli_sprint_hex8b_c(pBuffer, &data->iq[(IQ_SIZE+TQI_SIZE)*m+IQ_SIZE], data->nbSteps, (IQ_SIZE+TQI_SIZE)*n_ap);
        (void)printf("'%s',", (char*)pBuffer);
    }
    MEM_BufferFree(pBuffer);
    (void)printf("],");
}

/*! *********************************************************************************
* \brief        Print IQ measurement data
*
* \param[in]    mparams             Measurement parameters
* \param[in]    mciq_data_init      Initiator data
* \param[in]    mciq_data_refl      Reflector data
* \param[in]    engine_response     Engine response
********************************************************************************** */
static void cli_mciq_measurement_print
(
    measurement_parameters_t *mparams,
    mciq_data_t *mciq_data_init,
    mciq_data_t *mciq_data_refl,
    engine_response_t *engine_response
)
{
    mciq_data_t *mdata;
    uint32_t nb_steps, n_ap;

    /* In test mode, one of reflector or intiator nbSteps is null, keep the relevant one */
    nb_steps = (mciq_data_init->nbSteps != 0U) ? mciq_data_init->nbSteps : mciq_data_refl->nbSteps;
    n_ap = (mciq_data_init->n_ap != 0U) ? mciq_data_init->n_ap : mciq_data_refl->n_ap;

    mdata = mciq_data_init;
    (void)printf("mciq:{cfg:{n_ap:%u,n_stp:%u},", n_ap, nb_steps);
#ifdef INCLUDE_EMBEDDED_COMPUTATION_SUPPORT
    if (gLocalSettings->mciq_algo_flags != 0U) {
        if (engine_response->is_valid) {
            mciq_print_node_postprocessing(&engine_response->mciq_result, n_ap, gLocalSettings->mciq_algo_flags);
        }
    }
#endif /* INCLUDE_EMBEDDED_COMPUTATION_SUPPORT */

    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (mdata->nbSteps != 0U)) {
        (void)printf("init:{");
        mciq_print_node_data(mparams, mdata);
        (void)printf("},");
    }

    mdata = mciq_data_refl;
    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (mdata->nbSteps != 0U)) {
        (void)printf("refl:{");
        mciq_print_node_data(mparams, mdata);
        (void)printf("},");
    }

    (void)printf("},");
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

    if (fp > 0) {
      sign = 1;
    } else {
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
* \brief        ToF - print node data
*
* \param[in]    mparams     Measurement parameters
* \param[in]    data        TOF data
********************************************************************************** */
static void tof_print_node_data(measurement_parameters_t *mparams, tof_data_t *data)
{
    uint8_t *pBuffer = MEM_BufferAlloc(4 * (data->nbSteps) + 1);
    if(pBuffer == NULL)
        return;
    /* RTT are 18 bits + 4 error bits (3 bytes), resulting in 4 base64-encoded-bytes */
    cli_sprint_base64_24b(pBuffer, (uint8_t *)data->ts, data->nbSteps, convert_rtt_to_24bits);
    (void)printf("d:'%s',", pBuffer);
    MEM_BufferFree(pBuffer);

    /* RTT RSSI */
    (void)printf("r:");
    pBuffer = MEM_BufferAlloc(2 *  data->nbSteps + 4);
    if (pBuffer == NULL) {
        (void)printf("'NA:oom'");
    } else {
        cli_sprint_hex8b(pBuffer, (uint8_t *)data->ts, data->nbSteps, convert_rssi);
        (void)printf("'%s',", (char*)pBuffer);
        MEM_BufferFree(pBuffer);
    }

    /* Packet NADM */
    (void)printf("nadm:");
    pBuffer = MEM_BufferAlloc(data->nbSteps + 4); /* One byte (4bits) per NADM info */
    if (pBuffer == NULL) {
        (void)printf("'NA:oom'"); /* report out of memory */
    } else {
        cli_sprint_hex4b(pBuffer, (uint8_t *)data->ts, data->nbSteps, convert_nadm_to_4bits);
        (void)printf("'%s'", (char*)pBuffer);
        MEM_BufferFree(pBuffer);
    }
}

/*! *********************************************************************************
* \brief        ToF - print measurement
*
* \param[in]    mparams              Measurement parameters
* \param[in]    tof_data_init        TOF data - initiator
* \param[in]    tof_data_refl        TOF data - reflector
* \param[in]    engine_response      Engine response
********************************************************************************** */
static void cli_tof_measurement_print
(
    measurement_parameters_t *mparams,
    tof_data_t *tof_data_init,
    tof_data_t *tof_data_refl,
    engine_response_t *engine_response
)
{
    tof_data_t *tdata;
    uint32_t nb_steps;

    /* In test mode, one of reflector or intiator nbSteps is null, keep the relevant one */
    nb_steps = (tof_data_init->nbSteps != 0U) ? tof_data_init->nbSteps:tof_data_refl->nbSteps;

    tdata = tof_data_init;
    (void)printf("tof:{cfg:{n_stp:%d},", nb_steps);

    if (engine_response->is_valid) {
        tof_print_node_decision(engine_response->tof_result.dm_ad, engine_response->tof_result.dm_sr);
    }

    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (tdata->nbSteps != 0U)) {
        (void)printf("init:{");
        tof_print_node_data(mparams, tdata);
        (void)printf("},");
    }

    tdata = tof_data_refl;
    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (tdata->nbSteps != 0U)) {
        (void)printf("refl:{");
        tof_print_node_data(mparams, tdata);
        (void)printf("},");
    }
    (void)printf("},");
}

/*! *********************************************************************************
* \brief        Print mode0 data
*
* \param[in]    mparams          Measurement parameters
* \param[in]    cs_data        CS data
* \param[in]    data             CS mode0 data
********************************************************************************** */
static void mode0_print_node_data
(
    measurement_parameters_t *mparams,
    cs_data_t *cs_data,
    cs_mode0_data_t *data
)
{
    uint8_t *pBuffer;

    /* Mode0 RSSI */
    (void)printf("r:");
    pBuffer = MEM_BufferAlloc(2 * cs_data->mode0_nb + 4);
    if (pBuffer == NULL) {
        (void)printf("'NA:oom'");
    } else {
        cli_sprint_hex8b(pBuffer, data->mode0_buffer, cs_data->mode0_nb, convert_rssi_mode0);
        (void)printf("'%s'", (char*)pBuffer);
        MEM_BufferFree(pBuffer);
    }
    (void)printf(",");

    /* Mode0 CFO */
    (void)printf("c:");
    pBuffer = MEM_BufferAlloc(4 * cs_data->mode0_nb + 4);
    if (pBuffer == NULL) {
        (void)printf("'NA:oom'");
    } else {
        cli_sprint_hex16b(pBuffer, data->mode0_buffer, cs_data->mode0_nb, convert_cfo);
        (void)printf("'%s'", (char*)pBuffer);
        MEM_BufferFree(pBuffer);
    }
}

/*! *********************************************************************************
* \brief        Print measurement data
*
* \param[in]    mparams              Measurement parameters
* \param[in]    mode0_data_init      Mode0 data - initiator
* \param[in]    mode0_data_refl      Mode0 data - reflector
* \param[in]    cs_data              CS data
********************************************************************************** */
static void cli_mode0_measurement_print
(
    measurement_parameters_t *mparams,
    cs_data_t *cs_data,
    cs_mode0_data_t *mode0_data_init,
    cs_mode0_data_t *mode0_data_refl
)
{
    cs_mode0_data_t *data;
    uint32_t nb_steps = cs_data->mode0_nb;

    (void)printf("md0:{cfg:{n_stp:%d},", nb_steps);

    (void)printf("init:{");
    data = mode0_data_init;
    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (data != NULL)) {
        mode0_print_node_data(mparams, cs_data, data);
    }

    (void)printf("},refl:{");
    data = mode0_data_refl;
    if(((gLocalSettings->verbosity & (uint8_t)eVerbosityMeasurementData) != 0U) && (data != NULL)) {
        mode0_print_node_data(mparams, cs_data, data);
    }
    (void)printf("}},");
}

/*! *********************************************************************************
* \brief        Print CS measurement data
*
* \param[in]    mparams              Measurement parameters
* \param[in]    cs_data              CS data
********************************************************************************** */
static void cli_cs_measurement_print
(
    measurement_parameters_t *mparams,
    cs_data_t *cs_data
)
{
    uint8_t *pBuffer;

    (void)printf("cs:{");

    /* Configuration */
    (void)printf("cfg:{");
    (void)printf("rtyp:%u,rphy:%u,txpwr:%d,", mparams->cfg.rtt_type, mparams->cfg.rtt_phy, mparams->cfg.tx_pwr);
    (void)printf("fcs:%u,ip1:%u,ip2:%u,tpm:%u,ant:%d",
                 mparams->cfg.t_fcs, mparams->cfg.t_ip1, mparams->cfg.t_ip2, mparams->cfg.t_pm, mparams->cfg.ant_cfg_index);
    (void)printf("},");

    /* Status */
    (void)printf("sts:%u,", cs_data->status);

    if (cs_data->status == 0x00)
    {
        /* CS Steps */
        (void)printf("stp:{nb:%u,", cs_data->step_nb);

        /* Modes */
        (void)printf("md:");
        pBuffer = MEM_BufferAlloc(cs_data->step_nb + 4);
        if (pBuffer == NULL) {
            (void)printf("'NA:oom'");
        } else {
            cli_sprint_hex4b(pBuffer, cs_data->modeMap, cs_data->step_nb, NULL);
            (void)printf("'%s'", (char*)pBuffer);
            MEM_BufferFree(pBuffer);
        }
        (void)printf(",");

        /* Channels */
        (void)printf("ch:");
        pBuffer = MEM_BufferAlloc(2 * cs_data->step_nb + 4);
        if (pBuffer == NULL) {
            (void)printf("'NA:oom'");
        } else {
            cli_sprint_hex8b(pBuffer, cs_data->channelMap, cs_data->step_nb, NULL);
            (void)printf("'%s'", (char*)pBuffer);
            MEM_BufferFree(pBuffer);
        }
        (void)printf(",");

        /* Event counter for each subevent */
        (void)printf("evt:");
        pBuffer = MEM_BufferAlloc(2*cs_data->subevt_nb + 4);
        if (pBuffer == NULL) {
            (void)printf("'NA:oom'");
        } else {
            cli_sprint_hex8b(pBuffer, cs_data->subevtConnEvent, cs_data->subevt_nb, NULL);
            (void)printf("'%s'", (char*)pBuffer);
            MEM_BufferFree(pBuffer);
        }
        (void)printf(",");

        /* Subevt */
        (void)printf("se:");
        pBuffer = MEM_BufferAlloc(2*cs_data->subevt_nb + 4);
        if (pBuffer == NULL) {
            (void)printf("'NA:oom'");
        } else {
            cli_sprint_hex8b(pBuffer, cs_data->subevtStopIdxLocal, cs_data->subevt_nb, NULL);
            (void)printf("'%s'", (char*)pBuffer);
            MEM_BufferFree(pBuffer);
        }

        (void)printf("}");
    }

    (void)printf("},");
}

/*! *********************************************************************************
* \brief        Print CS debug data
*
* \param[in]    mparams              Measurement parameters
* \param[in]    debugBuffer          Debug data
* \param[in]    internal_data        Internal data
********************************************************************************** */
static void cli_debug_measurement_print
(
    measurement_parameters_t *mparams,
    debug_data_t *debugBuffer,
    event_internal_data_t *internal_data
)
{
    uint8_t *pBuffer;

    if (debugBuffer) {
        (void)printf("s:%u,p:%u,sps:%u,", debugBuffer->step_nb, debugBuffer->n_pairs, debugBuffer->samplesPerStep);

        /* Detailed IQ obtained from DBG RAM DMA to CM33 */
        uint8_t *iq = debugBuffer->iq;
        if (iq != NULL) {
            (void)printf("iq:[");
            for(int m = 0; m < debugBuffer->step_nb; m++)
            {
                pBuffer = MEM_BufferAlloc(4 * debugBuffer->samples_nb[m] + 4);
                if (pBuffer == NULL) {
                    (void)printf("'NA:oom',");
                } else {
                    /* RAW IQs are signed 11 bits, aligned on 2 bytes MSB boundaries - printed on 4 base64 chars each */
                    cli_sprint_base64_24b(pBuffer, iq, debugBuffer->samples_nb[m], convert_iq_to_24bits);
                    (void)printf("'%s',", (char*)pBuffer);
                    iq += debugBuffer->samples_nb[m] * IQ_SIZE_MEM;
                    MEM_BufferFree(pBuffer);
                }
            }
            (void)printf("],");

#ifdef CLI_PRINT_RX_IF_MIXER_IDX
            /* rx_if_mixer_idx */
            iq = debugBuffer->iq;
            printf("ifdx:[");
            for(int m = 0; m < debugBuffer->step_nb; m++)
            {
                pBuffer = MEM_BufferAlloc(2 * debugBuffer->samples_nb[m] + 4);
                if (pBuffer == NULL) {
                    printf("'NA:oom',");
                } else {
                    /* RX IF mixer index are signed 10 bits - printed on 2 base64 chars each */
                    cli_sprint_base64_12b(pBuffer, iq, debugBuffer->samples_nb[m], convert_iq_to_rx_if_mixer_idx_12bits);
                    printf("'%s',", (char*)pBuffer);
                    iq += debugBuffer->samples_nb[m] * IQ_SIZE_MEM;
                    MEM_BufferFree(pBuffer);
                }
            }
            printf("],");
#endif
        }
    }

    /*  Print internal RTT&MCIQ data obtained locally via HCI debug message */
    if ((internal_data != NULL) && (internal_data->rtt_internal != NULL)) {
        (void)printf("rttstat:");
        pBuffer = MEM_BufferAlloc(5 * internal_data->rtt_internal_nb + 4);
        if (pBuffer == NULL) {
            (void)printf("'NA:oom',");
        } else {
            cli_sprint_base64_30b(pBuffer, (uint8_t *)&internal_data->rtt_internal->hartt_stat, internal_data->rtt_internal_nb, convert_rttstat_to_30bits);
            (void)printf("'%s',", (char*)pBuffer);
            MEM_BufferFree(pBuffer);
        }
    }
}

/*! *********************************************************************************
* \brief        Print measurement information
*
* \param[in]    mparams              Measurement parameters
* \param[in]    internal_data        Internal data
********************************************************************************** */
static void cli_info_measurement_print
(
    measurement_parameters_t *mparams,
    event_internal_data_t *internal_data
)
{
    if (internal_data->event_internal != NULL) {
        (void)printf("syn:%d,", internal_data->event_internal->sync_step_id);
        (void)printf("syg:%u,", internal_data->event_internal->sync_rxgain);
        (void)printf("syr:%d,", internal_data->event_internal->sync_rssi);
        (void)printf("syc:%d,", internal_data->event_internal->sync_cfo);
        (void)printf("f:0x%04x,", internal_data->event_internal->flags);
        (void)printf("x:%d,", internal_data->event_internal->xtal_trim);
        (void)printf("ta:%d,", internal_data->event_internal->num_time_adj);
        (void)printf("te:%d,", internal_data->event_internal->temperature);
    }
}

/*! *********************************************************************************
* \brief        Print board address
*
* \param[in]    address     Device address
********************************************************************************** */
static void cli_print_board_identifier(uint8_t *address)
{
    uint8_t pBuffer[34];
    app_version_t app_ver = {0};
    FLib_MemCpy(&app_ver, &c_app_version, sizeof(app_version_t));
    cli_sprint_hex8b_reverse(pBuffer, address, (uint8_t)sizeof(bleDeviceAddress_t));
    /* TODO: revisit calibration process, zdm & zdt */
    (void)printf("v:v%u.%u.%u.%u,zdm:%u,zdt:%u,u:'0x%s'", app_ver.major, app_ver.minor, app_ver.patch, app_ver.build, 0, 0, (char*)pBuffer);
}

/*! *********************************************************************************
* \brief        Print measurement result at console
*
* \param[in]    meas_response     Measurement response parameters
* \param[in]    engine_response   Engine response
* \param[in]    serviceInfo       Service information
********************************************************************************** */
void cli_print_measurement
(
    isp_meas_response_t *meas_response,
    engine_response_t *engine_response,
    wrs_ServiceInfo_t *serviceInfo
)
{
    uint32_t start, diff;
    start = isp_soc_timestamp_get();
    FlagCRCData = 1;
    WrsCRC32 = 0xFFFFFFFF;
    (void)printf("items:[");
    {
        uint8_t uid[6] = {0};

        (void)printf("{");
        if(gLocalSettings->verbosity & eVerbosityBoardInfo) {
            (void)printf("init:{");
            cli_print_board_identifier(local_bid.uid);
            (void)printf("},refl:{");
            cli_print_board_identifier(uid);
            (void)printf("},");
        }

        if(gLocalSettings->verbosity & eVerbosityMeasurementData) {
            cli_cs_measurement_print(meas_response->mparams, meas_response->cs_data);
            cli_mode0_measurement_print(meas_response->mparams, meas_response->cs_data, &meas_response->mode0_data[0], &meas_response->mode0_data[1]);
        }

        if (meas_response->mciq_data[0].iq != NULL) {
            cli_mciq_measurement_print(meas_response->mparams, &meas_response->mciq_data[0], &meas_response->mciq_data[1], engine_response);
        }
        if (meas_response->tof_data[0].ts != NULL) {
            cli_tof_measurement_print(meas_response->mparams, &meas_response->tof_data[0], &meas_response->tof_data[1], engine_response);
        }

        if (gLocalSettings->verbosity & eVerbosityMeasurementInfo) {
            event_internal_data_t *internal_init = meas_response->internal_data[0];
            event_internal_data_t *internal_refl = meas_response->internal_data[1];

            (void)printf("info:{");
            if (internal_init != NULL) {
                (void)printf("init:{");
                cli_info_measurement_print(meas_response->mparams, internal_init);
                (void)printf("},");
            }
            if (internal_refl != NULL) {
                (void)printf("refl:{");
                cli_info_measurement_print(meas_response->mparams, internal_refl);
                (void)printf("},");
            }
            (void)printf("},");
        }

        if (gLocalSettings->verbosity & eVerbosityMeasurementDebug) {
            debug_data_t *debug_init = meas_response->debug_data[0];
            debug_data_t *debug_refl = meas_response->debug_data[1];
            event_internal_data_t *internal_init = meas_response->internal_data[0];
            event_internal_data_t *internal_refl = meas_response->internal_data[1];

            (void)printf("dbg:{");
            if ((debug_init != NULL) && (debug_init->step_nb != 0)) {
                (void)printf("init:{");
                cli_debug_measurement_print(meas_response->mparams, debug_init, internal_init);
                (void)printf("},");
            }
            if (((debug_refl != NULL) && (debug_refl->step_nb != 0)) || (internal_refl != NULL)) {
                (void)printf("refl:{");
                cli_debug_measurement_print(meas_response->mparams, debug_refl, internal_refl);
                (void)printf("},");
            }
            (void)printf("},");
        }

        (void)printf("},");
    }
    (void)printf("]");

    if(gLocalSettings->verbosity & eVerbosityMeasurementProfiling)
    {
        (void)printf("\nprofiling:{");
        (void)printf("t_meas:%lu,", isp_soc_timestamp_difference_us(serviceInfo->timestamps[eTimestampStartMeas], serviceInfo->timestamps[eTimestampStartTransfer]));
        (void)printf("t_transfer:%lu,", isp_soc_timestamp_difference_us(serviceInfo->timestamps[eTimestampStartTransfer], serviceInfo->timestamps[eTimestampStartPostProcessing]));
        (void)printf("t_post:%lu,", isp_soc_timestamp_difference_us(serviceInfo->timestamps[eTimestampStartPostProcessing], serviceInfo->timestamps[eTimestampStop]));
        diff = isp_soc_timestamp_difference_us(start, isp_soc_timestamp_get());
        (void)printf("t_print:%u,", (unsigned int)diff);
        (void)printf("}");
    }
#if(defined(__GNUC__))
    fflush(stdout);
#endif
    FlagCRCData = 0;
    (void)printf("\nCRC32:%08x", WrsCRC32);
    (void)printf("\nmarker:[DONE]\n");
}

/* calculates the CRC-8 checksum of print buffer data */
uint32_t CRC32(uint32_t WrsDataCRC32, const unsigned char *buf, size_t  size)
{
    uint8_t byte;
    for (size_t i = 0; i < size; i++) {
        byte = buf[i];
        WrsDataCRC32 = (WrsDataCRC32 >> 8) ^ crc32_table[((WrsDataCRC32 & 0xFFU) / 2U) ^ byte];
    }
    return WrsDataCRC32;
}
/* EOF */
