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
typedef enum OXI_SYSEX_CAT_e {
    MSG_CAT_FW_UPDATE = 0,
    MSG_CAT_BLE = 1,
    MSG_CAT_MIDI = 2,
    MSG_CAT_SYSTEM = 3,
    MSG_CAT_ANALOG = 4, // Factory?
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
    MSG_SYSTEM_SW_VERSION = 0,
    MSG_SYSTEM_HW_VERSION,
    MSG_SYSTEM_GET_CALIB_DATA,
    MSG_SYSTEM_SEND_CALIB_DATA,
    MSG_SYSTEM_SIZE,

    MSG_SYSTEM_ACK = 0x53,
    MSG_SYSTEM_NACK = 0x54,
    MSG_SYSTEM_FACTORY_TEST = 0x55,
    MSG_SYSTEM_FACTORY_TEST_NOK = 0x56,

    MSG_SYSTEM_MEM_RESET = 0x61,
    /* ----- */
} OXI_SYSEX_SYSTEM_e;

typedef enum {
    MSG_FW_UPDT_OXI_ONE = 0x31,
    MSG_FW_UPDT_OXI_BLE = 0x32,
    MSG_FW_UPDT_OXI_SPLIT = 0x33,
    /* ----- */
    MSG_FW_UPDT_READY = 0x50,
    MSG_FW_UPDT_STARTED = 0x51, // bootloader started
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
    MSG_PROJECT_GET_PROJ_HEADER = 0,
    MSG_PROJECT_SEND_PROJ_HEADER = 1,
    MSG_PROJECT_GET_PATTERN = 2,
    MSG_PROJECT_SEND_PATTERN = 3,
    MSG_PROJECT_DELETE_PROJECT = 4,
    MSG_PROJECT_DELETE_PATTERN = 5,
    /* ----- */
    MSG_PROJECT_SIZE,

    MSG_PROJECT_ACK = 0x53,
    MSG_PROJECT_NACK = 0x54,
} OXI_SYSEX_PROJECT_e;




#endif /* OXI_SYSEX_MSG_H_ */
