import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Page {
  id: home
  width: 800
  height: 520
  title: oxiConnected ? "Oxi One" : "Please connect your Oxi One"
  background: null

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
    rowSpacing: 8

    Button {
      id: backupBtn
      Layout.preferredHeight: 60
      Layout.fillWidth: true
      hoverEnabled: true
      contentItem: Text {
        text: oxiConnected === false
              && backupBtn.hovered ? qsTr("Please connect your Oxi") : qsTr(
                                       "Backup and restore")
        opacity: oxiConnected ? 1.0 : 0.5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      background: Rectangle {
        color: "#000"
        border.color: "#7a7a95"
        border.width: 1
      }
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse
                     && oxiConnected === false ? Qt.ForbiddenCursor : Qt.ArrowCursor
        onClicked: function () {
          if (oxiConnected === true) {
            stackView.push(Qt.resolvedUrl("BackupStartDialogue.qml"))
          }
        }
      }
    }

    Button {
      id: fwBtn
      hoverEnabled: true
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      contentItem: Text {
        text: oxiConnected === false
              && fwBtn.hovered ? qsTr("Please connect your Oxi") : qsTr(
                                   "Update firmware")
        opacity: oxiConnected === true ? 1.0 : 0.5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      background: Rectangle {
        color: "#000"
        border.color: "#7a7a95"
        border.width: 1
      }
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse
                     && oxiConnected === false ? Qt.ForbiddenCursor : Qt.ArrowCursor
        onClicked: {
          if (oxiConnected === true) {
            stackView.push("UpdateStartDialogue.qml")
          }
        }
      }
    }

    Button {
      id: globalBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      hoverEnabled: true
      contentItem: Text {
        text: globalBtn.hovered? qsTr("Comming soon") : qsTr("Global options")
        opacity: 0.5
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      background: Rectangle {
        color: "#000"
        border.color: "#7a7a95"
        border.width: 1
      }
    }

    Button {
      id: manBtn
      hoverEnabled: true
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      contentItem: Text {
        text: qsTr("Open manual")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      background: Rectangle {
        color: "#000"
        border.color: "#7a7a95"
        border.width: 1
      }
      onClicked: Qt.openUrlExternally("https://oxiinstruments.com/product/")
      MouseArea {
        hoverEnabled: true
        anchors.fill: parent
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
      }
    }

    Button {
      id: bugBtn
      Layout.fillWidth: true
      Layout.preferredHeight: 60
      contentItem: Text {
        text: qsTr("Report a bug")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      background: Rectangle {
        color: "#000"
        border.color: "#7a7a95"
        border.width: 1
      }
      onClicked: oxiConnected = !oxiConnected
    }
  }
}
