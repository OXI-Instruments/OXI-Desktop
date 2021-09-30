/********************************************************

         @@@@@@@@@@           @@@@@@@@@@@@@@@@@@@@@@
     @@@@          @@@
   @@@               @@@      @@@                @@@
  @@                   @@       @@@            @@@
 @@@                    @@         @@@      @@@
 @@          @@         @@            @@  @@
 @@@                    @@             @@@@
  @@                   @@            @@@  @@@
   @@@               @@@          @@@        @@@
     @@@@          @@@         @@@             @@@
         @@@@@@@@@@           @@                 @@@

 *********************************************************/
/*
 * OXI SYSEX PROTOCOL
 *
 *  Created on: Jul 7, 2021
 *      Author: ManuelVr
 */

#ifndef OXI_SYSEX_MSG_H_
#define OXI_SYSEX_MSG_H_

#define OXI_INSTRUMENTS_MIDI_ID		0x00, 0x21, 0x5B,
#define OXI_ONE_ID					0x00, 0x01,
#define OXI_ONE_BLE_ID		0x00, 0x03,


/* SYSEX CATHEGORIES */
typedef enum {
    MSG_CAT_FW_UPDATE = 0,
    MSG_CAT_BLE = 1,
    MSG_CAT_MIDI = 2,
    MSG_CAT_SYSTEM = 3,
    MSG_CAT_ANALOG = 4,
    MSG_CAT_PROJECT = 5,
    /* ----- */
    MSG_CAT_SIZE,
} OXI_SYSEX_CAT_e;


/* MESSAGE SUB CATHEGORIES */
typedef enum {
    MSG_ANAL_GOTO_CALIB = 0,
    /* ----- */
    MSG_ANAL_SIZE,
} OXI_SYSEX_ANALOG_e;

typedef enum {
    MSG_FW_UPDT_OXI_ONE = 0x31,
    MSG_FW_UPDT_OXI_BLE = 0x32,
    MSG_FW_UPDT_OXI_SPLIT = 0x33,
    /* ----- */
    MSG_FW_UPDT_READY = 0x50,
    MSG_FW_UPDT_STARTED = 0x51,
    MSG_FW_UPDT_EXIT = 0x52,
    MSG_FW_UPDT_ACK = 0x53,
    MSG_FW_UPDT_NACK = 0x54,
    MSG_FW_UPDT_RESTART = 0x55,
    MSG_FW_UPDT_SEND_DATA = 0x56,
    MSG_FW_UPDT_SIZE = 0x57,
    /* ----- */
    MSG_FW_UPDT_NEXT_PACKAGE = 0x7E,
    MSG_FW_UPDT_SYSTEM_RESET = 0x7F,
} OXI_SYSEX_FW_UPDT_e;

typedef enum {
    MSG_BLE_TURN_OFF = 0,
    MSG_BLE_TURN_ON,
    MSG_BLE_CONNECTED,
    MSG_BLE_DISCONNECTED,
    /* ----- */
    MSG_BLE_SIZE,
} OXI_SYSEX_BLE_e;

typedef enum {
    MSG_MIDI_USB_THRU = 0,
    MSG_MIDI_TRS_THRU,
    /* ----- */
    MSG_MIDI_SIZE,
} OXI_SYSEX_MIDI_e;

typedef enum {
    MSG_PROJECT_GET_INFO = 0, // Only supported for the Loaded project
    MSG_PROJECT_SET_INFO,	// Only supported for the Loaded project

    MSG_PROJECT_GET_PATTERN,	// Next byte is the pattern index // Only supported for the patterns the Loaded project
    MSG_PROJECT_SET_PATTERN,	// Next byte is the pattern index // Only supported for the patterns the Loaded project
    /* ----- */
    MSG_PROJECT_SIZE,
} OXI_SYSEX_PROJECT_e;




#endif /* OXI_SYSEX_MSG_H_ */
