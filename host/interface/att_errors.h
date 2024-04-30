/*! *********************************************************************************
 * \addtogroup GATT
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2015 Freescale Semiconductor, Inc.
* Copyright 2016-2019, 2021, 2023 NXP
*
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef ATT_ERRORS_H
#define ATT_ERRORS_H

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! ATT error codes */
typedef enum
{
    gAttErrCodeNoError_c                        = 0x00U,                /*!< No error has occured. */
    gAttErrCodeInvalidHandle_c                  = 0x01U,                /*!< The attribute handle given was not valid on this server. */
    gAttErrCodeReadNotPermitted_c               = 0x02U,                /*!< The attribute cannot be read. */
    gAttErrCodeWriteNotPermitted_c              = 0x03U,                /*!< The attribute cannot be written. */
    gAttErrCodeInvalidPdu_c                     = 0x04U,                /*!< The attribute PDU was invalid. */
    gAttErrCodeInsufficientAuthentication_c     = 0x05U,                /*!< The attribute requires authentication before it can be read or written. */
    gAttErrCodeRequestNotSupported_c            = 0x06U,                /*!< ATT Server does not support the request received from the client. */
    gAttErrCodeInvalidOffset_c                  = 0x07U,                /*!< Offset specified was past the end of the attribute. */
    gAttErrCodeInsufficientAuthorization_c      = 0x08U,                /*!< The attribute requires authorization before it can be read or written. */
    gAttErrCodePrepareQueueFull_c               = 0x09U,                /*!< Too many prepare writes have been queued. */
    gAttErrCodeAttributeNotFound_c              = 0x0AU,                /*!< No attribute found within the given attribute handle range. */
    gAttErrCodeAttributeNotLong_c               = 0x0BU,                /*!< The attribute cannot be read using the ATT_READ_BLOB_REQ PDU. */
    gAttErrCodeInsufficientEncryptionKeySize_c  = 0x0CU,                /*!< The Encryption Key Size used for encrypting this link is too short. */
    gAttErrCodeInvalidAttributeValueLength_c    = 0x0DU,                /*!< The attribute value length is invalid for the operation. */
    gAttErrCodeUnlikelyError_c                  = 0x0EU,                /*!< The attribute request could not be completed due to an unlikely error. */
    gAttErrCodeInsufficientEncryption_c         = 0x0FU,                /*!< The attribute requires encryption before it can be read or written. */
    gAttErrCodeUnsupportedGroupType_c           = 0x10U,                /*!< The attribute type is not a supported grouping attribute. */
    gAttErrCodeInsufficientResources_c          = 0x11U,                /*!< Insufficient Resources to complete the request. */
    gAttErrCodeDatabaseOutOfSync_c              = 0x12U,                /*!< The server requests the client to rediscover the database. */
    gAttErrCodeValueNotAllowed_c                = 0x13U,                /*!< The attribute parameter value was not allowed. */
    /* Reserved Error Opcodes                   =    0x14 - 0x7F */
    /* Application Error Opcodes                =    0x80 - 0x9F */
    /* Reserved Error Opcodes                   =    0xA0 - 0xDF */
    /* Common Profile And Service Error Opcodes =    0xE0 - 0xFF */
    /*     Reserved for Future Use Opcodes      =    0xE0 - 0xFB */
    gAttErrCodeWriteRequestRejected_c           = 0xFCU,                /*!< Write request cannot be fulfilled for reasons other than permissions. */
    gAttErrCodeCccdImproperlyConfigured_c       = 0xFDU,                /*!< CCCD is not configured according to the requirements of the profile or service. */
    gAttErrCodeProcedureAlreadyInProgress_c     = 0xFEU,                /*!< Profile or service request cannot be serviced because another operation is still in progress.*/
    gAttErrCodeOutOfRange_c                     = 0xFFU,                /*!< Attribute value is out of range as defined by a profile or service specification. */
} attErrorCode_t;

#endif /* ATT_ERRORS_H */

/*! *********************************************************************************
* @}
********************************************************************************** */
