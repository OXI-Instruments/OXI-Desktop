import 'dart:developer' as dev;

import 'package:flutter/material.dart';
import 'package:oxi_companion_flttr/common/oxi_theme.dart';
import 'package:provider/provider.dart';

import 'package:flutter_midi_command/flutter_midi_command.dart';

import 'package:oxi_companion_flttr/models/midi_device_model.dart';


class HomeScreen extends StatefulWidget {

  static const routeName = '/home';

  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  @override
  Widget build(BuildContext context) {

    // final args = ModalRoute.of(context)!.settings.arguments;
    // dev.log("Args: {$args}");

    return Scaffold(
      appBar: OxiAppBar(
          title: Text("Oxi One",
            style: TextStyle(
                fontWeight: FontWeight.w100,
                fontSize: 24
            ),
          )
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: OutlinedButton(
                  onPressed: () {
                    Navigator.of(context).pushNamed('/update_select');
                  },
                  child: const Text(
                      "Update Firmware",
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.w100,
                        color: Color(0xfffff0f8)
                      ),
                  ),
                style: OutlinedButton.styleFrom(
                  fixedSize: Size(260.0, 62.0),
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
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: OutlinedButton(
                  onPressed: null,
                  child: const Text(
                    "Open Manual",
                    style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.w100
                    ),
                  ),
                  style: OutlinedButton.styleFrom(
                      fixedSize: Size(260.0, 62.0)
                  )
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: OutlinedButton(
                  onPressed: null,
                  child: const Text(
                    "Report a Bug",
                    style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.w100
                    ),
                  ),
                  style: OutlinedButton.styleFrom(
                      fixedSize: Size(260.0, 62.0)
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