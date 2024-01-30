#ifndef SEQ_SCALE_H
#define SEQ_SCALE_H

#pragma once


#include "stdint.h"

typedef struct
{
    const char * scale_text;
    const uint8_t scale_steps[12];
    const uint8_t scale_notes;
} SEQ_scale_s;


typedef enum
{
    SCALE_START = 0,
    SCALE_HARMONIZER = 0,
    SCALE_Chromatic,
    SCALE_Major,
    SCALE_Minor,
    SCALE_Dorian,
    SCALE_Frigyan,
    SCALE_Lydian,
    SCALE_Mixolydian,
    SCALE_Locryan,
    SCALE_PentatonicMajor,
    SCALE_PentatonicMinor,
    SCALE_Harmonic_Minor,
    SCALE_Melodic_Minor,
    Blues_1,
    Arabian,
    SCALE_Romanian,
    Balinese,
    Hungarian_1,
    Hungarian_2,
    Oriental,
    Raga_Todi,
    Chinese,
    Japanese_1,
    Japanese_2,
    Persian,
    Diminished,
    SCALE_Phrygian_Dominant,
    SCALE_Byzantine,
    SCALE_Spanish_Phrygian,
    SCALE_Neapolitan_Minor,
    SCALE_Neapolitan_Major,
    SCALE_Ukrainian_Dorian, // Dorian Sharp Four
    SCALE_SIZE,
    SCALE_RESET
} SEQ_scale_e;


static const SEQ_scale_s scale_array[SCALE_SIZE] = {
        {"HARMON", 		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, 	12},
        {"Chromatic", 	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}, 	12},
        {"Major", 		{0, 2, 4, 5, 7, 9, 11, 12, 14, 0, 0, 0}, 	7},
        {"Minor", 		{0, 2, 3, 5, 7, 8, 10, 12, 14, 0, 0, 0}, 	7},
        {"Dorian", 		{0, 2, 3, 5, 7, 9, 10, 12, 14, 0, 0, 0}, 	7},
        {"Phrygian", 	{0, 1, 3, 5, 7, 8, 10, 12, 13, 0, 0, 0}, 	7},
        {"Lydian", 		{0, 2, 4, 6, 7, 9, 11, 12, 14, 0, 0, 0}, 	7},
        {"Mixolydian", 	{0, 2, 4, 5, 7, 9, 10, 12, 14, 0, 0, 0}, 	7},
        {"Locrian", 	{0, 1, 3, 5, 6, 8, 10, 12, 13, 0, 0, 0}, 	7},
        {"PentaMaj", 	{0, 2, 4, 7, 9, 12, 14, 16, 19, 0, 0, 0}, 	5},
        {"PentaMin", 	{0, 3, 5, 7, 10, 12, 15, 17, 19, 0, 0, 0}, 	5},
        {"Harm Min", 	{0, 2, 3, 5, 7, 8, 11, 12, 14, 0, 0, 0}, 	7},	/* Harmo minor */
        {"Melodic", 	{0, 2, 3, 5, 7, 9, 11, 12, 14, 0, 0, 0}, 	7},	/* Melodic ascending */
        {"Blues", 		{0, 3, 5, 6, 7, 10, 12, 15, 17, 0, 0, 0}, 	6},
        {"Arabian", 	{0, 2, 4, 5, 6, 8, 10, 12, 14, 0, 0, 0}, 	7},
        {"Romanian", 	{0, 2, 3, 6, 7, 9, 10, 12, 17, 0, 0, 0}, 	7},
        {"Balinese", 	{0, 2, 3, 7, 8, 12, 14, 15, 19, 0, 0, 0}, 	5},
        {"Hungar 1 ", 	{0, 2, 3, 6, 7, 8, 11, 12, 14, 0, 0, 0}, 	7},
        {"Hungar 2 ", 	{0, 3, 4, 6, 7, 9, 10, 12, 15, 0, 0, 0}, 	7},
        {"Oriental", 	{0, 1, 4, 5, 6, 8, 10, 12, 13, 0, 0, 0}, 	7},
        {"RagaToe", 	{0, 1, 3, 6, 7, 8, 10, 12, 13, 0, 0, 0}, 	7},
        {"Chinese", 	{0, 4, 6, 7, 11, 12, 16, 18, 19, 0, 0, 0}, 	5},
        {"Japan 1", 	{0, 1, 5, 7, 8, 12, 13, 17, 19, 0, 0, 0}, 	5},
        {"Japan 2", 	{0, 2, 5, 7, 8, 12, 14, 17, 19, 0, 0, 0}, 	5},
        {"Persian", 	{0, 1, 4, 6, 8, 11, 12, 15, 17, 0, 0, 0}, 	6},
        {"Diminish",	{0, 2, 3, 5, 6, 8, 9, 11, 12, 0, 0, 0}, 	8},
        {"PhrygDom",	{0, 1, 4, 5, 7, 8, 10, 12, 13, 0, 0, 0}, 	7},
        {"Byzantine",	{0, 1, 4, 5, 7, 8, 11, 12, 13, 0, 0, 0}, 	7},
        {"SpainPhry",	{0, 1, 3, 4, 5, 7, 8, 10, 12, 0, 0, 0}, 	8},
        {"NeapolMin",	{0, 1, 3, 5, 7, 8, 11, 12, 13, 0, 0, 0}, 	7},
        {"NeapolMaj",	{0, 1, 3, 5, 7, 9, 11, 12, 13, 0, 0, 0}, 	7},
        {"UkranDori",	{0, 2, 3, 6, 7, 9, 10, 12, 14, 0, 0, 0}, 	7},
};


int SEQ_GetScaleStep(unsigned int scale_index, int step)
{
    if (scale_index > SCALE_SIZE - 1) {
        return 0;
    }
    if (step < 0) {
        return 0;
    }
    if (step > 11) return 12;
    return scale_array[scale_index].scale_steps[step];
}

inline uint8_t SEQ_GetScaleNumNotes(unsigned int scale_index)
{
    if (scale_index > SCALE_SIZE - 1) {
        return 1;
    }
    return scale_array[scale_index].scale_notes;
}


#endif // SEQ_SCALE_H
