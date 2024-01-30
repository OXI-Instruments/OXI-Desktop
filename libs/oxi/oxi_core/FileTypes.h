#ifndef FILETYPES_H
#define FILETYPES_H

#pragma once

enum FileType {
    FILE_SYSEX,
    FILE_PROJECT,
    FILE_PATTERN,
    FILE_CALIBRATION,
    FILE_MIDI,
    FILE_SIZE,
};

constexpr const char * FileExtension[FILE_SIZE] {
    ".syx",
    ".oxipro",
    ".oxipat",
    ".oxical",
    ".mid",
};

#endif // FILETYPES_H
