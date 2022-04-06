#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#pragma once

#include "SYSEX_APP.h"
#include "Parser.h"
#include "crc32.h"
#include <memory>

namespace oxitools {

// PROJ_buffer_s
//      SEQ_Preset_s[4][16] = color, name
//
//      ARR_Song_s[10]
//          ARR_Track_s[4]
//              ARR_Pattern_s[16] = preset_num, repeat, program_change
//
// SEQ_param_buffer_s = octave, root_note, scale_index...
//     SEQ_Lfo_s
//     SEQ_ModExt_s
//
// Patterns:
//     SEQ_multi_buffer_s
//          SEQ_step_multi_s
//     SEQ_mono_buffer_s
//          SEQ_step_mono_buffer_s
//     SEQ_chord_buffer_s
//          SEQ_step_chord_buffer_s
//     SEQ_poly_buffer_s
//          SEQ_step_poly_buffer_s

struct ProjectPatternData {

    int type = 0xFF;
    char data[8192];

    ProjectPatternData() {}

    void readPattern(void * pattern, const int type) {
        this->type = type;
        uint32_t expected_crc;
        uint32_t crc_received = 0;
        uint16_t buf_len;
        auto * pattern_data = reinterpret_cast<char *>(pattern);
        switch (type) {
        case SEQ_MULTITRACK:
        {
            SEQ_multi_buffer_s * p_buf = (SEQ_multi_buffer_s*)pattern;
            buf_len = sizeof(SEQ_multi_buffer_s) ; // 7404
            crc_received = p_buf->crc_check;
            break;
        }
        case SEQ_MONO:
        {
            SEQ_mono_buffer_s * p_buf = (SEQ_mono_buffer_s*)pattern;
            buf_len = sizeof(SEQ_mono_buffer_s); // 7404
            crc_received = p_buf->crc_check;
            break;
        }
        case SEQ_POLY:
        {
            SEQ_poly_buffer_s * p_buf = (SEQ_poly_buffer_s*)pattern;
            buf_len = sizeof(SEQ_poly_buffer_s); // 7404
            crc_received = p_buf->crc_check;
            break;
        }
        case SEQ_CHORDS:
        {
            SEQ_chord_buffer_s * p_buf = (SEQ_chord_buffer_s*)pattern;
            buf_len = sizeof(SEQ_chord_buffer_s); // 7404
            crc_received = p_buf->crc_check;
            break;
        }
        default:
            buf_len = 0;
            break;
        }

        uint16_t crc_pos = (buf_len - 4 ) / 4;
        expected_crc = crc32((uint32_t*)pattern, crc_pos);
        if (expected_crc == crc_received) {
            memcpy(data, pattern_data, buf_len);
        }

    }

    void serialize(oxitools::DataWriter & writer) {
        switch (type) {
            case SEQ_MONO:
            {
                SEQ_mono_buffer_s * pattern = reinterpret_cast<SEQ_mono_buffer_s *>(data);
                serialize_SEQ_mono_buffer_s(writer, *pattern);
                break;
            }
            case SEQ_POLY:
            {
                SEQ_poly_buffer_s * pattern = reinterpret_cast<SEQ_poly_buffer_s *>(data);
                serialize_SEQ_poly_buffer_s(writer, *pattern);
                break;
            }
            case SEQ_CHORDS:
            {
                SEQ_chord_buffer_s * pattern = reinterpret_cast<SEQ_chord_buffer_s *>(data);
                serialize_SEQ_chord_buffer_s(writer, *pattern);
                break;
            }
            case SEQ_MULTITRACK:
            {
                SEQ_multi_buffer_s * pattern = reinterpret_cast<SEQ_multi_buffer_s *>(data);
                serialize_SEQ_multi_buffer_s(writer, *pattern);
                break;
            }
        }
    }
};

struct ProjectData {
    PROJ_buffer_s project;
//    SEQ_param_buffer_s sequencers[4];
    ProjectPatternData patterns[4][16];

    void serialize(oxitools::DataWriter & writer) {
        serialize_PROJ_buffer_s(writer, project);
//        for (int i = 0; i < 4; ++i) serialize_SEQ_param_buffer_s(writer, sequencers[i]);
        for (int i = 0; i < 4; ++i)
            for (int i = 0; i<4; ++i)
                patterns[4][16].serialize(writer);
    }
};

}

#endif // PROJECTDATA_H
