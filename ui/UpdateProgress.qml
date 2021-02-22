import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15
//import QtGraphicalEffects 1.0

Page {
  width: 800
  height: 520
  id: updateProgress
  background: null

  property string progressText

  title: "Firmware update"

  ColumnLayout {
    id: progressColumn
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: 280

    ProgressBar {
      id: progress
      indeterminate: true
      from: 0
      to: 1
      value: 0
      Layout.fillWidth: true
      contentItem: Item {
        implicitWidth: parent.width
        Image {
          id: progressImg
          source: "background.svg"
          width: progress.visualPosition * parent.width
          height: parent.height
        }
      }
    }
    Rectangle {
      id: progressTextWrapper
      Layout.alignment: Qt.AlignHCenter
      height: progressText.height
      Layout.fillWidth: true
      color: "#00000000"

      Text {
        id: progressText
        text: qsTr("Waiting for the Oxi One...")
        font.styleName: "Light"
        font.pointSize: 16
        anchors.horizontalCenter: parent.horizontalCenter
        color: "white"
        onTextChanged: {
          anchors.horizontalCenter = parent.horizontalCenter
        }
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          blockNav = false
          progressTimer.stop()
          waitTimer.stop()
//          stackView.clear()
          stackView.pop()
        }
      }
    }
    Button {
      id: confirmBtn
      enabled: false
      opacity: 0
      Layout.fillWidth: true
      Layout.preferredHeight: 40
      contentItem: Text {
        text: qsTr("OK")
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
      onClicked: {
//        stackView.clear()
        stackView.pop()
      }
    }
  }

  Connections {
    target: hw
    function onIsConnectedSignal() {
      progressText.text = "Update successful!"
      confirmBtn.opacity = 1
      confirmBtn.enabled = true
      blockNav = false
    }
    function onUpdateStartSignal() {
      console.log("Update started!")
      progress.indeterminate = false
      progressText.text =  "Updating..."
    }
    function onProgressSignal(val) {
      progress.value = val
      if (val > 0.9) { //TODO maybe listen to signal instead?
        progressText.text = "Oxi starting up..."
      }
    }
  }

  Component.onCompleted: {
    blockNav = true
  }
}
