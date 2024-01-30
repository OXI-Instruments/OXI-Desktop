#ifndef SEQ_DEFS_H
#define SEQ_DEFS_H

#include "SYSEX_APP.h"
#include "SEQ_Scale.h"
#include "crc32.h"
#include "string.h"



#define SEQ_STEP_INACTIVE -1
#define SEQ_PROB_DEFAULT	0
#define SEQ_NOTES_MAX		8
#define MOD_SHORT_LEN			5

#define MAP_SIZE 16

#define SEQ_HEADER_LEN 				4
#define SEQ_HEADER_LEN_POSITION		2
#define SEQ_HEADER_TYPE_POSITION	0
#define SEQ_HEADER_VERSION_POS		1
#define SEQ_HEADER_LEN_POSITION		2


enum PolyVersion {
    //
    PolyVersion0 = 1,
    // Add independent MOD lengths
    PolyVersion1 = 2,
    // Add SeqParamVersion2
    // cc mod internal dest
    PolyVersion2 = 3,
    // MAP
    PolyVersion6 = 7,
    // SeqParamVersion7
    PolyVersion7 = 8,
    // automatically derive latest version
    PolyLast,
    PolyLatest = PolyLast - 1,
};


typedef enum
{
    SEQ_COPY_FROM_SEQUENCE = 0,
    SEQ_COPY_TO_SEQUENCE,		// If loading project don't check scale & root till the end
    SEQ_COPY_TO_SEQUENCE_CHECK_HARMO, // If loading pattern, check scale & root
    SEQ_COPY_TO_SEQUENCE_UNDO_REDO,
    SEQ_COPY_BUFFER
} SEQ_copy_from_e;

typedef enum
{
    SEQ_ON = 0,
    SEQ_OFF
} SEQ_active_e;

typedef enum {
    MOD_EXT_1 = 0,
    MOD_EXT_2,
    MOD_EXT_SIZE,
} MOD_ext_num_e;

typedef enum {
    LFO_1 = 0,
    LFO_2,
    LFO_SIZE,
} LFO_num_e;

typedef enum
{
    SEQ_UNMUTED = 0,
    SEQ_MUTED,
    SEQ_MUTE_RESET = 0xff
} SEQ_mute_e;


typedef struct {
    int8_t type; // NOTE or CC, 0 = OFF
    int8_t number; // NOTE number or CC number

    int8_t ch;
    uint8_t track;

    int8_t int_dest_1;
    int8_t amt_1;
    int8_t offset_1;
    int8_t int_dest_2;
    int8_t amt_2;
    int8_t offset_2;

    int8_t param_1;
    int8_t p_amt_1;
    int8_t p_off_1;
    int8_t param_2;
    int8_t p_amt_2;
    int8_t p_off_2;

    int8_t cc_out;
    int8_t consume;

    int8_t cc_out_ch;
    int8_t reserved;

    int8_t value; // -1 = OFF
    int8_t last_ui_value;
    int8_t last_cc_value;
} MAP_s;


typedef enum {
    MIDI_OUTPUT_ALL = 0,
    MIDI_OUTPUT_TRS,
    MIDI_OUTPUT_USB,
    MIDI_OUTPUT_BLE,
    MIDI_OUTPUT_SIZE,

    MIDI_OUTPUT_ALL_BUT_BLE = 0xFD,
    MIDI_OUTPUT_ALL_BUT_USB = 0xFE,
    MIDI_OUTPUT_OFF = 0xFF
} MIDI_Output_e;

typedef struct {
    int16_t cv_int_amt	;
    int8_t cv_int_dest	;
    int16_t cv_cc_amt	;
    int8_t cv_cc_dest	;
} SEQ_ModExt_v2_s;


typedef struct
{
    int8_t lru[SEQ_NOTES_POLY_MAX];
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
} SEQ_step_poly_s;



typedef struct SEQ_s
{
    uint8_t seq;
    SEQ_type_e type; 								// Stored in NVM
    uint8_t empty;									// Stored in NVM
    void * steps;
    void * specifics;
    int8_t last_note[SEQ_NOTES_MAX];

    int8_t division_index[SEQ_NOTES_MAX];		// Stored in NVM
//	int8_t division_index_mod[SEQ_NOTES_STEP_MAX];

    int8_t division_pending_index[SEQ_NOTES_MAX];
    int8_t division_clocks[SEQ_NOTES_MAX];
    uint16_t division_timestamp[SEQ_NOTES_MAX];
    uint32_t timestamp_transport[SEQ_NOTES_MAX];
    uint32_t timestamp_note_on[SEQ_NOTES_MAX];
    uint32_t timestamp_note_off[SEQ_NOTES_MAX];

    uint8_t time_sig_divider[SEQ_NOTES_MAX];	// Stored in NVM
    uint8_t time_sig_divisor[SEQ_NOTES_MAX];	// Stored in NVM
    int8_t time_stretch[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t time_offset[SEQ_NOTES_MAX];			// Stored in NVM

    uint8_t direction[SEQ_NOTES_MAX]; 				// Stored in NVM
    int8_t swing; 									// Stored in NVM

    uint8_t previous_step[SEQ_NOTES_MAX];
    uint8_t start_step[SEQ_NOTES_MAX];			// Stored in NVM
    uint8_t end_step[SEQ_NOTES_MAX];			// Stored in NVM

    // 4.0 remember part
    int8_t last_selected_part;
    int8_t last_selected_mod_part;

    // CC MOD
    int8_t mod_div_index[SEQ_NOTES_MAX];		// Stored in NVM
    int8_t mod_div_clocks[SEQ_NOTES_MAX];
    uint8_t start_mod[SEQ_NOTES_MAX];			// Stored in NVM
    uint8_t end_mod[SEQ_NOTES_MAX];				// Stored in NVM
    uint8_t mod_linked[SEQ_NOTES_MAX];			// Stored in NVM
    uint8_t mod_smooth[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t cc_mod_dest[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t mod_lane_max[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t mod_lane_min[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t mod_internal_dest_1[SEQ_NOTES_MAX];	// Stored in NVM
    int8_t mod_internal_amt_1[SEQ_NOTES_MAX];	// Stored in NVM
    int8_t mod_internal_amt_offset_1[SEQ_NOTES_MAX];// Stored in NVM
    int8_t mod_internal_dest_2[SEQ_NOTES_MAX];	// Stored in NVM
    int8_t mod_internal_amt_2[SEQ_NOTES_MAX];	// Stored in NVM
    int8_t mod_internal_amt_offset_2[SEQ_NOTES_MAX];// Stored in NVM
    uint8_t cc_mod_on_off[SEQ_NOTES_MAX];		// Stored in NVM
    int8_t cc_mod_offset[SEQ_NOTES_MAX];		// Stored in NVM
    char cc_mod_name[SEQ_NOTES_MAX][MOD_SHORT_LEN];	// Stored in NVM

    int8_t root_note;						// Stored in NVM
    int8_t octave;							// Stored in NVM
    int8_t scale_index;						// Stored in NVM

    // RAND
    int8_t rand_vel[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t rand_oct[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t rand_gate[SEQ_NOTES_MAX];			// Stored in NVM // NEW
    uint8_t rand_prob[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t rand_repeat[SEQ_NOTES_MAX];			// Stored in NVM
    int8_t rand_division[SEQ_NOTES_MAX];		// Stored in NVM

    // RAND Generator
    int8_t rand_density[SEQ_NOTES_MAX];
    int8_t rand_range[SEQ_NOTES_MAX];
    int8_t rand_bias[SEQ_NOTES_MAX];
    int8_t rand_tie[SEQ_NOTES_MAX];
    int8_t rand_humanization[SEQ_NOTES_MAX];
    int8_t rand_max_index[SEQ_NOTES_MAX];


    // ACTIVE - MUTE
    SEQ_active_e active;		// Stored in NVM
    SEQ_mute_e mute;			// Stored in NVM

    int8_t transpose; 		// Stored in NVM  SeqParamVersion2

    int8_t bar_to_reset;	// Stored in NVM	SeqParamVersion2

    // MIDI CH & DEST
    int8_t midi_channel;						// Stored in NVM
    MIDI_Output_e midi_output[SEQ_NOTES_MAX];	// Stored in NVM // NEW

    // PROG CHG, BANK
    int8_t prog_change[SEQ_NOTES_MAX];		// Stored in NVM // NEW
    int8_t bank_select_lsb[SEQ_NOTES_MAX];	// Stored in NVM // NEW
    int8_t bank_select_msb[SEQ_NOTES_MAX];	// Stored in NVM // NEW

    // 3.9.27 Instrument
    int8_t instrument[SEQ_NOTES_MAX];       // Stored in NVM

    SEQ_Lfo_s lfo_s[LFO_SIZE];		// Stored in NVM

    SEQ_ModExt_v2_s mod_ext_s[MOD_EXT_SIZE]; // Stored in NVM
    uint8_t last_selected;

    MAP_s map[MAP_SIZE]; // Stored in NVM

} SEQ_s;




void SEQ_PolyTouch(uint8_t voice, int8_t * lru) {
    int16_t source = SEQ_NOTES_POLY_MAX - 1;
    int16_t destination = SEQ_NOTES_POLY_MAX - 1;
    while (source >= 0) {
        if (lru[source] != voice) {
            lru[destination--] = lru[source];
        }
        --source;
    }
    lru[0] = voice;
}

static void SeqPolyStepAdd(SEQ_step_poly_s * step_poly, int32_t voice, int32_t step, int32_t octave)
{
    if (voice != SEQ_STEP_INACTIVE) {
        step_poly->step[voice] = step;
        step_poly->gate[voice] = 50;
        step_poly->vel[voice] = 64;
        step_poly->oct[voice] = octave;
        step_poly->offset[voice] = 0;
        step_poly->repeat[voice] = 1;
        step_poly->prob[voice] = SEQ_PROB_DEFAULT;
        step_poly->size++;
        SEQ_PolyTouch(voice, step_poly->lru);
    }
}




int16_t SEQ_PolyStack(SEQ_s * p_seq, uint16_t abs_step, int16_t step, int16_t octave, uint16_t toggle)
{
    if (abs_step > (SEQ_STEPS_MAX - 1)) {
        return SEQ_STEP_INACTIVE;
    }
    if (step == SEQ_STEP_INACTIVE) return SEQ_STEP_INACTIVE;

    SEQ_step_poly_s * step_poly = &((SEQ_step_poly_s*)p_seq->steps)[abs_step];
    int16_t voice_index, voice = SEQ_STEP_INACTIVE;
    uint16_t i;
    uint16_t scale_num = SEQ_GetScaleNumNotes(p_seq->scale_index);
    for (voice_index = 0; voice_index < SEQ_NOTES_POLY_MAX; voice_index++)
    {
        if ((step_poly->step[voice_index] == step) && (step_poly->oct[voice_index] == octave)) {
            voice = voice_index;
            break;
        }
        // To handle steps that are out of scale range
        if ((step_poly->step[voice_index] != SEQ_STEP_INACTIVE) && (step_poly->step[voice_index] >= scale_num))
        {
            int16_t step_temp = step_poly->step[voice_index] % scale_num;
            int16_t oct_temp = step_poly->step[voice_index] / scale_num + step_poly->oct[voice_index];

            if ((step_temp == step) && (oct_temp == octave)) {
                voice = voice_index;
                break;
            }
        }
    }
    if (toggle == 1) {
        if (voice != SEQ_STEP_INACTIVE) { /* Remove note if found and return */
            return voice;
        }

        // SEQ_NotEmpty(p_seq);

        // Try to find the least recently touched, currently inactive voice.
        if (voice == SEQ_STEP_INACTIVE) {
            for (i = 0; i < SEQ_NOTES_POLY_MAX; ++i) {
                if (step_poly->step[step_poly->lru[i]] == SEQ_STEP_INACTIVE) {
                    voice = step_poly->lru[i]; // TODO, check if necessary, if always true.
                }
            }
        }
        // If all voices are active, use the least or most recently played note
        // (voice-stealing).
        if (voice == SEQ_STEP_INACTIVE) {
            for (i = 0; i < SEQ_NOTES_POLY_MAX; ++i) {
                if (step_poly->lru[i] < SEQ_NOTES_POLY_MAX)
                    voice = step_poly->lru[i];
            }

        }
        if (step == SEQ_STEP_INACTIVE) {
            return SEQ_STEP_INACTIVE;
        }

        // check note limits
        int note = SEQ_GetScaleStep(p_seq->scale_index, step) + octave * 12 + p_seq->root_note + p_seq->octave;

        if (note > 0 && note < 120)
            SeqPolyStepAdd(step_poly, voice, step, octave);
    }
    return voice;
}




static void MapClear(MAP_s * p_map)
{
    memset(p_map, 0, sizeof(MAP_s));

    p_map->int_dest_1 = -1;
    p_map->amt_1 = 100;
    p_map->int_dest_2 = -1;
    p_map->amt_2 = 100;
    p_map->param_1 = -1;
    p_map->p_amt_1 = 100;
    p_map->param_2 = -1;
    p_map->p_amt_2 = 100;
    p_map->cc_out = -1;
}

static void MapCopy(void * p_map_to, void * p_map_from)
{
    memcpy(p_map_to, p_map_from, 18 /*MAP_STRUCT_SIZE*/);
}

int MAP_WriteBuffer(SEQ_s * p_seq, uint8_t * buff)
{
    int ptr = 0;
    for (int map_idx = 0; map_idx < MAP_SIZE; ++map_idx) {
        MapCopy(&buff[ptr], &p_seq->map[map_idx]);
        ptr += 18; // MAP_STRUCT_SIZE;
    }
    return ptr;
}


uint16_t  SEQ_ParamCopy(SEQ_s * p_seq, uint8_t * p_copy, SEQ_copy_from_e copy_type, uint16_t version)
{
    uint16_t ret = 0;

    p_copy[ret++] = (uint8_t)p_seq->empty;
    for (uint16_t i = 0; i < SEQ_NOTES_MAX; i++) {
        p_copy[ret++] = (uint8_t)p_seq->direction[i] & 0x0F;
    }
    memcpy(&p_copy[ret], p_seq->division_index, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->end_step, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->start_step, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;


    p_copy[ret++] = (uint8_t)p_seq->octave;
    p_copy[ret++] = (uint8_t)p_seq->root_note;
    p_copy[ret++] = (uint8_t)p_seq->scale_index;
    p_copy[ret++] = (int8_t)p_seq->swing;

    p_copy[ret++] = (uint8_t)p_seq->midi_channel;
    memcpy(&p_copy[ret], p_seq->midi_output, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;

    memcpy(&p_copy[ret], p_seq->prog_change, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->bank_select_lsb, sizeof(p_seq->bank_select_lsb));
    ret += sizeof(p_seq->bank_select_lsb);
    memcpy(&p_copy[ret], p_seq->bank_select_msb, sizeof(p_seq->bank_select_msb));
    ret += sizeof(p_seq->bank_select_msb);

    memcpy(&p_copy[ret], p_seq->time_sig_divider, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->time_sig_divisor, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->time_stretch, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->time_offset, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;

    memcpy(&p_copy[ret], p_seq->rand_vel, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_oct, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_gate, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_prob, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_repeat, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_division, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;

    memcpy(&p_copy[ret], &p_seq->lfo_s[LFO_1], sizeof(SEQ_Lfo_s));
    ret += sizeof(SEQ_Lfo_s);
    memcpy(&p_copy[ret], &p_seq->lfo_s[LFO_2], sizeof(SEQ_Lfo_s));
    ret += sizeof(SEQ_Lfo_s);

    // SeqParamVersion7
    p_copy[ret++] = p_seq->transpose;

    p_copy[ret++] = p_seq->bar_to_reset;

    memcpy(&p_copy[ret], &p_seq->mod_ext_s[MOD_EXT_1], sizeof(SEQ_ModExt_v2_s));
    ret += sizeof(SEQ_ModExt_v2_s);
    memcpy(&p_copy[ret], &p_seq->mod_ext_s[MOD_EXT_2], sizeof(SEQ_ModExt_v2_s));
    ret += sizeof(SEQ_ModExt_v2_s);
    p_copy[ret++] = 0; // p_seq->not_used

    // Version 1
    {
        memcpy(&p_copy[ret], p_seq->mod_div_index, SEQ_NOTES_MAX);
        ret += SEQ_NOTES_MAX;
        memcpy(&p_copy[ret], p_seq->start_mod, SEQ_NOTES_MAX);
        ret += SEQ_NOTES_MAX;
        memcpy(&p_copy[ret], p_seq->end_mod, SEQ_NOTES_MAX);
        ret += SEQ_NOTES_MAX;
        memcpy(&p_copy[ret], p_seq->mod_linked, SEQ_NOTES_MAX);
        ret += SEQ_NOTES_MAX;
        memcpy(&p_copy[ret], p_seq->mod_smooth, SEQ_NOTES_MAX);
        ret += SEQ_NOTES_MAX;
    }

    // SeqParamVersion6
    ret += MAP_WriteBuffer(p_seq, &p_copy[ret]);

    // SeqParamVersion7
    // Mute
    p_copy[ret++] = p_seq->mute;
    // Instr definition
    memcpy(&p_copy[ret], p_seq->instrument, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    // Rand generator
    memcpy(&p_copy[ret], p_seq->rand_density, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_range, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_bias, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_tie, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_humanization, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;
    memcpy(&p_copy[ret], p_seq->rand_max_index, SEQ_NOTES_MAX);
    ret += SEQ_NOTES_MAX;

    return ret;

}


static uint16_t WritePoly(uint8_t * copy_to, SEQ_s * p_seq)
{
    uint16_t ret;

    uint16_t i = 0;
    copy_to[SEQ_HEADER_TYPE_POSITION] = p_seq->type;
    copy_to[SEQ_HEADER_VERSION_POS] = PolyLatest;
    ret = SEQ_HEADER_LEN;

    for (i = 0; i < SEQ_STEPS_MAX; ++i)
    {
        /* Ignore note pointer */
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].step, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].vel, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].oct, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].gate, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].repeat, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].prob, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        memcpy(&copy_to[ret], ((SEQ_step_poly_s*)p_seq->steps)[i].offset, SEQ_NOTES_POLY_MAX);
        ret += SEQ_NOTES_POLY_MAX;
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].modulation[0];
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].modulation[1];
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].modulation[2];
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].modulation[3];
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].param;
        copy_to[ret++] = ((SEQ_step_poly_s*)p_seq->steps)[i].size;
    }
    memcpy(&copy_to[ret], p_seq->cc_mod_on_off, SEQ_MOD_NUM);
    ret+=SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->cc_mod_dest, SEQ_MOD_NUM);
    ret+=SEQ_MOD_NUM;

    // Poly Version 2
    memcpy(&copy_to[ret], p_seq->mod_lane_max, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_lane_min, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_dest_1, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_amt_1, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_amt_offset_1, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_dest_2, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_amt_2, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;
    memcpy(&copy_to[ret], p_seq->mod_internal_amt_offset_2, SEQ_MOD_NUM);
    ret += SEQ_MOD_NUM;

    // Version 1
    memcpy(&copy_to[ret], p_seq->cc_mod_offset, SEQ_MOD_NUM);
    ret+=SEQ_MOD_NUM;


    for (i = 0; i < SEQ_MOD_NUM; i++) {
        memcpy(&copy_to[ret], &p_seq->cc_mod_name[i], MOD_SHORT_LEN);
        ret += MOD_SHORT_LEN;
    }

    ret += SEQ_ParamCopy(p_seq, &copy_to[ret], SEQ_COPY_FROM_SEQUENCE, PolyLatest);

    return ret;
}


uint32_t SEQ_CopySeqBuffer(SEQ_s * p_seq, uint8_t * copy_from, uint8_t * copy_to)
{
    uint16_t ret = 0;

    ret = WritePoly(copy_to, p_seq);

    memcpy(&copy_to[SEQ_HEADER_LEN_POSITION], &ret, 2);
    ret = ((ret+3)/4); // round to uint32_t
    uint32_t crc_result = crc32((uint32_t*)copy_to, ret);
    ((uint32_t *)copy_to)[ret] = crc_result;
    ret += 4;
    return ret;
}


#endif // SEQ_DEFS_H
