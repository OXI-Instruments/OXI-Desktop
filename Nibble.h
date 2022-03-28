#ifndef NIBBLE_H
#define NIBBLE_H

#pragma once

#include <mainwindow.h>

inline void DeNibblize(std::vector<uint8_t> &buf, std::vector<uint8_t> &data, int offset)
{
    int count = 0;
    for (uint32_t it = offset; it < data.size(); ++it)
    {
        uint8_t byte = data.at(it);

        if ((count & 1) == 0)
        {
            uint8_t byte_ = (byte << 4) & 0xF0; // byte high
            buf.push_back(byte_);
        } else {
            int size = buf.size() - 1;
            buf[size] |= byte & 0xF;
        }
        count++;
    }
}

inline void Nibblize(std::vector<uint8_t> &buf, uint8_t data[], int length)
{
    for (int it = 0; it < length; ++it)
    {
        uint8_t byte = data[it];
        uint8_t byte_h = (byte >> 4) & 0x0F;
        buf.push_back(byte_h);
        uint8_t byte_l = byte & 0xF;
        buf.push_back(byte_l);
    }
}


#endif // NIBBLE_H
