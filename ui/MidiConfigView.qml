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

  title: "MIDI config"

  ColumnLayout {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
//    anchors.fill: parent
    width: 400
    spacing: 18
    Layout.fillHeight: false

    RowLayout {
      Layout.fillHeight: false

      Text {
        id: transportIgnoreTxt
        text: qsTr("Ignore transport controls")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
        Layout.alignment: Qt.AlignLeft
        Layout.fillWidth: true
      }
      Oxi.OxiSwitch {
        id: transportIgnoreTgl
        checked: true
      }
    }

    RowLayout {
      Layout.fillHeight: false

      Text {
        Layout.fillWidth: true
        id: sendMidiClcTxt
        text: qsTr("Send MIDI clock")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
        Oxi.OxiSwitch {
            id: sendMidiClcTgl
        }
    }

    RowLayout {
      Layout.fillHeight: false

      Text {
        Layout.fillWidth: true
        id: midiThruLbl
        text: qsTr("MIDI thru")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
        Oxi.OxiSwitch {
            id: midiThruTgl
        }
    }

    RowLayout {
      Layout.fillHeight: false

      Text {
        Layout.fillWidth: true
        id: btMidiThruLbl
        text: qsTr("BlueTooth MIDI thru")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
        Oxi.OxiSwitch {
            id: btMidiThruTgl
        }
    }

    RowLayout {
      Layout.fillHeight: false

      Text {
        Layout.fillWidth: true
        id: usbMidiThruLbl
        text: qsTr("USB MIDI thru")
        color: OxiTheme.colors.font_color
        font.pointSize: 16
        font.styleName: "Light"
      }
        Oxi.OxiSwitch {
            id: usbMidiThruTgl
        }
    }
  }
}
