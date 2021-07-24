library oxi_one_companion.oxi_one;

import 'package:flutter_midi_command/flutter_midi_command.dart';

class OxiOne {

  static final OxiOne _oxiOne = OxiOne._internal();

  OxiOne._internal();

  factory OxiOne() {
    return _oxiOne;
  }

  MidiDevice? _device;

  MidiDevice get device => _device!;
  set device (MidiDevice? dev) => _device = dev;
}