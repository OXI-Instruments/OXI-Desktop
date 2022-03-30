/*
 * SYSEX_PROJ.h
 *
 *  Created on: Aug 4, 2021
 *      Author: ManuelVr
 */

#ifndef SYSEX_PROJ_H_
#define SYSEX_PROJ_H_

#include "stdint.h"

#define SONG_NUM            12
#define SEQUENCERS_NUM		4
#define PATTERN_PER_SEQ     16
#define NAME_LEN            14
#define PROJ_NAME_LEN       12


typedef struct __packed
{
    char name[NAME_LEN];
} SYSEX_Name_s;

//#pragma pack(1)
#pragma pack(push,1)
typedef struct
//__packed
{
	uint8_t project_num;
    char project_name[PROJ_NAME_LEN];
	uint16_t tempo;
    SYSEX_Name_s pattern_name[SEQUENCERS_NUM][PATTERN_PER_SEQ];
    SYSEX_Name_s song_name[SONG_NUM];
	// SYSEX_Name_s cc_name[128];
	uint32_t checksum;
} SYSEX_ProjInfo_s;
#pragma pack(pop)

#endif /* SYSEX_PROJ_H_ */
