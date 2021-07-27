// Copyright 2018 The Flutter team. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:io';
import 'dart:ui';
import 'dart:developer' as developer;

import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';
import 'package:flutter_midi_command/flutter_midi_command.dart';
import 'package:desktop_window/desktop_window.dart';
import 'package:provider/provider.dart';

import 'package:oxi_companion_flttr/models/midi_device_model.dart';

import 'package:oxi_companion_flttr/screens/home.dart';
import 'package:oxi_companion_flttr/screens/device_selector.dart';
import 'package:oxi_companion_flttr/screens/update_select.dart';
import 'package:oxi_companion_flttr/screens/updater.dart';
import 'package:oxi_companion_flttr/screens/update_succeeded.dart';


import 'package:oxi_companion_flttr/common/oxi_theme.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {

  MidiDevice? _oxi;

  @override
  Widget build(BuildContext context) {
    if (Platform.isLinux || Platform.isWindows || Platform.isMacOS) {
      _resize();
    }

    return ChangeNotifierProvider(
      create: (context) => MidiDeviceModel(),
      child: MaterialApp(
        title: 'Oxi One Companion',
        initialRoute: '/device_selector',
        routes: {
          '/update_select': (context) => UpdateSelect(),
          '/device_selector': (context) => DeviceSelector(),
          '/updater': (context) => Updater(),
          '/update_succeeded': (context) => UpdateSucceed(),
          '/home': (context) => HomeScreen(),
        },
        theme: oxiTheme,
        debugShowCheckedModeBanner: false,

      )
    );
  }
  Future _resize() async {
    await DesktopWindow.setMinWindowSize(Size(600,600));
    await DesktopWindow.setMaxWindowSize(Size(600,600));
  }
}
