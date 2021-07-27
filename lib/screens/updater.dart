import 'dart:async';
import 'dart:developer' as dev;
import 'dart:io';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter/scheduler.dart';
import 'package:provider/provider.dart';

import 'package:flutter_midi_command/flutter_midi_command.dart';
import 'package:flutter_midi_command/flutter_midi_command_messages.dart';

import 'package:oxi_companion_flttr/models/midi_device_model.dart';
import 'oxi_one.dart' as oxiOne;

import 'package:oxi_companion_flttr/common/oxi_theme.dart';

class Updater extends StatefulWidget {
  late MidiDevice? _device;

  // Updater(this._syxData);

  @override
  _UpdaterState createState() => _UpdaterState();
}

class _UpdaterState extends State<Updater> with TickerProviderStateMixin {

  // late AnimationController controller;
  // late StreamSubscription<Uint8List> _txSubscription;
  MidiCommand _midiCommand = MidiCommand();
  double _progress = 0;

  @override
  void initState() {
    super.initState();
    // controller = AnimationController(
    //     vsync: this,
    //     duration: Duration(seconds: 7),
    // )..addListener(() {
    //   setState(() { });
    // });
    // _txSubscription = _midiCommand.onMidiDataSent.listen((event) {
    //   dev.log("sending stuff!!!");
    //   dev.log(event.buffer.toString());
    // });


    // WidgetsBinding.instance?.addPostFrameCallback((_) async {
    //   final Uint8List _syxData = ModalRoute.of(context)?.settings.arguments as Uint8List;
    //   send(_syxData).then((value) => null).whenComplete(() => Navigator.of(context).pushNamedAndRemoveUntil('/update_succeeded', (_)=>(false)));
    // });
    Future.delayed(Duration.zero, () async {
      final Uint8List _syxData = ModalRoute.of(context)?.settings.arguments as Uint8List;
      send(_syxData).then((value) => null).whenComplete(() => Navigator.of(context).pushNamedAndRemoveUntil('/update_succeeded', (_)=>(false)));
    });
  }

  @override
  void dispose() {
    // controller.dispose();
    _midiCommand.disconnectDevice(oxiOne.OxiOne().device);
    super.dispose();
  }

  Future<void> send(Uint8List _syxData) async {
    _midiCommand.connectToDevice(oxiOne.OxiOne().device);

    int chunkSize = _syxData.indexWhere((element) => element == 0xf7) + 1;
    int chunks = (_syxData.lengthInBytes % chunkSize == 0) ?
    _syxData.lengthInBytes ~/ chunkSize :
    _syxData.lengthInBytes ~/ chunkSize + 1;
    dev.log("sending $chunks chunks of data with size $chunkSize");

    for (int chunkCount = 1, start = 0, end = chunkSize; chunkCount <= chunks; chunkCount++ ){
      if (chunkCount == chunks){
        dev.log("sending last chunk: ");
        _midiCommand.sendData(_syxData.sublist(start));
      } else {
        _midiCommand.sendData(_syxData.sublist(start, end));
        dev.log("last byte of this chunk $chunkCount is ${_syxData[end-1]}");
      }
      start = chunkCount*chunkSize;
      end = start+chunkSize;
      sleep(Duration(milliseconds: 190));
      if (chunkCount == chunks) {
        return;
      } else {
        setState(() {
          _progress = 100/chunks*chunkCount;
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {

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
                value: _progress,
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
      ),
    );
  }

  void _clickDummy() {
    dev.log("klick klock");
  }
}