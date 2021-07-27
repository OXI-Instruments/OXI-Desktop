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
                fontSize: 22.0
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(10.0),
              child: OutlinedButton(
                  onPressed: () => Navigator.of(context).pushNamedAndRemoveUntil('/home', (_)=>(false)),
                  child: const Text(
                    "Awesome!",
                    style: TextStyle(
                        fontSize: 18,
                        fontWeight: FontWeight.w100,
                        color: Color(0xfffff0f8)
                    ),
                  ),
                  style: OutlinedButton.styleFrom(
                      fixedSize: Size(180.0, 40.0),
                      shape: ContinuousRectangleBorder (
                        side: BorderSide(
                          color: Color(0xffffffff),
                          width: 5.0,
                          style: BorderStyle.solid,
                        ),
                      )
                  )
              ),
            ),
          ],
        ),
      )
    );
  }

  void _clickDummy() {
    dev.log("klick klock");
  }
}