#ifndef PARSER_H
#define PARSER_H

#include "SYSEX_APP.h"
#include "ProjectVersion.h"
#include <cstdio>
#include <cstring>

namespace oxitools {

class DataWriter {
public:
    DataWriter(char * const data, const size_t size): data(data), size(size) {
        pointer = data;
    }

    void writeUint8(const uint8_t & value) { *pointer++ = value; }

    void writeInt8(const int8_t & value) { *pointer++ = value; }

    void writeChar(const char & value) { *pointer++ = value; }

    void writeUint16(const uint16_t value) {
        *pointer++ = static_cast<uint8_t>(value >> 8 & 0xFF);
        *pointer++ = static_cast<uint8_t>(value & 0xFF);
    }

    void writeUint32(const uint32_t value) {
        *pointer++ = static_cast<uint8_t>(value >> 24 & 0xFF);
        *pointer++ = static_cast<uint8_t>(value >> 16 & 0xFF);
        *pointer++ = static_cast<uint8_t>(value >> 8 & 0xFF);
        *pointer++ = static_cast<uint8_t>(value & 0xFF);
    }

    void writeInt16(const int16_t value) {
        *pointer++ = static_cast<uint8_t>(value >> 8 & 0xFF);
        *pointer++ = static_cast<uint8_t>(value & 0xFF);
    }

private:
    const char * data;
    const size_t size;
    char * pointer;
};

class DataReader {

public:

    DataReader(char * const data, const size_t size): data(data), size(size) {
        pointer = data;
    }

    uint8_t readUint8() { return *pointer++; }

    int8_t readInt8() { return *pointer++; }

    char readChar() { return *pointer++; }

    template<typename T>
    void read(T & val, size_t len) {
        //char * ptr = reinterpret_cast<char*>(&val);
        memcpy(&val, pointer, len);
        pointer += len;
    }


    uint16_t readUint16() {
        uint8_t hi = *pointer;
        uint8_t lo = *(pointer + 1);
        uint16_t value = static_cast<uint16_t>(hi) << 8 | static_cast<uint16_t>(lo);
        pointer += 2;
        return value;
    }

    uint32_t readUint32() {
        uint32_t value = (*pointer) << 24 | *(pointer+1) << 16 | *(pointer+2) << 8 | *(pointer+3);
        pointer += 4;
        return value;
    }

    int16_t readInt16() {
        uint16_t value = (*pointer) << 8 | *(pointer+1);
        pointer += 2;
        return value;
    }

    uint8_t peekUint8() const {
        return *pointer;
    }

private:
    const char * data;
    const size_t size;
    char * pointer;
};

// MARK: - Serialization

inline void serialize_SEQ_Lfo_s(oxitools::DataWriter & writer, SEQ_Lfo_s & object)
{
    writer.writeUint8(object.rate);
    writer.writeInt8(object.shape);
    writer.writeInt8(object.amount);
    writer.writeUint8(object.dest);
}

inline void serialize_SEQ_ModExt_s(oxitools::DataWriter & writer, SEQ_ModExt_s & object)
{
    writer.writeUint8(object.cc_ext);
    writer.writeInt8(object.cc_ext_dest);
    writer.writeInt16(object.cv_in_amt);
    writer.writeUint8(object.cv_in_dest);
}

inline void serialize_SEQ_param_buffer_s(oxitools::DataWriter & writer, SEQ_param_buffer_s & object)
{
    writer.writeUint8(object.emtpy);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.direction[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.division_index[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.end_step[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.start_step[i]);
    writer.writeUint8(object.octave);
    writer.writeUint8(object.root_note);
    writer.writeUint8(object.scale_index);
    writer.writeUint8(object.swing);
    writer.writeUint8(object.rand_vel);
    writer.writeUint8(object.rand_oct);
    writer.writeUint8(object.rand_prob);
    writer.writeUint8(object.rand_repeat);
    writer.writeUint8(object.midi_channel);
    writer.writeUint8(object.rand_division);
    serialize_SEQ_Lfo_s(writer, object.lfo);
    serialize_SEQ_ModExt_s(writer, object.mod_ext);
    writer.writeUint8(object.color_rotation);
}

inline void serialize_SEQ_step_multi_s(oxitools::DataWriter & writer, SEQ_step_multi_s & object)
{
    writer.writeUint8(object.trigger);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeInt8(object.multi_step[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeUint8(object.vel[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeUint8(object.gate[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeUint8(object.modulation[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeInt8(object.repeat[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeInt8(object.prob[i]);
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) writer.writeInt8(object.offset[i]);
}

inline void serialize_SEQ_multi_buffer_s(oxitools::DataWriter & writer, SEQ_multi_buffer_s & object)
{
    writer.writeUint8(object.type);
    writer.writeUint8(0); // aligment byte
    writer.writeUint16(object.length);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) serialize_SEQ_step_multi_s(writer, object.steps[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.note_offset[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.midi_channel_offset[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.track_glide[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.cc_mod[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) writer.writeUint8(object.cc_mod_global[i]);
    writer.writeUint8(object.mute_mask);
    writer.writeUint8(object.random_mask);
    writer.writeUint8(object.solo_mask);
    writer.writeUint8(object.lfo_mask);
    writer.writeUint8(object.mod_ext_mask);
    writer.writeUint8(object.mod_ext_mask);
    serialize_SEQ_param_buffer_s(writer, object.seq_params);
    writer.writeUint32(object.crc_check);
}

inline void serialize_SEQ_step_mono_buffer_s(oxitools::DataWriter & writer, SEQ_step_mono_buffer_s & object)
{
    writer.writeInt8(object.step);
    writer.writeUint8(object.velocity);
    writer.writeInt8(object.octave);
    writer.writeUint8(object.gate);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.modulation[i]);
    writer.writeUint8(object.repeat);
    writer.writeInt8(object.prob);
    writer.writeInt8(object.offset);
    writer.writeInt8(object.param);
}

inline void serialize_SEQ_mono_buffer_s(oxitools::DataWriter & writer, SEQ_mono_buffer_s & object)
{
    writer.writeUint8(object.type);
    writer.writeUint8(0); // aligment byte
    writer.writeUint16(object.length);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) serialize_SEQ_step_mono_buffer_s(writer, object.steps[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod_global[i]);
    //writer.writeUint16(object.word_align);
    writer.writeUint32(object.crc_check);
}

inline void serialize_SEQ_chord_buffer_s(oxitools::DataWriter & writer, SEQ_chord_buffer_s & object)
{
    writer.writeUint8(object.type);
    writer.writeUint8(0); // aligment byte
    writer.writeUint16(object.length);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) serialize_SEQ_step_mono_buffer_s(writer, object.steps[i]);
    writer.writeUint8(object.arp_type);
    writer.writeUint8(object.arp_oct);
    writer.writeUint8(object.arp_distance);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) writer.writeUint8(object.chord_type[i]);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) writer.writeUint8(object.chord_inversion[i]);
    writer.writeUint8(object.chord_bass);
    writer.writeUint8(object.chord_spread);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod_global[i]);
    serialize_SEQ_param_buffer_s(writer, object.seq_params);
    writer.writeUint32(object.crc_check);
}

inline void serialize_SEQ_step_poly_buffer_s(oxitools::DataWriter & writer, SEQ_step_poly_buffer_s & object)
{
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeInt8(object.step[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeUint8(object.vel[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeInt8(object.oct[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeUint8(object.gate[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeInt8(object.repeat[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeInt8(object.prob[i]);
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) writer.writeInt8(object.offset[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.modulation[i]);
    writer.writeInt8(object.param);
    writer.writeInt8(object.size);
}

inline void serialize_SEQ_poly_buffer_s(oxitools::DataWriter & writer, SEQ_poly_buffer_s & object)
{
    writer.writeUint8(object.type);
    writer.writeUint8(0); // aligment byte
    writer.writeUint16(object.length);
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) serialize_SEQ_step_poly_buffer_s(writer, object.steps[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) writer.writeUint8(object.cc_mod_global[i]);
    serialize_SEQ_param_buffer_s(writer, object.seq_params);
    //writer.writeUint16(object.word_align);
    writer.writeUint32(object.crc_check);
}

inline void serialize_SEQ_Preset_s(oxitools::DataWriter & writer, const SEQ_Preset_s & object)
{
    writer.writeUint8(object.is_empty);
    writer.writeInt8(object.color_variation);
    for (int i = 0; i < SEQ_PRESET_NAME_LEN; ++i) writer.writeChar(object.preset_name[i]);
}

inline void serialize_ARR_Pattern_s(oxitools::DataWriter & writer, const ARR_Pattern_s & object)
{
    writer.writeInt8(object.repeat);
    writer.writeUint8(object.preset_num);
    writer.writeInt8(object.program_change);
}

inline void serialize_ARR_Track_s(oxitools::DataWriter & writer, const ARR_Track_s & object)
{
    for (int i = 0; i < ARR_PATTERN_NUM; ++i) serialize_ARR_Pattern_s(writer, object.patterns[i]);
}

inline void serialize_ARR_Song_s(oxitools::DataWriter & writer, const ARR_Song_s & object)
{
    for (int i = 0; i < ARR_SONG_NAME_LEN; ++i) writer.writeChar(object.song_name[i]);
    for (int i = 0; i < ARR_TRACKS_NUM; ++i) serialize_ARR_Track_s(writer, object.tracks[i]);
}

inline void serialize_CV_assign_s(oxitools::DataWriter & writer, const CV_assign_s & object)
{
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.cv_seq[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.cv_out[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.cv_voice[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeInt8(object.cv_config_1[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeInt8(object.cv_config_2[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.gate_seq[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.gate_out[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.gate_voice[i]);
    for (int i = 0; i < CV_SIZE; ++i) writer.writeUint8(object.gate_config[i]);
}

inline void serialize_PROJ_buffer_s(oxitools::DataWriter & writer, const PROJ_buffer_s & object)
{
    writer.writeUint16(object.proj_version);
    for (int i = 0; i < PROJ_NAME_LEN; ++i) writer.writeChar(object.proj_name[i]);
    writer.writeUint16(object.proj_num);
    writer.writeUint16(object.tempo);
    for (int i = 0; i < ARR_TRACKS_NUM; ++i) {
        for (int j = 0; j < ARR_PATTERN_NUM; ++j) {
            serialize_SEQ_Preset_s(writer, object.presets[i][j]);
        }
    }
    writer.writeUint8(object.loaded_song);
    for (int i = 0; i < ARR_SONG_NUM; ++i) serialize_ARR_Song_s(writer, object.songs[i]);
    serialize_CV_assign_s(writer, object.cv_assign);
    for (int i = 0; i < 4; ++i) writer.writeUint8(object.loaded_preset[i]);
    for (int i = 0; i < 4; ++i) writer.writeUint8(object.active_seq[i]);
}

// MARK: - Deserialization

inline void deserialize_SEQ_Lfo_s(oxitools::DataReader & reader, SEQ_Lfo_s & object)
{
    object.rate = reader.readUint8();
    object.shape = reader.readInt8();
    object.amount = reader.readInt8();
    object.dest = reader.readUint8();
}

inline void deserialize_SEQ_ModExt_s(oxitools::DataReader & reader, SEQ_ModExt_s & object)
{
    object.cc_ext = reader.readUint8();
    object.cc_ext_dest = reader.readInt8();
    object.cv_in_amt = reader.readInt16();
    object.cv_in_dest = reader.readUint8();
}

inline void deserialize_SEQ_param_buffer_s(oxitools::DataReader & reader, SEQ_param_buffer_s & object)
{
    object.emtpy = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.direction[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.division_index[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.end_step[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.start_step[i] = reader.readUint8();
    object.octave = reader.readUint8();
    object.root_note = reader.readUint8();
    object.scale_index = reader.readUint8();
    object.swing = reader.readUint8();
    object.rand_vel = reader.readUint8();
    object.rand_oct = reader.readUint8();
    object.rand_prob = reader.readUint8();
    object.rand_repeat = reader.readUint8();
    object.midi_channel = reader.readUint8();
    object.rand_division = reader.readUint8();
    deserialize_SEQ_Lfo_s(reader, object.lfo);
    deserialize_SEQ_ModExt_s(reader, object.mod_ext);
    object.color_rotation = reader.readUint8();
}

inline void deserialize_SEQ_step_multi_s(oxitools::DataReader & reader, SEQ_step_multi_s & object)
{
    object.trigger = reader.readUint8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.multi_step[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.vel[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.gate[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.modulation[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.repeat[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.prob[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NUM_TRACKS_MULTI; ++i) object.offset[i] = reader.readInt8();
}

inline void deserialize_SEQ_multi_buffer_s(oxitools::DataReader & reader, SEQ_multi_buffer_s & object)
{
    object.type = reader.readUint8();
    reader.readUint8(); // aligment byte
    object.length = reader.readUint16();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) deserialize_SEQ_step_multi_s(reader, object.steps[i]);
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.note_offset[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.midi_channel_offset[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.track_glide[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.cc_mod[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_STEP_MAX; ++i) object.cc_mod_global[i] = reader.readUint8();
    object.mute_mask = reader.readUint8();
    object.random_mask = reader.readUint8();
    object.solo_mask = reader.readUint8();
    object.lfo_mask = reader.readUint8();
    object.mod_ext_mask = reader.readUint8();
    object.mod_ext_mask = reader.readUint8();
    deserialize_SEQ_param_buffer_s(reader, object.seq_params);
    object.crc_check = reader.readUint32();
}

inline void deserialize_SEQ_step_mono_buffer_s(oxitools::DataReader & reader, SEQ_step_mono_buffer_s & object)
{
    object.step = reader.readInt8();
    object.velocity = reader.readUint8();
    object.octave = reader.readInt8();
    object.gate = reader.readUint8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.modulation[i] = reader.readUint8();
    object.repeat = reader.readUint8();
    object.prob = reader.readInt8();
    object.offset = reader.readInt8();
    object.param = reader.readInt8();
}

inline void deserialize_SEQ_mono_buffer_s(oxitools::DataReader & reader, SEQ_mono_buffer_s & object)
{
    object.type = reader.readUint8();
    reader.readUint8(); // aligment byte
    object.length = reader.readUint16();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) deserialize_SEQ_step_mono_buffer_s(reader, object.steps[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod[i] = reader.readUint8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod_global[i] = reader.readUint8();
    //object.word_align = reader.readUint16();
    object.crc_check = reader.readUint32();
}

inline void deserialize_SEQ_chord_buffer_s(oxitools::DataReader & reader, SEQ_chord_buffer_s & object)
{
    object.type = reader.readUint8();
    reader.readUint8(); // aligment byte
    object.length = reader.readUint16();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) deserialize_SEQ_step_mono_buffer_s(reader, object.steps[i]);
    object.arp_type = reader.readUint8();
    object.arp_oct = reader.readUint8();
    object.arp_distance = reader.readUint8();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) object.chord_type[i] = reader.readUint8();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) object.chord_inversion[i] = reader.readUint8();
    object.chord_bass = reader.readUint8();
    object.chord_spread = reader.readUint8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod[i] = reader.readUint8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod_global[i] = reader.readUint8();
    deserialize_SEQ_param_buffer_s(reader, object.seq_params);
    object.crc_check = reader.readUint32();
}

inline void deserialize_SEQ_step_poly_buffer_s(oxitools::DataReader & reader, SEQ_step_poly_buffer_s & object)
{
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.step[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.vel[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.oct[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.gate[i] = reader.readUint8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.repeat[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.prob[i] = reader.readInt8();
    for (int i = 0; i < SEQ_NOTES_POLY_MAX; ++i) object.offset[i] = reader.readInt8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.modulation[i] = reader.readUint8();
    object.param = reader.readInt8();
    object.size = reader.readInt8();
}

inline void deserialize_SEQ_poly_buffer_s(oxitools::DataReader & reader, SEQ_poly_buffer_s & object)
{
    object.type = reader.readUint8();
    reader.readUint8(); // aligment byte
    object.length = reader.readUint16();
    for (int i = 0; i < SEQ_STEPS_MAX; ++i) deserialize_SEQ_step_poly_buffer_s(reader, object.steps[i]);
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod[i] = reader.readUint8();
    for (int i = 0; i < SEQ_MOD_NUM; ++i) object.cc_mod_global[i] = reader.readUint8();
    deserialize_SEQ_param_buffer_s(reader, object.seq_params);
    //object.word_align = reader.readUint16();
    object.crc_check = reader.readUint32();
}

// MARK: -

inline void deserialize_SEQ_Preset_s(oxitools::DataReader & reader, SEQ_Preset_s & object)
{
    object.is_empty = reader.readUint8();
    object.color_variation = reader.readInt8();
    for (int i = 0; i < SEQ_PRESET_NAME_LEN; ++i) object.preset_name[i] = reader.readChar();
}

inline void deserialize_ARR_Pattern_s(oxitools::DataReader & reader, ARR_Pattern_s & object)
{
    object.repeat = reader.readInt8();
    object.preset_num = reader.readUint8();
    object.program_change = reader.readInt8();
}

inline void deserialize_ARR_Track_s(oxitools::DataReader & reader, ARR_Track_s & object)
{
    for (int i = 0; i < ARR_PATTERN_NUM; ++i)
        deserialize_ARR_Pattern_s(reader, object.patterns[i]);
}

inline void deserialize_ARR_Song_s(oxitools::DataReader & reader, ARR_Song_s & object)
{
    for (int i = 0; i < ARR_SONG_NAME_LEN; ++i)
        object.song_name[i] = reader.readChar();
    for (int i = 0; i < ARR_TRACKS_NUM; ++i)
        deserialize_ARR_Track_s(reader, object.tracks[i]);
}

inline void deserialize_CV_assign_s(oxitools::DataReader & reader, CV_assign_s & object)
{
    for (int i = 0; i < CV_SIZE; ++i) object.cv_seq[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.cv_out[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.cv_voice[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.cv_config_1[i] = reader.readInt8();
    for (int i = 0; i < CV_SIZE; ++i) object.cv_config_2[i] = reader.readInt8();
    for (int i = 0; i < CV_SIZE; ++i) object.gate_seq[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.gate_out[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.gate_voice[i] = reader.readUint8();
    for (int i = 0; i < CV_SIZE; ++i) object.gate_config[i] = reader.readUint8();
}

inline void deserialize_PROJ_buffer_s(oxitools::DataReader & reader, PROJ_buffer_s & object)
{
    object.proj_version = reader.readUint16();


    if (object.proj_version > ProjectVersion::Latest) {
        object.proj_version = ProjectVersion::Version0;
    }

    for (int i = 0; i < PROJ_NAME_LEN; ++i) object.proj_name[i] = reader.readChar();
    object.proj_num = reader.readUint8();
    uint8_t byte_align = reader.readUint8();
    if (object.proj_version == ProjectVersion::Version0)
    {
        reader.read(object.tempo, sizeof(object.tempo));
    } else {
        object.tempo = reader.readUint16();
    }
    for (int i = 0; i < ARR_TRACKS_NUM; ++i) {
        for (int j = 0; j < ARR_PATTERN_NUM; ++j) {
            deserialize_SEQ_Preset_s(reader, object.presets[i][j]);
        }
    }
    object.loaded_song = reader.readUint8();
    for (int i = 0; i < ARR_SONG_NUM; ++i) {
        deserialize_ARR_Song_s(reader, object.songs[i]);
    }
    deserialize_CV_assign_s(reader, object.cv_assign);
    for (int i = 0; i < ARR_TRACKS_NUM; ++i) object.loaded_preset[i] = reader.readUint8();
    for (int i = 0; i < ARR_TRACKS_NUM; ++i) object.active_seq[i] = reader.readUint8();

    if (object.proj_version == ProjectVersion::Version0)
    {
        object.arr_launch_master = 0xFF;
    } else {
        object.arr_launch_master = reader.readUint8();
    }


}

}

#endif // PARSER_H
