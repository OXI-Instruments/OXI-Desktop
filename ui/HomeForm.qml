import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
//import QtGraphicalEffects 1.0
//import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1

import "components" as Oxi
import "style"

Page {
  id: home
  width: window.width
  height: window.height-80
  title: oxiConnected ? "Oxi One" : "Please connect your Oxi One"
  background: null //Oxi.OxiTheme.colors.accent_purple

  GridLayout {
    id: btns
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
//    anchors.fill: parent
    width: 240
//    height: 400
    columns: 1
    rows: 5
    z: 1
    rowSpacing: 12

    Oxi.UiButton {
      id: backupBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: oxiConnected === false && backupBtn.hovered ?
        qsTr("Please connect your Oxi") :
        qsTr("Backup and restore")
      txtOpacity: oxiConnected ? 1 : 0.5
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse && oxiConnected === false ?
            Qt.ForbiddenCursor :
            Qt.ArrowCursor
        onClicked: function () {
          if (oxiConnected === true) {
            stackView.push(Qt.resolvedUrl("BackupStartDialogue.qml"))
          }
        }
      }
    }

    Oxi.UiButton {
      id: fwBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: oxiConnected === false && fwBtn.hovered ?
        qsTr("Please connect your Oxi") :
        qsTr("Update firmware")
      txtOpacity: oxiConnected === true ? 1.0 : 0.5

      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse && oxiConnected === false ?
            Qt.ForbiddenCursor :
            Qt.ArrowCursor
        onClicked: {
          if (oxiConnected === true) {
            stackView.push("UpdateStartDialogue.qml")
          }
        }
      }
    }

    Oxi.UiButton {
      id: projBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: projBtn.hovered? qsTr("Comming soon") : qsTr("Project manager")
      txtOpacity: 0.5
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse && oxiConnected === false ?
            Qt.ForbiddenCursor :
            Qt.ArrowCursor
        onClicked: function () {
          window.minimumWidth = 1000
          window.maximumHeight = 800
          window.maximumWidth = 1000
          window.minimumHeight = 800
          if (oxiConnected === true) {
//            stackView.push(Qt.resolvedUrl("GlobalConfigView.qml"))
          }
        }
      }
    }

    Oxi.UiButton {
      id: globalBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: globalBtn.hovered? qsTr("Comming soon") : qsTr("Configure device")
      txtOpacity: 0.5
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse && oxiConnected === false ?
            Qt.ForbiddenCursor :
            Qt.ArrowCursor
        onClicked: function () {
          if (oxiConnected === true) {
            stackView.push(Qt.resolvedUrl("GlobalConfigView.qml"))
          }
        }
      }
    }

    Oxi.UiButton {
      id: manBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: qsTr("Open manual")
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
          Qt.openUrlExternally("https://oxiinstruments.com/product/")
        }
        onHoveredChanged: {
          if(containsMouse) {
            parent.contentItem.text = "Will open your browser"
          } else {
            parent.contentItem.text = "Open manual"
          }
        }
      }
    }

    Oxi.UiButton {
      id: bugBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      txt: qsTr("Report a bug")
      onClicked: oxiConnected = !oxiConnected
    }
  }
}
