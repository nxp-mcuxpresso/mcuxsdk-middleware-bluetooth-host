 /*! *********************************************************************************
 * \defgroup BLE OTAP Custom Profile
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2020, 2022-2023 NXP
*
*
* \file
*
* This file is the interface file for the BLE OTAP Service/Profile
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef OTAP_INTERFACE_H
#define OTAP_INTERFACE_H

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public constants & macros
*************************************************************************************
************************************************************************************/

/*! BLE OTAP Protocol definitions. */
#define gOtap_CmdIdFieldSize_c                      (1U)
#define gOtap_ImageIdFieldSize_c                    (2U)
#define gOtap_ImageVersionFieldSize_c               (8U)
#define gOtap_ChunkSeqNumberSize_c                  (1U)
#define gOtap_MaxChunksPerBlock_c                   (256U)

/*!< ATT_MTU - 3 - 1 - 1 = 23 - 3 - 1 - 1 = 20 - 2 = 18 for ATT Write Without Response */
#define gOtap_AttCommandMtuDataChunkOverhead_c      (3U + gOtap_CmdIdFieldSize_c + gOtap_ChunkSeqNumberSize_c) /*!< 3 for Att Opcode [1] and Attribute Handle [2] - ATT Write Without Response */
#define gOtap_l2capCmdMtuDataChunkOverhead_c        (2U + gOtap_CmdIdFieldSize_c + gOtap_ChunkSeqNumberSize_c) /*!< 2 for the eventual SDU Length */
#define gOtap_ImageChunkDataSizeAttDefault_c        (gAttDefaultMtu_c - gOtap_AttCommandMtuDataChunkOverhead_c)
#define gOtap_ImageChunkDataSizeAttMax_c            (gAttMaxMtu_c - gOtap_AttCommandMtuDataChunkOverhead_c)
#define gOtap_ImageChunkDataSizeL2capCoc_c          (255U)

#define gOtap_L2capLePsm_c                          (0x004FU)

/*! BLE OTAP Image File Format Definitions */

/*! Enumerated type for the recognized image file header versions. */
typedef enum bleOtaFileHeaderVersion_tag
{
    gbleOtapHeaderVersion0100_c                     = 0x0100U,
} bleOtaFileHeaderVersion_t;

#define gBleOtaFileHeaderIdentifier_c               (0x0B1EF11EU)
#define gBleOtaFileHeaderDefaultFieldControl_c      (0x0000U)
#define gBleOtaCompanyIdentifier_c                  (0x01FFU)
#define gBleOtaHeaderStrLength_c                    (32U)

#define gBleOtaImageIdCurrentRunningImage_c         (0x0000U)
#define gBleOtaImageIdNoImageAvailable_c            (0xFFFFU)

/* otapStatus_t */
#define gOtapStatusSuccess_c                        0x00U /*!< The operation was successful. */
#define gOtapStatusImageDataNotExpected_c           0x01U /*!< The OTAP Server tried to send an image data chunk to the OTAP Client but the Client was not expecting it. */
#define gOtapStatusUnexpectedTransferMethod_c       0x02U /*!< The OTAP Server tried to send an image data chunk using a transfer method the OTAP Client does not support/expect. */
#define gOtapStatusUnexpectedCmdOnDataChannel_c     0x03U /*!< The OTAP Server tried to send an unexpected command (different from a data chunk) on a data Channel (ATT or CoC) */
#define gOtapStatusUnexpectedL2capChannelOrPsm_c    0x04U /*!< The selected channel or PSM is not valid for the selected transfer method (ATT or CoC). */
#define gOtapStatusUnexpectedOtapPeer_c             0x05U /*!< A command was received from an unexpected OTAP Server or Client device. */
#define gOtapStatusUnexpectedCommand_c              0x06U /*!< The command sent from the OTAP peer device is not expected in the current state. */
#define gOtapStatusUnknownCommand_c                 0x07U /*!< The command sent from the OTAP peer device is not known. */
#define gOtapStatusInvalidCommandLength_c           0x08U /*!< Invalid command length. */
#define gOtapStatusInvalidCommandParameter_c        0x09U /*!< A parameter of the command was not valid. */
#define gOtapStatusFailedImageIntegrityCheck_c      0x0AU /*!< The image integrity check has failed. */
#define gOtapStatusUnexpectedSequenceNumber_c       0x0BU /*!< A chunk with an unexpected sequence number has been received. */
#define gOtapStatusImageSizeTooLarge_c              0x0CU /*!< The upgrade image size is too large for the OTAP Client. */
#define gOtapStatusUnexpectedDataLength_c           0x0DU /*!< The length of a Data Chunk was not expected. */
#define gOtapStatusUnknownFileIdentifier_c          0x0EU /*!< The image file identifier is not recognized. */
#define gOtapStatusUnknownHeaderVersion_c           0x0FU /*!< The image file header version is not recognized. */
#define gOtapStatusUnexpectedHeaderLength_c         0x10U /*!< The image file header length is not expected for the current header version. */
#define gOtapStatusUnexpectedHeaderFieldControl_c   0x11U /*!< The image file header field control is not expected for the current header version. */
#define gOtapStatusUnknownCompanyId_c               0x12U /*!< The image file header company identifier is not recognized. */
#define gOtapStatusUnexpectedImageId_c              0x13U /*!< The image file header image identifier is not as expected. */
#define gOtapStatusUnexpectedImageVersion_c         0x14U /*!< The image file header image version is not as expected. */
#define gOtapStatusUnexpectedImageFileSize_c        0x15U /*!< The image file header image file size is not as expected. */
#define gOtapStatusInvalidSubElementLength_c        0x16U /*!< One of the sub-elements has an invalid length. */
#define gOtapStatusImageStorageError_c              0x17U /*!< An image storage error has occurred. */
#define gOtapStatusInvalidImageCrc_c                0x18U /*!< The computed CRC does not match the received CRC. */
#define gOtapStatusInvalidImageFileSize_c           0x19U /*!< The image file size is not valid. */
#define gOtapStatusInvalidL2capPsm_c                0x1AU /*!< A block transfer request has been made via the L2CAP CoC method but the specified Psm is not known. */
#define gOtapStatusNoL2capPsmConnection_c           0x1BU /*!< A block transfer request has been made via the L2CAP CoC method but there is no valid PSM connection. */
#define gOtapNumberOfStatuses_c                     0x1CU

/* otapCmdIdt_t */
#define gOtapCmdIdNoCommand_c                       0x00U /*!< No command. */
#define gOtapCmdIdNewImageNotification_c            0x01U /*!< OTAP Server -> OTAP Client - A new image is available on the OTAP Server */
#define gOtapCmdIdNewImageInfoRequest_c             0x02U /*!< OTAP Client -> OTAP Server - The OTAP Client requests image information from the OTAP Server */
#define gOtapCmdIdNewImageInfoResponse_c            0x03U /*!< OTAP Server -> OTAP Client - The OTAP Server sends requested image information to the OTAP Client */
#define gOtapCmdIdImageBlockRequest_c               0x04U /*!< OTAP Client -> OTAP Server - The OTAP Client requests an image block from the OTAP Server */
#define gOtapCmdIdImageChunk_c                      0x05U /*!< OTAP Server -> OTAP Client - The OTAP Server sends an image chunk to the OTAP Client */
#define gOtapCmdIdImageTransferComplete_c           0x06U /*!< OTAP Client -> OTAP Server - The OTAP Client notifies the OTAP Server that an image transfer was completed*/
#define gOtapCmdIdErrorNotification_c               0x07U /*!< Bidirectional - An error has occurred */
#define gOtapCmdIdStopImageTransfer_c               0x08U /*!< Bidirectional - The OTAP Client request the OTAP Server to stop an image transfer. The OTAP Server informs the OTAP Client that the image transfer was stopped. */


/*! BLE OTAP Image File Header */
typedef PACKED_STRUCT bleOtaImageFileHeader_tag
{
  uint32_t  fileIdentifier;                     /*!< 0x0B1EF11E - Magic number to identify an image file as being a BLE upgrade image file. */
  uint16_t  headerVersion;                      /*!< Version of the header. */
  uint16_t  headerLength;                       /*!< Header length. */
  uint16_t  fieldControl;                       /*!< Header field control - shows if any other non-default fields are present in the header.*/
  uint16_t  companyId;                          /*!< 0x01FF - FSL Company Identifier. */
  uint8_t   imageId[gOtap_ImageIdFieldSize_c];  /*!< Should be unique for each image on an OTAP Server. */
  uint8_t   imageVersion[gOtap_ImageVersionFieldSize_c];
  uint8_t   headerString[gBleOtaHeaderStrLength_c];
  uint32_t  totalImageFileSize;                 /*!< Total image file size including the header. */
} bleOtaImageFileHeader_t;

/*! BLE OTAP Image File Sub-element tag id enumeration. */
typedef enum bleOtaImageFileSubElementTagId_tag
{
    /* Reserved sub-element tags */
    gBleOtaSubElemTagIdUpgradeImage_c       = 0x0000,   /*!< Sub-element contains the actual upgrade image. */
    /* Manufacturer specific sub-element tags */
    gBleOtaSubElemTagIdSectorBitmap_c       = 0xF000,   /*!< Sub-element contains the sector bitmap specific to the FLASH memory of a the target device.
                                                         *   The size of this sub-element value is implementation specific. */
    gBleOtaSubElemTagIdImageFileCrc_c       = 0xF100,   /*!< Sub-element contains the CRC of the image file (The CRC does not cover this sub-element)
                                                         *   The size of this sub-element value is implementation specific. */
} bleOtaImageFileSubElementTagId_t;

typedef PACKED_STRUCT subElementHeader_tag
{
  uint16_t tagId;       /*!< Sub-element tag identifier. */
  uint32_t dataLen;     /*!< Sub-element value field length. */
} subElementHeader_t;


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! OTAP Client (Target) - Configuration */
typedef struct otapClientConfig_tag
{
    uint16_t    hService;
} otapClientConfig_t;

/*! OTAP Server (Source) - Configuration */
typedef struct otapServerConfig_tag
{
    uint16_t    hService;
    uint16_t    hControlPoint;
    uint16_t    hControlPointCccd;
    uint16_t    hData;
} otapServerConfig_t;

/*! BLE OTAP Protocol statuses. */
typedef uint8_t otapStatus_t;

/*! OTAP Server storage mode type enumeration. */
typedef enum otapServerStorageMode_tag
{
    gOtapServerStoargeNone_c            = 0x00, /*!< The OTAP Server does not have internal storage and all commands will be relayed between the OTAP Client and the PC (or other serial connected device). */
    gOtapServerStoargeInternal_c        = 0x01, /*!< The OTAP Server has internal storage. It will download an image from a PC (or other serial connected device) and it will then send it to the OTAP Client. */
} otapServerStorageMode_t;

/*! OTAP Transfer Methods type definition */
typedef enum otapTransferMethod_tag
{
    gOtapTransferMethodAtt_c            = 0x00,
    gOtapTransferMethodL2capCoC_c       = 0x01,
} otapTransferMethod_t;

/*! OTAP Protocol Command Id enumeration */
typedef uint8_t otapCmdIdt_t;

/*! OTAP New Image Notification Command */
typedef PACKED_STRUCT otapCmdNewImgNotification_tag
{
    uint8_t     imageId[gOtap_ImageIdFieldSize_c];
    uint8_t     imageVersion[gOtap_ImageVersionFieldSize_c];
    uint32_t    imageFileSize;
} otapCmdNewImgNotification_t;

/*! OTAP New Image Info Request Command */
typedef PACKED_STRUCT otapCmdNewImgInfoReq_tag
{
    uint8_t     currentImageId[gOtap_ImageIdFieldSize_c];
    uint8_t     currentImageVersion[gOtap_ImageVersionFieldSize_c];
} otapCmdNewImgInfoReq_t;

/*! OTAP New Image Info Response Command */
typedef PACKED_STRUCT otapCmdNewImgInfoRes_tag
{
    uint8_t     imageId[gOtap_ImageIdFieldSize_c];
    uint8_t     imageVersion[gOtap_ImageVersionFieldSize_c];
    uint32_t    imageFileSize;
} otapCmdNewImgInfoRes_t;

/*! OTAP Image Block Request Command */
typedef PACKED_STRUCT otapCmdImgBlockReq_tag
{
    uint8_t     imageId[gOtap_ImageIdFieldSize_c];
    uint32_t    startPosition;
    uint32_t    blockSize;
    uint16_t    chunkSize;
    uint8_t     transferMethod;     /*!< otapTransferMethod_t - ATT or L2CAP Credit Oriented Channels (CoC) */
    uint16_t    l2capChannelOrPsm;
} otapCmdImgBlockReq_t;

/*! OTAP Image Chunk Command - for ATT transfer method only. */
typedef PACKED_STRUCT otapCmdImgChunkAtt_tag
{
    uint8_t     seqNumber;  /*!< The sequence number of the sent chunk. Max 256 chunks per block. */
    uint8_t     data[gOtap_ImageChunkDataSizeAttMax_c];
} otapCmdImgChunkAtt_t;

/*! OTAP Image Chunk Command - for CoC transfer method only. */
typedef PACKED_STRUCT otapCmdImgChunkCoc_tag
{
    uint8_t     seqNumber;  /*!< The sequence number of the sent chunk. Max 256 chunks per block. */
    uint8_t     data[gOtap_ImageChunkDataSizeL2capCoc_c];
} otapCmdImgChunkCoc_t;

/*! OTAP Image Transfer Complete Command */
typedef PACKED_STRUCT otapCmdImgTransferComplete_tag
{
    uint8_t     imageId[gOtap_ImageIdFieldSize_c];
    uint8_t     status;     /*!< otapStatus_t */
} otapCmdImgTransferComplete_t;

/*! OTAP Error Notification Command */
typedef PACKED_STRUCT otapErrNotification_tag
{
    uint8_t     cmdId;      /*!< otapCmdIdt_t - The command which caused the error. */
    uint8_t     errStatus;  /*!< otapStatus_t */
} otapErrNotification_t;

/*! OTAP Stop Image Transfer Command */
typedef PACKED_STRUCT otapCmdStopImgTransfer_tag
{
    uint8_t     imageId[gOtap_ImageIdFieldSize_c];
} otapCmdStopImgTransfer_t;

/*! General OTAP Command type structure.
 *  The image chunk commands are not included here. Their length is variable. */
typedef PACKED_STRUCT otapCommand_tag
{
    uint8_t   cmdId;
    PACKED_UNION
    {
        /* Pairing Methods */
        otapCmdNewImgNotification_t         newImgNotif;
        otapCmdNewImgInfoReq_t              newImgInfoReq;
        otapCmdNewImgInfoRes_t              newImgInfoRes;
        otapCmdImgBlockReq_t                imgBlockReq;
        otapCmdImgTransferComplete_t        imgTransComplete;
        otapErrNotification_t               errNotif;
        otapCmdStopImgTransfer_t            stopImgTransf;
        otapCmdImgChunkCoc_t                imgChunkCoc;               
    } cmd;
} otapCommand_t;

typedef union cmd_tag
{
    /* Pairing Methods */
    otapCmdNewImgNotification_t         newImgNotif;
    otapCmdNewImgInfoReq_t              newImgInfoReq;
    otapCmdNewImgInfoRes_t              newImgInfoRes;
    otapCmdImgBlockReq_t                imgBlockReq;
    otapCmdImgTransferComplete_t        imgTransComplete;
    otapErrNotification_t               errNotif;
    otapCmdStopImgTransfer_t            stopImgTransf;
}cmd_t;

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/

#define gOtapCmdImageChunkCocMaxLength_c   (sizeof(otapCmdImgChunkCoc_t) + gOtap_CmdIdFieldSize_c)
#define gOtapCmdImageChunkAttMaxLength_c   (gOtap_ImageChunkDataSizeAttMax_c + gOtap_ChunkSeqNumberSize_c + gOtap_CmdIdFieldSize_c)

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*!**********************************************************************************
* \brief        Starts the OTAP Client Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t OtapCS_Start(otapClientConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Stops the OTAP Client Service functionality
*
* \param[in]    pServiceConfig  Pointer to structure that contains server
*                               configuration information.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t OtapCS_Stop(otapClientConfig_t *pServiceConfig);

/*!**********************************************************************************
* \brief        Subscribes a GATT client to the OTAP Client Service
*
* \param[in]    pClient  Client Id in Device DB.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t OtapCS_Subscribe(deviceId_t clientDeviceId);

/*!**********************************************************************************
* \brief        Unsubscribes a GATT client from the OTAP Client Service
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t OtapCS_Unsubscribe(void);

/*!**********************************************************************************
* \brief        Sends a command to the OTAP Server via ATT indications if the server
*               has enabled indications for the proper attribute.
*
* \param[in]    serviceHandle   Service handle.
* \param[in]    pCommand        Pointer to the OTAP command.
* \param[in]    cmdLength       Length of the command.
*
* \return       gBleSuccess_c or error.
************************************************************************************/
bleResult_t OtapCS_SendCommandToOtapServer (uint16_t serviceHandle,
                                            void* pCommand,
                                            uint16_t cmdLength);

//bleResult_t Tms_RecordTemperatureMeasurement (uint16_t serviceHandle, int16_t temperature);

#ifdef __cplusplus
}
#endif

#endif /* OTAP_INTERFACE_H */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
