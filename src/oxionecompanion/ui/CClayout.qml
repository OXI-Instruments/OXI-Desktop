import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQuck.Layouts 1.15

Page {
  width: 800
  height: 520
  id: ccLayoutView
  background: null

  title: qsTr("Edit CC controls")

  ListModel {
    id: ccControlsModel
    ListElement {
      name: "Modulation Weel"
      ccNumber: 1
      rangeStart: 0
      rangeStop: 127
      steps: 128
    }
    ListElement {
      name: "Cutoff"
      ccNumber: 1
      rangeStart: 0
      rangeStop: 127
      steps: 2
    }
  }

  Component {
    id: ccControlsDelegate
    Column {
    }
  }
}
