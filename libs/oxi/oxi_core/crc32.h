#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

#pragma once

inline uint32_t crc32(uint32_t * data, int data_len)
{
    const uint32_t POLY = 0x4C11DB7;
    uint32_t init = 0XFFFFFFFF;
    uint32_t crc_ = 0;

    for (int index = 0; index < data_len; index++) {
        crc_ = init ^ data[index];
        for (int i = 0; i < 32 ; i++) {
            if ((0x80000000 & crc_) != 0) {
                crc_ = (crc_ << 1) ^ POLY;
            } else {
                crc_ = (crc_ << 1);
            }
        }
        init = crc_;
    }
    return crc_;
}


#endif // CRC32_H
