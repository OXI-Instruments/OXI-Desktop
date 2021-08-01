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

  late AnimationController _controller;
  MidiCommand _midiCommand = MidiCommand();
  double _progress = 0;
  late Timer _sendLoop;
  late Uint8List _syxData;
  List<Uint8List> _chunks = [];
  int _chunkCount = 0;

  @override
  void initState() {
    super.initState();

    _controller = AnimationController(
        vsync: this,
        lowerBound: 0,
        upperBound: 1,
    )..addListener(() {
      setState(() { });
    });

    WidgetsBinding.instance?.addPostFrameCallback((_) async {
      _syxData = ModalRoute.of(context)?.settings.arguments as Uint8List;
      _prepareChunks();
      _midiCommand.connectToDevice(oxiOne.OxiOne().device);
      _sendLoop = Timer.periodic(Duration(milliseconds: 250), send);
    });
  }

  @override
  void dispose() {
    // controller.dispose();
    _midiCommand.disconnectDevice(oxiOne.OxiOne().device);
    super.dispose();
  }

  void _prepareChunks(){
    int chunkSize = _syxData.indexWhere((element) => element == 0xf7) + 1;
    int chunks = (_syxData.lengthInBytes % chunkSize == 0) ?
    _syxData.lengthInBytes ~/ chunkSize :
    _syxData.lengthInBytes ~/ chunkSize + 1;
    for (int chunkCount = 1, start = 0, end = chunkSize; chunkCount <= chunks; chunkCount++ ) {
      if (chunkCount == chunks){
        _chunks.add(_syxData.sublist(start));
      } else {
        _chunks.add(_syxData.sublist(start, end));
      }
      start = chunkCount*chunkSize;
      end = start+chunkSize;
    }
  }

  void send(Timer timer) {
    _midiCommand.sendData(_chunks.elementAt(_chunkCount));
    if (_chunkCount == _chunks.length-1) {
      timer.cancel();
      Navigator.of(context).pushNamedAndRemoveUntil('/update_succeeded', (_)=>(false));
    } else {
      double progress = 1/_chunks.length*_chunkCount;
      setState(() {
        _progress = progress;
      });
      _controller.animateTo(progress, duration: Duration(milliseconds: 180));
      _chunkCount++;
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
      body: WillPopScope(
        onWillPop: () async {
          return false;
        },
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              Container(
                alignment: Alignment.center,
                height: 80.0,
                padding: const EdgeInsets.only(left: 140, right: 140),
                child: LinearProgressIndicator(
                  value: _progress,
                  minHeight: 8,
                  valueColor: ColorTween(begin: OxiTheme.purple, end: OxiTheme.cyan).animate(
                    CurvedAnimation(
                      parent: _controller,
                      curve: Curves.linear
                    )
                  ),
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
      ),
    );
  }

  void _clickDummy() {
    dev.log("klick klock");
  }
}