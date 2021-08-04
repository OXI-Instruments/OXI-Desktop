[[_TOC_]]

# Overview

**As of March 2021 this document is in draft state. Everything can change at any time ;)**

This document describes the way the app communicates with the Oxi One. This protocol uses MIDI SysEx messages for two-way communication.
```
OXI Instruments Manufacturer ID: \0x00 \0x21 \0x5B
OXI One ID: \0x00 \0x01
```


# Structure

The communication is structured in a request-response model. This means that every message sent to the Oxi One will be answered by the device with an appropriate response.

## Requests

Messages are encapsulated into the well-known SysEx message format. This means that the first 6 bytes as well as the last byte are defined by the official standard and therefore static.

|  | SysEx header | manufacturer ID | product ID | message category | message | options | SysEx Footer |
| ---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte number** | 1 | 2, 3, 4 | 5, 6 | 7 | 8 | 9 - N | N + 1 |
| **Bytes** | f0 | 00 21 5B | 00 01 | XX | XX | XX XX | f7 |

In the Oxi One protocol we defined three more fields: *message category*, *message* and *options*.

## Response

Unless stated otherwise, if there's no data requested from the Oxi (ie. example when device settings are changed) the Oxi responds to it in the following way:

|  | SysEx header | manufacturer ID | product ID | message category | message | status | SysEx Footer |
| ---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte number** | 1 | 2, 3, 4 | 5, 6 | 7 | 8 | 9 | 10 |
| **Bytes** | f0 | 00 21 5B | 00 01 | XX | XX | XX | f7 |

**message category** and **message** are equal to the request.
**status** represents the success of the requested operation.

# Messages

## General

### Device probe

### Device information

## Firmware update

### Current implementation

The current sequence to update the firmware is the following:

  1. Oxi One has to be put in update mode by the user on the device itself
  2. Oxi One: listen to firmware data
  3. Oxi One: reboot on successful update

### Target implementation

  1. Application: Send update request
  2. Oxi One: Displays user confirmation on the screen (timeout?)
  3. User: confirms/refuses update
  4. Oxi One: Responds with user decision
  (assuming that the user agreed)
  5. Oxi One: Reboots into update mode
  6. Oxi One: Sends update-ready message
  7. Application: Sends N-th chunk of the firmware data
  8. Oxi One: Replies with ACK
  (go to 7. until all chunks are sent and acknowledged)
  9. Oxi One: reboot into normal mode

## Global settings

The following messages are used to request and set certain options.

### Ignore transport controls

Ignore transport controls on the interfaces MIDI, Bluetooth LE or the analog clock input.

Supported states:

| interface | message category | message | option |
| :---: | :---: | :---: | :---: |
| MIDI | 01 | 10 | Off: 00 |
|  |  |  | On: 01 |
| BT LE | 01 | 11 | Off: 00 |
|  |  |  | On: 01 |
| Analog | 01 | 12 | Off: 00 |
|  |  |  | On: 01 |

## Project management

### Project list

#### Request

| message category | message |
| :---: | :---: |
| 02 | 00 |

#### Response

| message category | message | project name 1 | project name N |
| :---: | :---: | :---: | :---: | 
| 1 | 2 | 3 + 16 | 19 + 16...|
| 02 | 00 | XX | YY |


### Sequence list of a project
The following C/C++ typedefs describe the data structure to transfer project information between the app and the OXI One.

```
typedef struct {
	char name[14];
} SYSEX_Name_s;

typedef struct
{
  uint8_t project_num;
	char project_name[12];
	uint16_t tempo;
	SYSEX_Name_s pattern_name[4][16];
	SYSEX_Name_s song_name[16];
	SYSEX_Name_s cc_name[128];
} SYSEX_ProjInfo_s;
```


