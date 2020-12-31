import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

Page {
  width: 800
  height: 520
  id: backupStartDialogue
  background: null

  property string progressText

  title: ""

  ColumnLayout {
    id: progressColumn
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: 280

    ProgressBar {
      id: backupProgress
      indeterminate: true
      from: 0
      to: 1
      value: 0
      Layout.fillWidth: true
    }
    Timer {
      id: progressTimer
      interval: 200
      repeat: true
      running: false
      onTriggered: {
        if (backupProgress.value < 1) {
          backupProgress.value += 0.01
        } else {
          blockNav = false
          progressTimer.stop()
          confirmBtn.visible = true
          progressText.text = "Backup succeeded"
        }
      }
    }
    Timer {
      id: waitTimer
      interval: 1000
      running: false
      onTriggered: {
        progressText.text = "Backing up your data"
        progressTextWrapper.width = progressText.width
        progressTimer.start()
        backupProgress.indeterminate = false
      }
    }
    Component.onCompleted: {
      waitTimer.start()
    }
    Rectangle {
      id: progressTextWrapper
      Layout.alignment: Qt.AlignHCenter
      height: progressText.height
      width: progressText.width
      color: "#00000000"

      Text {
        id: progressText
        text: qsTr("Starting backup")
        font.styleName: "Light"
        font.pointSize: 16
        color: "white"
      }
      MouseArea {
        anchors.fill: parent
        onClicked: {
          blockNav = false
          progressTimer.stop()
          waitTimer.stop()
          stackView.push("BackupDataView.qml")
        }
      }
    }
    Button {
      id: confirmBtn
      visible: false
      Layout.fillWidth: true
      Layout.preferredHeight: 60
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
      onClicked: stackView.push("BackupDataView.qml")

    }
  }

  Component.onCompleted: {
    blockNav = true
  }
}
