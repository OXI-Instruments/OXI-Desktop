#ifndef PROJECT_H
#define PROJECT_H



#include "QMainWindow"

#include "SYSEX_APP.h"
#include "crc32.h"
#include "Nibble.h"
#include "ProjectVersion.h"
#include "Parser.h"
#include "ProjectData.h"

using namespace oxitools;

class Project
{
public:
    Project();
    void readProject(QByteArray &buff);
    void readPattern(QByteArray &buff, SEQ_e seq_index, uint16_t pattern_index);

private:
    uint16_t version_ = ProjectVersion::Latest;
    char version2_ = '0';
    char proj_name[PROJ_NAME_LEN];
    uint8_t proj_num;
    uint8_t byte_align;
    uint16_t tempo;
    SEQ_Preset_s presets[4][ARR_PATTERN_NUM];
    uint8_t loaded_song;
    ARR_Song_s songs[ARR_SONG_NUM];
    CV_assign_s cv_assign;
    uint8_t loaded_preset[4];
    uint32_t crc_check;

public:
    // ProjectData project_data_;
    PROJ_buffer_s project_data_;
//    SEQ_param_buffer_s sequencers[4];
    ProjectPatternData pattern_data_[4][16];



};

#endif // PROJECT_H
