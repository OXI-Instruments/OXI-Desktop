import 'dart:async';
import 'dart:developer' as dev;
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'package:flutter_midi_command/flutter_midi_command.dart';
import 'package:flutter_midi_command/flutter_midi_command_messages.dart';

import 'package:oxi_companion_flttr/models/midi_device_model.dart';
import 'oxi_one.dart' as oxiOne;

import 'package:oxi_companion_flttr/common/oxi_theme.dart';

class Updater extends StatefulWidget {
  late MidiDevice? _device;

  // Updater(this._device)

  @override
  _UpdaterState createState() => _UpdaterState();
}

class _UpdaterState extends State<Updater> with TickerProviderStateMixin {

  late AnimationController controller;
  late StreamSubscription<Uint8List> _txSubscription;
  MidiCommand _midiCommand = MidiCommand();

  @override
  void initState() {
    super.initState();
    controller = AnimationController(
        vsync: this,
        duration: Duration(seconds: 7),
    )..addListener(() {
      setState(() { });
    });
    _txSubscription = _midiCommand.onMidiDataSent.listen((event) {
      dev.log("sending stuff!");
      dev.inspect(event.buffer.toString());
    });
    _midiCommand.connectToDevice(oxiOne.OxiOne().device);
  }

  @override
  void dispose() {
    controller.dispose();
    _midiCommand.disconnectDevice(oxiOne.OxiOne().device);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {

    // dev.log(_device.name);
    // MidiCommand().connectToDevice(device);
    NoteOnMessage(channel: 0, note: 64, velocity: 120).send();
    sleep(Duration(milliseconds: 200));
    NoteOffMessage(channel: 0, note: 64, velocity: 120).send();
    SysExMessage(value: 0x1939859348349384);
    return Scaffold(
      appBar: OxiAppBar(
        title: Text(
          "Installing update",
          style: TextStyle(
              fontWeight: FontWeight.w100,
              fontSize: 24
          ),
        ),
        disableNav: true
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Container(
              alignment: Alignment.center,
              height: 80.0,
              padding: const EdgeInsets.only(left: 40, right: 40),
              child: LinearProgressIndicator(
                value: controller.value,
                minHeight: 8,
              ),
            ),
            Text(
              "Update in progress, hold your breath!",
              style: TextStyle(
                fontSize: 12.0
              ),
            )
          ],
        ),
      )
    );
  }

  void _clickDummy() {
    dev.log("klick klock");
  }
}