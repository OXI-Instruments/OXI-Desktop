import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

//import com.oxiinstruments.hardware 1.0
ApplicationWindow {
  id: window
  minimumWidth: 800
  maximumHeight: 600
  maximumWidth: 800
  minimumHeight: 600
  visible: true
  title: qsTr("Oxi One Tool")
  background: bgr

  Item {
    id: bgr
    Rectangle {
      id: barBg
      width: 800
      height: 565
    }

    LinearGradient {
      anchors.fill: barBg
      start: Qt.point(0, 0)
      end: Qt.point(680, 600)
      gradient: Gradient {
        GradientStop {
          position: 0.2
          color: "#a84eaf"
        }
        GradientStop {
          position: 0.8
          color: "#7baefb"
        }
      }
    }

    Rectangle {
      id: menuBg
      y: 65
      x: 0
      width: 800
      height: 495
      color: "black"
    }
  }

  header: Item {
    id: windowHeader
    height: 60
    width: 800

    Rectangle {
      //        color: "#883e2424"
      color: "#000"
      anchors.fill: parent

      Rectangle {
        id: toolButton
        visible: !blockNav && stackView.depth > 1
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height
        width: 65
        gradient: Gradient {
          orientation: Gradient.Horizontal
          GradientStop {
            position: 1.00;
            color: "#000";
          }
          GradientStop {
            position: 0.00;
            color: "#69a84eaf";
          }
        }

        Text {
          text: "◀"
          color: "#ebebf1"
          font.pointSize: 18
          font.styleName: "Light"
          anchors.left: -10

          anchors.verticalCenter: parent.verticalCenter
          anchors.horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            if (blockNav === false) {
              stackView.pop()
            }
          }
        }
      }
      Rectangle {
        id: titleContainer
        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        color: "#00000000"

        Text {
          id: toolBarTitle
          anchors.horizontalCenter: parent.horizontalCenter
          lineHeight: stackView.depth == 1 && oxiConnected ? 1.1 : 1

          text: stackView.currentItem.title
          anchors.verticalCenter: parent.verticalCenter
          font.pointSize: 24
          color: "#ffffff"
          font.styleName: "Thin"
        }
        Text {
          id: oxiVersionTitleText
          text: oxiVersion
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.top: toolBarTitle.bottom
          anchors.topMargin: -14

          font.pointSize: 10
          color: "#ffffff"
          font.styleName: "Thin"
          visible: oxiConnected && stackView.depth == 1
        }
      }
    }
  }

  footer: ToolBar {
    id: footerItm
    font.styleName: "Thin"
    height: 35
    background: Rectangle {
      color: "black"
    }
    horizontalPadding: 30

    Label {
      id: footerLbl
      text: "App version 0.2b4"
      anchors.verticalCenter: parent.verticalCenter

      anchors.right: parent.right
      horizontalAlignment: Text.AlignRight
      verticalAlignment: Text.AlignVCenter
    }
  }

  StackView {
    id: stackView
    width: 800
    height: 600
    initialItem: "HomeForm.qml"
    anchors.fill: parent
  }

  property bool oxiConnected: false
  property var oxiVersion: "1.4b2"
  property var backupNumber: 0
  property var blockNav: false

  Connections {
    target: hw
    onConnectionChanged: {
      oxiConnected = connected
    }
  }

  Component.onCompleted: {

    //      oxiConnected = hw.detectDevice()
  }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

