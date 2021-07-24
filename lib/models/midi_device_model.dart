import 'dart:async';
import 'dart:developer' as dev;

import 'package:flutter/material.dart';
import 'package:flutter_midi_command/flutter_midi_command.dart';

class MidiDeviceModel extends ChangeNotifier {
  List<MidiDevice>? _devices = [];
  final MidiCommand _midiCommand = MidiCommand();

  StreamSubscription<String>? _devSubscription;

  List<MidiDevice>? get devices => _devices;

  MidiDeviceModel() {
    dev.log("at least this is working...");
    _devSubscription = _midiCommand.onMidiSetupChanged!.listen((event) {
      dev.log(event);
      dev.inspect(this);
      _midiCommand.devices.then((value) => _devices=value).whenComplete(() => notifyListeners());
    });
  }

  MidiDevice? getDevice (String name) {
    return _devices!.singleWhere((device) => device.name == name);
  }
}