import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15

import "components" as Oxi
import "style"


Page {
  width: 800
  height: 520
  id: globalConfigView
  background: null

  title: "Global config"

  ColumnLayout {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
//    anchors.fill: parent
    width: 400
    spacing: 18
    Layout.fillHeight: false
//    height: 400

    RowLayout {
      Text {
        id: startUpOptLbl
        text: qsTr("Startup options")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
        Layout.fillWidth: true
      }
      Text {
        id: startUpOptArr
        text: qsTr("▶")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
    }

    RowLayout {
      Text {
        id: midiOptLbl
        text: qsTr("MIDI options")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
        Layout.fillWidth: true
      }
      Text {
        id: midiOptArr
        text: qsTr("▶")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
      MouseArea {
        width: parent.width
        height: parent.height
        onClicked: {
          stackView.push("MidiConfigView.qml")
        }
      }
    }

    RowLayout {
      Layout.fillHeight: false


      Text {
        Layout.fillWidth: true
        id: sendMidiCcMuteLbl
        text: qsTr("Send CC when track muted")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
      Oxi.OxiSwitch {
          id: sendMidiCcMuteTgl
      }
    }
    RowLayout {
      Layout.fillHeight: false

      Text {
        id: defaultBpmLbl
        text: qsTr("Default BPM")
        font.styleName: "Light"
        font.pointSize: 16
        color: "white"
        Layout.fillWidth: true
      }
      TextField {
        id: defaultBpmIn
        placeholderText: "127"
        placeholderTextColor: "white"
        font.styleName: "Light"
        focus: false
        background: Rectangle {
            color: "#000"
            border.color: "#7a7a95"
            border.width: 1
        }
        horizontalAlignment: TextInput.AlignRight
        implicitWidth: 54
//          height: 30
        leftPadding: 0
        color: "white"
        maximumLength: 3
        validator: RegularExpressionValidator {
            regularExpression: /[0-9]{1,3}/
        }
      }
    }
    Item { Layout.fillHeight: true }
  }
}
