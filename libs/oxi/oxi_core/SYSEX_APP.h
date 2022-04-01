/*
 * SYSEX_APP.h
 *
 *  Created on: Jan 22, 2022
 *      Author: ManuelVr
 */

#ifndef SYSEX_APP_H_
#define SYSEX_APP_H_

#include "stdint.h"

#define SEQ_STEPS_MAX			128
#define SEQ_NOTES_STEP_MAX		8
#define SEQ_NUM_TRACKS_MULTI	8
#define SEQ_NOTES_POLY_MAX		7

#define SEQ_MOD_NUM				4

#define PROJ_HEADER_LEN		2
#define PROJ_CRC_LEN		4
#define PROJ_NAME_LEN		12

typedef enum
{
    SEQ_1 = 0,
    SEQ_2,
    SEQ_3,
    SEQ_4,
    SEQ_SIZE
} SEQ_e;

typedef enum
{
    SEQ_MONO = 0,
    SEQ_POLY,
    SEQ_CHORDS,
    SEQ_MULTITRACK,
    SEQ_TYPE_SIZE,
    SEQ_CURVE,
    SEQ_MELODIC_STEPS,
    SEQ_CARTESIAN,
} SEQ_type_e;

#pragma pack(push,2)
typedef struct {
    int16_t rate; // default : 12
    int8_t shape; // def: 0
    int8_t amount; // def: -1 == 0xFF
    uint8_t dest; // def: 0
    int8_t offset;
    // uint8_t align; ?¿?¿?
//	int8_t retrigger_mode;
} SEQ_Lfo_s;
#pragma pack(pop)

#pragma pack(push,2)
typedef struct {
    uint8_t cc_ext;
    int8_t cc_ext_dest;
    int16_t cv_in_amt;
    uint8_t cv_in_dest;
} SEQ_ModExt_s;
#pragma pack(pop)

// GENERAL PARAMETERS
#pragma pack(push,1)
typedef struct {
    uint8_t emtpy;
    uint8_t direction[SEQ_NOTES_STEP_MAX];
    uint8_t division_index[SEQ_NOTES_STEP_MAX];
    uint8_t end_step[SEQ_NOTES_STEP_MAX];
    uint8_t start_step[SEQ_NOTES_STEP_MAX];
    uint8_t octave;
    uint8_t root_note;
    uint8_t scale_index;
    uint8_t swing;
    uint8_t rand_vel;
    uint8_t rand_oct;
    uint8_t rand_prob;
    uint8_t rand_repeat;
    uint8_t midi_channel;
    uint8_t rand_division;
    SEQ_Lfo_s lfo;
    SEQ_ModExt_s mod_ext;
    uint8_t color_rotation;
} SEQ_param_buffer_s;
#pragma pack(pop)

// Multitrack step data
#pragma pack(push,1)
typedef struct
{
    uint8_t trigger;
    int8_t multi_step[SEQ_NUM_TRACKS_MULTI];
    uint8_t vel[SEQ_NUM_TRACKS_MULTI];
    uint8_t gate[SEQ_NUM_TRACKS_MULTI];
    uint8_t modulation[SEQ_NUM_TRACKS_MULTI];
    int8_t repeat[SEQ_NUM_TRACKS_MULTI];
    int8_t prob[SEQ_NUM_TRACKS_MULTI];
    int8_t offset[SEQ_NUM_TRACKS_MULTI];
} SEQ_step_multi_s;
#pragma pack(pop)


// MULTITRACK  SEQ
#pragma pack(push,1)
typedef struct {
    uint8_t type;
    uint8_t byte_align;
    uint16_t length;
    SEQ_step_multi_s steps[SEQ_STEPS_MAX];
    uint8_t note_offset[SEQ_NOTES_STEP_MAX];
    uint8_t midi_channel_offset[SEQ_NOTES_STEP_MAX];
    uint8_t track_glide[SEQ_NOTES_STEP_MAX];
    uint8_t cc_mod[SEQ_NOTES_STEP_MAX];
    uint8_t cc_mod_global[SEQ_NOTES_STEP_MAX]; // TODO change order in the future, like in other modes
    uint8_t mute_mask;
    uint8_t random_mask;
    uint8_t solo_mask;
    uint8_t lfo_mask;
    uint8_t mod_ext_mask;
    SEQ_param_buffer_s seq_params;
    uint8_t byte_align2;
    uint16_t word_align;
    uint32_t crc_check;
} SEQ_multi_buffer_s;
#pragma pack(pop)

// Mono step data
#pragma pack(push,1)
typedef struct {
    int8_t step;
    uint8_t velocity;
    int8_t octave;
    uint8_t gate;
    uint8_t modulation[SEQ_MOD_NUM];
    uint8_t repeat;
    int8_t prob;
    int8_t offset;
    int8_t param; // == glide in mono and strum in chord
} SEQ_step_mono_buffer_s;
#pragma pack(pop)

// MONO SEQ
#pragma pack(push,1)
typedef struct {
    uint8_t type;
    uint8_t byte_align;
    uint16_t length;
    SEQ_step_mono_buffer_s steps[SEQ_STEPS_MAX];
    uint8_t cc_mod_global[SEQ_MOD_NUM];
    uint8_t cc_mod[SEQ_MOD_NUM];
    SEQ_param_buffer_s seq_params;
    uint32_t crc_check;
} SEQ_mono_buffer_s;
#pragma pack(pop)

#define  SEQ_step_chord_buffer_s SEQ_step_mono_buffer_s

// CHORD SEQ
#pragma pack(push,1)
typedef struct {
    uint8_t type;
    uint8_t byte_align;
    uint16_t length;
    SEQ_step_chord_buffer_s steps[SEQ_STEPS_MAX];
    uint8_t arp_type;
    uint8_t arp_oct;
    uint8_t arp_distance;
    uint8_t chord_type[SEQ_STEPS_MAX];
    uint8_t chord_inversion[SEQ_STEPS_MAX];
    uint8_t chord_bass;
    uint8_t chord_spread;
    uint8_t cc_mod_global[SEQ_MOD_NUM];
    uint8_t cc_mod[SEQ_MOD_NUM];
    SEQ_param_buffer_s seq_params;
    uint8_t byte_align2;
    uint16_t word_align;
    uint32_t crc_check;
} SEQ_chord_buffer_s;
#pragma pack(pop)





// Poly step data
#pragma pack(push,1)
typedef struct {
    int8_t step[SEQ_NOTES_POLY_MAX];
    uint8_t vel[SEQ_NOTES_POLY_MAX];
    int8_t oct[SEQ_NOTES_POLY_MAX];
    uint8_t gate[SEQ_NOTES_POLY_MAX];
    int8_t repeat[SEQ_NOTES_POLY_MAX];
    int8_t prob[SEQ_NOTES_POLY_MAX];
    int8_t offset[SEQ_NOTES_POLY_MAX];
    uint8_t modulation[SEQ_MOD_NUM];
    int8_t param;
    int8_t size;
} SEQ_step_poly_buffer_s;
#pragma pack(pop)

// POLY SEQ
#pragma pack(push,1)
typedef struct {
    uint8_t type;
    uint8_t byte_align;
    uint16_t length;
    SEQ_step_poly_buffer_s steps[SEQ_STEPS_MAX];
    uint8_t cc_mod_global[SEQ_MOD_NUM];
    uint8_t cc_mod[SEQ_MOD_NUM];
    SEQ_param_buffer_s seq_params;
    uint32_t crc_check;
} SEQ_poly_buffer_s;
#pragma pack(pop)


// PROJECT RELATED STUFF
#define SEQ_PRESET_NAME_LEN	(14)
#define ARR_TRACKS_NUM			4
#define ARR_PATTERN_NUM			16
#define ARR_SONG_NAME_LEN		14

#define ARR_SONG_NUM		10

#define CV_SIZE		8
#define GATE_SIZE		8

typedef struct
{
    uint8_t is_empty;
    int8_t color_variation;
    char preset_name[SEQ_PRESET_NAME_LEN];
} SEQ_Preset_s;

typedef struct
{
    int8_t repeat;
    uint8_t preset_num;
    int8_t program_change;
} ARR_Pattern_s;

typedef struct
{
    ARR_Pattern_s patterns[ARR_PATTERN_NUM];
} ARR_Track_s;

typedef struct
{
    char song_name[ARR_SONG_NAME_LEN];
    ARR_Track_s tracks[ARR_TRACKS_NUM];
} ARR_Song_s;

typedef struct
{
    uint8_t cv_seq[CV_SIZE]; // CV_source_e
    uint8_t cv_out[CV_SIZE]; // CV_source_e
    uint8_t cv_voice[CV_SIZE];
    int8_t cv_config_1[CV_SIZE];
    int8_t cv_config_2[CV_SIZE];
    uint8_t gate_seq[GATE_SIZE];
    uint8_t gate_out[GATE_SIZE];
    uint8_t gate_voice[GATE_SIZE];
    int8_t gate_config[GATE_SIZE];
} CV_assign_s;

// PROJ
#pragma pack(push,1)
typedef struct {
//	uint16_t length;
//    char proj_version;
//    char proj_version2;
    uint16_t proj_version;
    char proj_name[PROJ_NAME_LEN];
    uint8_t proj_num;
    uint8_t byte_align;
    uint16_t tempo;
    SEQ_Preset_s presets[4][ARR_PATTERN_NUM];
    uint8_t loaded_song;
    ARR_Song_s songs[ARR_SONG_NUM];
    CV_assign_s cv_assign;
    uint8_t loaded_preset[4];
    uint8_t active_seq[4];
    uint8_t arr_launch_master;
    uint8_t dummy_align_data[20];
    uint32_t crc_check;
} PROJ_buffer_s;
#pragma pack(pop)



// PROJ 0.1.3
typedef struct {
    uint16_t length;
    char proj_name[PROJ_NAME_LEN];
    uint8_t proj_num;
    uint8_t byte_align;
    uint16_t tempo;
    SEQ_Preset_s presets[4][ARR_PATTERN_NUM];
    uint8_t loaded_song;
    ARR_Song_s songs[ARR_SONG_NUM];
    CV_assign_s cv_assign;
    uint8_t loaded_preset[4];
    uint8_t dummy_align_data[21];
    uint32_t crc_check;
} PROJ_buffer_0_1_3_s;



#endif // SYSEX_APP_H
