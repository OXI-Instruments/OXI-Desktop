import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

Page {
  width: 800
  height: 520
  id: updateStartDialogue
  background: null

  title: qsTr("Update firmware")

  ColumnLayout {
    id: formColumn
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: 460
    spacing: 10

    Text {
      text: "This will install firmware version 1.4.2 from our servers. An automatic backup of your device will be made before the installation starts."
      wrapMode: Text.WordWrap
      Layout.fillWidth: true
//      width: parent.width
      color: "white"
      font.pointSize: 16
      font.styleName: "Light"
    }

    Button {
      id: backupBtn
      Layout.alignment: Qt.AlignHCenter
      hoverEnabled: true
      contentItem: Text {
        text: "Install update"
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
      text: "Install from file"
      color: "#d7d7d7"
      font.pointSize: 14
      font.styleName: "Thin"
//      Layout.fillWidth: true
      Layout.alignment: Qt.AlignHCenter
      Layout.topMargin: 5
    }
  }
}
