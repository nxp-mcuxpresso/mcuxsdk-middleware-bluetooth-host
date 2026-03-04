/*! *********************************************************************************
* Copyright 2025-2026 NXP
*
* NXP Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from NXP.
********************************************************************************** */
#ifndef PTS_TEST_VECTORS_H
#define PTS_TEST_VECTORS_H
/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"

/************************************************************************************
*************************************************************************************
* Public memory declarations
*************************************************************************************
************************************************************************************/
/* Test vector 1 - single_subevent_mode1_only */
extern uint16_t tv1Se0DataIndex;
extern uint16_t tv1Se0c0DataIndex;
extern uint16_t tv1Se0c1DataIndex;
extern uint8_t tv1[];

/* Test vector 2 - single_subevent_mode2_mode1 (actually two subevents) */
extern uint16_t tv2Se0DataIndex;
extern uint16_t tv2Se0c0DataIndex;
extern uint16_t tv2Se0c1DataIndex;
extern uint16_t tv2Se0c2DataIndex;
extern uint16_t tv2Se1DataIndex;
extern uint8_t tv2[];

/* Test vector 3 - single_subevent_mode2_only */
extern uint16_t tv3Se0DataIndex;
extern uint16_t tv3Se0c0DataIndex;
extern uint16_t tv3Se0c1DataIndex;
extern uint16_t tv3Se0c2DataIndex;
extern uint8_t tv3[];


/* Test vector 4 - single_subevent_mode3_only (actually two subevents) */
extern uint16_t tv4Se0DataIndex;
extern uint16_t tv4Se0c0DataIndex;
extern uint16_t tv4Se0c1DataIndex;
extern uint16_t tv4Se0c2DataIndex;
extern uint16_t tv4Se0c3DataIndex;
extern uint16_t tv4Se1DataIndex;
extern uint16_t tv4Se1c0DataIndex;
extern uint8_t tv4[];

/* Test vector 5 - multiple_subevents_mode1_only (7 subevents) */
extern uint16_t tv5Se0DataIndex;
extern uint16_t tv5Se1DataIndex;
extern uint16_t tv5Se2DataIndex;
extern uint16_t tv5Se3DataIndex;
extern uint16_t tv5Se4DataIndex;
extern uint16_t tv5Se5DataIndex;
extern uint16_t tv5Se6DataIndex;
extern uint8_t tv5[];

/* Test vector 6 - multiple_subevents_mode2_mode1 (7 subevents) */
extern uint16_t tv6Se0DataIndex;
extern uint16_t tv6Se1DataIndex;
extern uint16_t tv6Se2DataIndex;
extern uint16_t tv6Se3DataIndex;
extern uint16_t tv6Se4DataIndex;
extern uint16_t tv6Se5DataIndex;
extern uint16_t tv6Se6DataIndex;
extern uint8_t tv6[];

/* Test vector 7 - multiple_subevents_mode2_only (6 subevents) */
extern uint16_t tv7Se0DataIndex;
extern uint16_t tv7Se1DataIndex;
extern uint16_t tv7Se2DataIndex;
extern uint16_t tv7Se3DataIndex;
extern uint16_t tv7Se4DataIndex;
extern uint16_t tv7Se5DataIndex;
extern uint8_t tv7[];

/* Test vector 8 - multiple_subevents_mode3_only (8 subevents) */
extern uint16_t tv8Se0DataIndex;
extern uint16_t tv8Se1DataIndex;
extern uint16_t tv8Se2DataIndex;
extern uint16_t tv8Se3DataIndex;
extern uint16_t tv8Se4DataIndex;
extern uint16_t tv8Se5DataIndex;
extern uint16_t tv8Se6DataIndex;
extern uint16_t tv8Se7DataIndex;
extern uint8_t tv8[];


/* Test vector 9 - filter_input_mode0123_Ini_sounding_nAP4 (4 subevents) */
extern uint16_t tv9Se0DataIndex;
extern uint16_t tv9Se0c0DataIndex;
extern uint16_t tv9Se1DataIndex;
extern uint16_t tv9Se1c0DataIndex;
extern uint16_t tv9Se2DataIndex;
extern uint16_t tv9Se3DataIndex;
extern uint8_t tv9[];

#endif /* PTS_TEST_VECTORS_H */