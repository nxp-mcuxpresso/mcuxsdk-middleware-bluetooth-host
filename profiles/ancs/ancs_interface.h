 /*! *********************************************************************************
 * \defgroup BLE ANCS Profile
 * @{
 ********************************************************************************** */
/*! *********************************************************************************
* Copyright 2018-2019, 2023-2024 NXP
*
*
* \file
*
* This file is the interface file for the BLE ANCS Service/Profile
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

#ifndef ANCS_INTERFACE_H
#define ANCS_INTERFACE_H

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


/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/*! ANCS Client Configuration */
typedef struct ancsClientConfig_tag
{
    uint16_t    hService;
    uint16_t    hNotificationSource;
    uint16_t    hNotificationSourceCccd;
    uint16_t    hControlPoint;
    uint16_t    hDataSource;
    uint16_t    hDataSourceCccd;
} ancsClientConfig_t;

/*! AMS Client Configuration */
typedef struct amsClientConfig_tag
{
    uint16_t    hService;
    uint16_t    hRemoteCommand;
    uint16_t    hRemoteCommandCccd;
    uint16_t    hEntityUpdate;
    uint16_t    hEntityUpdateCccd;
    uint16_t    hEntityAttribute;
} amsClientConfig_t;

/*! ANCS Notification Category Id enumeration */
typedef enum ancsCatId_tag
{
    gAncsCatIdOther_c               = 0u,
    gAncsCatIdIncomingCall_c        = 1u,
    gAncsCatIdMissedCall_c          = 2u,
    gAncsCatIdVoicemail_c           = 3u,
    gAncsCatIdSocial_c              = 4u,
    gAncsCatIdSchedule_c            = 5u,
    gAncsCatIdEmail_c               = 6u,
    gAncsCatIdNews_c                = 7u,
    gAncsCatIdHealthAndFitness_c    = 8u,
    gAncsCatIdBusinessAndFinance_c  = 9u,
    gAncsCatIdLocation_c            = 10u,
    gAncsCatIdEntertainment_c       = 11u,
    gAncsCatIdInvalid_c,
} ancsCatId_t;

/*! AMS Remote Command Id enumeration */
typedef enum amsRemoteCommandId_tag
{
    gAmsRemoteCommandIdPlay_c               = 0u,
    gAmsRemoteCommandIdPause_c              = 1u,
    gAmsRemoteCommandIdTogglePlayPause_c    = 2u,
    gAmsRemoteCommandIdNextTrack_c          = 3u,
    gAmsRemoteCommandIdPreviousTrack_c      = 4u,
    gAmsRemoteCommandIdVolumeUp_c           = 5u,
    gAmsRemoteCommandIdVolumeDown_c         = 6u,
    gAmsRemoteCommandIdAdvanceRepeatMode_c  = 7u,
    gAmsRemoteCommandIdAdvanceShuffleMode_c = 8u,
    gAmsRemoteCommandIdSkipForward_c        = 9u,
    gAmsRemoteCommandIdSkipBackward_c       = 10u,
    gAmsRemoteCommandIdLikeTrack_c          = 11u,
    gAmsRemoteCommandIdDislikeTrack_c       = 12u,
    gAmsRemoteCommandIdBookmarkTrack_c      = 13u,
    gAmsRemoteCommandIdInvalid_c,
} amsRemoteCommandId_t;

/*! AMS Entity Update Id enumeration */
typedef enum amsEntityUpdateId_tag
{
    gAmsEntityIdPlayer_c                = 0u,
    gAmsEntityIdQueue_c                 = 1u,
    gAmsEntityIdTrack_c                 = 2u,
    gAmsEntityIdInvalid_c,
} amsEntityUpdateId_t;

/*! AMS Track Attribute Id enumeration */
typedef enum amsTrackAttributeId_tag
{
    gAmsTrackAttributeIdArtist_c        = 0u,
    gAmsTrackAttributeIdAlbum_c         = 1u,
    gAmsTrackAttributeIdTitle_c         = 2u,
    gAmsTrackAttributeIdDuration_c      = 3u,
    gAmsTrackAttributeIdInvalid_c,
} amsTrackAttributeId_t;

/*! ANCS Notification Event Id enumeration */
typedef enum ancsEventId_tag
{
    gAncsEventIdNotificationAdded_c     = 0u,
    gAncsEventIdNotificationModified_c  = 1u,
    gAncsEventIdNotificationRemoved_c   = 2u,
    gAncsEventIdInvalid_c,
} ancsEventId_t;

/*! ANCS Notification Event Flags enumeration */
typedef uint8_t ancsEventFlag_t;
typedef enum
{
    gAncsEventFlagSilent_c          = 0x01u,
    gAncsEventFlagImportant_c       = 0x02u,
    gAncsEventFlagPreExisting_c     = 0x04u,
    gAncsEventFlagPositiveAction_c  = 0x08u,
    gAncsEventFlagNegativeAction_c  = 0x10u,
} ancsEventFlag_tag;

#define gAncsValidEventFlagsMask_c = (gAncsEventFlagSilent_c | gAncsEventFlagImportant_c |              \
                                      gAncsEventFlagPreExisting_c | gAncsEventFlagPositiveAction_c |    \
                                      gAncsEventFlagNegativeAction_c)

/*! ANCS Command Id enumeration */
typedef enum ancsComdId_tag
{
    gAncsCmdIdGetNotificationAttributes_c   = 0u,   /*!< Obtain notification attributes from the Notifications Provider */
    gAncsCmdIdGetAppAttributes_c            = 1u,   /*!< Obtain application information from the Notifications Provider */
    gAncsCmdIdPerformNotificationAction_c   = 2u,   /*!< Perform a specified action on a notification from the Notifications Provider */
    gAncsCmdIdNoCommand_c                   = 0xffu /*!< No command */
} ancsComdId_t;

/*! ANCS Notification Attribute Id enumeration */
typedef enum ancsNotifAttrId_tag
{
    gAncsNotifAttrIdAppIdentifier_c         = 0u,
    gAncsNotifAttrIdTitle_c                 = 1u,   /*!< Followed by a 2 byte max length parameter */
    gAncsNotifAttrIdSubtitle_c              = 2u,   /*!< Followed by a 2 byte max length parameter */
    gAncsNotifAttrIdMessage_c               = 3u,   /*!< Followed by a 2 byte max length parameter */
    gAncsNotifAttrIdMessageSize_c           = 4u,
    gAncsNotifAttrIdDate_c                  = 5u,
    gAncsNotifAttrIdPositiveActionLabel_c   = 6u,
    gAncsNotifAttrIdNegativeActionLabel_c   = 7u,
    gAncsNotifAttrIdInvalid                 = 0xffu,
} ancsNotifAttrId_t;

/*! ANCS Action Id enumeration */
typedef enum ancsActionId_tag
{
    gAncsActionIdPositive_c     = 0u,
    gAncsActionIdNegative_c     = 1u,
} ancsActionId_t;

/*! ANCS Application Attribute Id enumeration */
typedef enum ancsAppAttrId_tag
{
    gAncsAppAttrIdDisplayName_c = 0u,
    gAncsAppAttrIdInvalid_c     = 0xffu,
} ancsAppAttrId_t;

/*! ANCS Notification Source Event */
typedef struct ancsNsEvent_tag
{
    ancsEventId_t   eventId;
    uint8_t         eventFlags;
    ancsCatId_t     categoryId;
    uint8_t         categoryCount;
    uint32_t        notifUid;
} ancsNsEvent_t;

/*! ANCS Notification Source Packet */
typedef PACKED_STRUCT ancsNsPacket_tag
{
    uint8_t         eventId;
    uint8_t         eventFlags;
    uint8_t         categoryId;
    uint8_t         categoryCount;
    uint32_t        notifUid;
} ancsNsPacket_t;

#define gAncsNsPacketMaxLength          (8u)

#define gAncsEventIdFieldLength_c       (1u)
#define gAncsEventFlagsFieldLength_c    (1u)
#define gAncsCategIdFieldLength_c       (1u)
#define gAncsCategCountFieldLength_c    (1u)
#define gAncsNotifUidFieldLength_c      (4u)
#define gAncsCmdIdFieldLength_c         (1u)
#define gAncsAttrIdFieldLength_c        (1u) /*!< For both Notification and Application attributes */
#define gAncsAttrMaxLengthFieldLength_c (2u) 
#define gAncsAttrLengthFieldLength_c    (2u) /*!< For both Notification and Application attributes */
#define gAncsActionIdFieldLength_c      (1u)
#define gAmsTotalRemoteCommandIds_c       (14u)
#define gAmsRemoteCommandIdFieldLength_c  (1u)
#define gAmsEntityUpdateIdFieldLength_c   (1u)
#define gAmsTrackAttributeIdFieldLength_c (1u)
#define gAmsTrackNumAttributeId_c         (4u) /* Number of attributes to be configured for the track */


/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* ANCS_INTERFACE_H */

/*! **********************************************************************************
 * @}
 ************************************************************************************/
