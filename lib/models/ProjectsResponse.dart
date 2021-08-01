import 'dart:typed_data';

import 'package:oxi_companion_flttr/models/project.dart';

class ProjectsResponse {
  final List<Project> projects;

  ProjectsResponse(this.projects);

  ProjectsResponse.fromBinary(Uint8List data):
      projects = _extractProjects(data.sublist(0, 16));

  static List<Project> _extractProjects(Uint8List data){
    return [Project.fromBinary(data)];
  }
}