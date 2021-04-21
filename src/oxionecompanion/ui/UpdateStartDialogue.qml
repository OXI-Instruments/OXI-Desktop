import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15
//import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1

import "components" as Oxi
import "style"

Page {
  width: window.width
  height: 520
  id: updateStartDialogue
  background: null

  title: qsTr("Update firmware")

  FileDialog {
    id: updateDialog
    title: "Chose update file"
    folder: StandardPaths.writableLocation(StandardPaths.DownloadsLocation)
    nameFilters: ["Sysex file (*.syx)", "All files (*)"]
    onAccepted: {
      hw.start_update(settings.url_to_path(updateDialog.file))
      stackView.push(Qt.resolvedUrl("UpdateProgress.qml"))
    }
  }

  ColumnLayout {
    id: formColumn
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: 340
    spacing: 10

    Rectangle {
        Layout.fillWidth: true
        Layout.maximumWidth: 340
        Text {
          id: updateTxt
          text: update_avail ?
            "This will install firmware version 1.4.2 from our servers. An automatic backup of your device will be made before the installation starts.":
            "It seems like there's no online update available right now. Go make some music! :)"
          wrapMode: Text.WordWrap
          anchors.centerIn: parent
          color: OxiTheme.colors.font_color
          font.pointSize: 16
          font.styleName: "Light"
        }
    }

    Oxi.UiButton {
      id: backupBtn
      visible: update_avail
      Layout.alignment: Qt.AlignHCenter
      hoverEnabled: true
      txt: "Install update"
      width: 260
      height: 60
      onClicked: {
        stackView.push("LoadingScreen.qml", {
                         "progressText": "Backing up your data",
                         "title": "Data backup"
                       })
      }
    }
    Text {
      text: "Install from file instead"
      color: OxiTheme.colors.font_color
      opacity: 0.8
      font.pointSize: 12
      font.styleName: "Light"
//      Layout.fillWidth: true
      Layout.alignment: Qt.AlignHCenter
      Layout.topMargin: update_avail? 5 : 25
      font.underline: false
      MouseArea {
        anchors.fill: parent
        onClicked: updateDialog.visible = true
        hoverEnabled: true
        onHoveredChanged: {
          if (containsMouse) {
            parent.font.underline = true
          } else {
            parent.font.underline = false
          }
        }
      }
    }
  }
}
