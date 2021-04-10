import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

Page {
  width: 800
  height: 520
  id: backupItemView
  background: null

  property var name: "backupname"
  property var oxiVersion: "1.2"
  property var date: "11. sept 2001"
  property var id: 1
  property var delSafeCount: 3

  property Item backupOptions: Item {
    ColumnLayout {
      Button {
        id: editBtn
        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Edit"
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
          backupControls.push(backupEdit)
        }
      }
      Button {
        id: restoreBtn
        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Restore this backup"
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
        onClicked: function () {}
      }
    }
  }

  property Item backupEdit: Item {
    ColumnLayout {
      Button {
        id: renameBtn
//        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Rename"
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
          renameDialog.open()
          backupUserName.focus = true
        }
      }
      Button {
        id: deleteBtn
//        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Delete"
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
          delConfirmDialog.open()
          delSafeCountTimer.start()
        }
      }
      Button {
        id: doneBtn
//        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Done"
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
          backupControls.pop()
        }
      }
    }
  }

  Popup {
    id: renameDialog
    width: 280
    height: 160
//    visible: false
    modal: true
    Overlay.modal: Rectangle {
      color: "#99000000"
    }
//    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    background: Rectangle {
      color: "#000"
      border.color: "#7a7a95"
      border.width: 1
    }
    ColumnLayout {
      anchors.fill: parent
      Text {
        id: renameTxt
        text: qsTr("Enter a new name")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      TextField {
        id: backupUserName
        text: "My initial backup"
        placeholderTextColor: "white"
        font.styleName: "Thin"
        focus: true
        background: Rectangle {
            color: "#000"
            border.color: "#7a7a95"
            border.width: 1
        }
//        width: 260
        Layout.fillWidth: parent
        height: 30
        leftPadding: 0
        color: "white"
        maximumLength: 32
        validator: RegularExpressionValidator {
            regularExpression: /[0-9A-Za-z\.\-_\ ]+/
        }
      }
      Button {
        id: renameDoneBtn
        Layout.fillWidth: parent
        hoverEnabled: true
        contentItem: Text {
          text: "Done"
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
          renameDialog.close()
        }
      }
    }
  }

  Popup {
    id: delConfirmDialog
    width: delConfirmTxt.width
    height: 160
//    visible: false
    modal: true
    Overlay.modal: Rectangle {
      color: "#99000000"
    }
//    parent: Overlay.overlay
    anchors.centerIn: Overlay.overlay
    background: Rectangle {
      color: "#000"
      border.color: "#7a7a95"
      border.width: 1
    }
    ColumnLayout {
      anchors.fill: parent
      Text {
        id: delConfirmTxt
        text: qsTr("This will delete the backup forever!")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
        color: "white"
        font.styleName: "Light"
      }
      Button {
        id: delConfirmBtn
        Layout.fillWidth: parent
        enabled: false
        opacity: 0.5
        hoverEnabled: true
        contentItem: Text {
          id: delConfirmBtnTxt
          text: "Delete it!"
          styleColor: "#bb4345"
          color: "#bb4345"
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          font.pointSize: 16
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
          delConfirmDialog.close()
//          backupsModel.remove(id)
          stackView.pop()
        }
        Timer {
          id: delSafeCountTimer
          interval: 1000
          running: false
          repeat: true
          triggeredOnStart: true
          onTriggered: {
            if (delSafeCount > 0) {
              delConfirmBtnTxt.text = "Think (" + delSafeCount + ")"
              delSafeCount--
            } else {
              delSafeCountTimer.stop()
              delConfirmBtnTxt.text = "Delete it!"
              delConfirmBtn.opacity = 1
              delConfirmBtn.enabled = true
            }
          }
        }
      }
    }
  }

  title: "View backup"

  ColumnLayout {
    anchors.horizontalCenter: parent.horizontalCenter

    RowLayout {
      Layout.topMargin: 40
      spacing: 20
      ColumnLayout {
        Layout.alignment: Qt.AlignRight
        Text {
          text: "Name"
          color: "white"
          font.pointSize: 16
          font.styleName: "Light"
          horizontalAlignment: Text.AlignRight
        }
        Text {
          text: "Created"
          color: "white"
          font.pointSize: 16
          font.styleName: "Light"
        }
      }

      ColumnLayout {
        Layout.alignment: Qt.AlignLeft
        Text {
          text: name
          color: "white"
          font.pointSize: 16
          font.styleName: "Light"
        }
        Text {
          text: date
          color: "white"
          font.pointSize: 16
          font.styleName: "Light"
        }
      }
    }
    RowLayout {
      Layout.fillWidth: parent
      StackView {
        id: backupControls
        initialItem: backupOptions
      }
    }
  }
}
