#ifndef FILETYPES_H
#define FILETYPES_H

#pragma once

enum FileType {
    FILE_SYSEX,
    FILE_PROJECT,
    FILE_PATTERN,
    FILE_CALIBRATION,
    FILE_SIZE,
};

extern const char * FileExtension[FILE_SIZE];

#endif // FILETYPES_H
