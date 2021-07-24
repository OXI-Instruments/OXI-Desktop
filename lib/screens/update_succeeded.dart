import 'dart:developer' as dev;
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

import 'package:flutter_midi_command/flutter_midi_command.dart';
import 'package:file_picker/file_picker.dart';

import 'package:oxi_companion_flttr/models/midi_device_model.dart';

import 'package:oxi_companion_flttr/common/oxi_theme.dart';

class UpdateSucceed extends StatefulWidget {
  @override
  _UpdateSucceedState createState() => _UpdateSucceedState();
}

class _UpdateSucceedState extends State<UpdateSucceed> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: OxiAppBar(
        title: Text(
          "Success!",
          style: TextStyle(
              fontWeight: FontWeight.w100,
              fontSize: 24
          ),
        ),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Text(
              "Update was successful!",
              style: TextStyle(
                fontSize: 12.0
              ),
            ),
            TextButton(
                onPressed: () => Navigator.of(context).pushNamed('/'),
                child: Text ("Awesome!")
            )
          ],
        ),
      )
    );
  }

  void _clickDummy() {
    dev.log("klick klock");
  }

  void _selectUpdate() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles();
    if (result != null) {
      File file = File(result.files.single.path as String);
      dev.log(file.path);
      Navigator.of(context).pushNamedAndRemoveUntil('/updater', (_)=>(false));
    }
  }
}