[[_TOC_]]

# Overview

**As of March 2021 this document is in draft state. Everything can change at any time ;)**

This document describes the way the app communicates with the Oxi One. This protocol currently uses MIDI SysEx messages for two-way communication.

# Structure

The communication is structured in a request-response model. This means that every message sent to the Oxi One will be answered by the device with an appropriate response.

| Bytes |
| ---- |
| SysEx header | manufacturer ID | product ID | message category | message | options | SysEx Footer |
| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | ... | last |
| f0 | 00 | 22 | 42 | 08 | 15 | 01 | 01 | 02 | 00 | f7 |


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
  7. Application: Sends Nth chunk of the firmware data
  8. Oxi One: Replies with ACK
  (go to 7. until all chunks are sent and acknowledged)
  9. Oxi One: reboot into normal mode

## Global settings

The following messages are used to request and set certain options.

### Ignore transport constrols

Ignore transport controls on the interfaces MIDI, Bluetooth LE or the analog clock input.


| Ignore transport controls | 02 01 00 | 02 01 00 |


## Project management
