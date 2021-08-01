import 'dart:async';
import 'dart:developer' as dev;

import 'package:flutter/widgets.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'package:flutter_midi_command/flutter_midi_command.dart';

import 'package:oxi_companion_flttr/common/oxi_theme.dart';
import 'package:oxi_companion_flttr/models/midi_device_model.dart';
import 'oxi_one.dart' as oxiOne;


class ProjectSelector extends StatefulWidget {
  @override
  _ProjectSelectorState createState() => _ProjectSelectorState();
}

class _ProjectSelectorState extends State<ProjectSelector> {

  MidiCommand _midiCommand = MidiCommand();
  late StreamSubscription<MidiPacket> _devSubscription;


  @override
  void initState() {
    // TODO: implement initState
    super.initState();
    _devSubscription = _midiCommand.onMidiDataReceived!.listen((event) {
      setState(() {});
      dev.log('');
      // dev.inspect(this);
    });
  }

  @override
  void dispose() {
    _devSubscription.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: OxiAppBar(
        title: Text(
          "Please select your device",
          style: TextStyle(fontWeight: FontWeight.w100, fontSize: 24),
        ),
      ),
      body: FutureBuilder(
        builder: (BuildContext context, AsyncSnapshot snapshot) {
            dev.inspect(snapshot);
            if (snapshot.hasData && snapshot.data != null) {
              return _buildSuggestions(snapshot.data as List<MidiDevice>);
            } else {
              return _waiting();
            }
          },
        future: _midiCommand.devices,
      )
    );
  }

  Widget _waiting() {
    return Center(
      child: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: CircularProgressIndicator(),
          ),
          Text("Searching for devices")
        ],
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
      ),
    );
  }

  Widget _buildSuggestions(List<MidiDevice> devices) {
    // var devices = context.watch<MidiDeviceModel>();
    return ListView.builder(
      itemBuilder: (context, index) {
        return _buildDevRow(devices[index]);
      },
      itemCount: devices.length,
    );

    // return ListView.builder(itemBuilder: (ctx, i) {
    //   if (i.isOdd) return const Divider(color: Color.fromARGB(255, 240, 240, 248), height: .2,);
    //
    //   final index = i ~/ 2;
    //   if (index >= _suggestions.length) {
    //     _suggestions.addAll(generateWordPairs().take(10));
    //   }
    //   return _buildRow(_suggestions[index]);
    // }, padding: const EdgeInsets.only(left: 230, right: 230),);
  }

  Widget _buildDevRow(MidiDevice? device) {
    return ListTile(
      title: Center(
          child: Text(
            device != null ? device.name: "Nothing",
            style: OxiTheme.data.textTheme.bodyText2,
          ),
      ),
      onTap: () => {
        oxiOne.OxiOne().device = device,
        Navigator.of(context)
            .pushNamedAndRemoveUntil('/home', (_) => false)
      },
    );
  }
}