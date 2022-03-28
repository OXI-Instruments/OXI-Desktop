#include <Qt>

namespace  MidiMsg {


/*! Enumeration of MIDI types */
typedef enum
{
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch/key-pressure
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch/pressure
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    SysExEnd		  = 0xF7,
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
} MIDI_Msg_e;

}
//#define OXI_INSTRUMENTS_MIDI_ID	0x00, 0x22, 0x03,
//#define OXI_ONE_ID	0x00, 0x02,
//#define OXI_ONE_BLUETOOTH_ID	0x00, 0x02,
#include "OXI_SYSEX_MSG.h"

static const uint8_t sysex_header[] = {
    0xf0,  // <SysEx>
    OXI_INSTRUMENTS_MIDI_ID  // Manufacturer ID for Mutable instruments.
    OXI_ONE_ID  // Product ID for "any other project".
};
