import 'dart:typed_data';

class Project {
  final int id;
  final String name;

  Project(this.id, this.name);

  Project.fromBinary(Uint8List data):
      id = data.elementAt(0),
      name = data.sublist(1, 16).toString();
}