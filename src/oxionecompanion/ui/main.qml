import QtQuick 2.15
import QtQuick.Controls 2.5
//import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.15
//import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import "components"


ApplicationWindow {
  id: window
  minimumWidth: 600
  maximumHeight: 600
  maximumWidth: 600
  minimumHeight: 600
  visible: true
  title: qsTr("Oxi One Tool")
  background: bgr

  Style {
    id: style
    FontLoader {
      id: light
      source: "../NotoSansDisplay-Light.ttf"
    }
    FontLoader {
      id: thin
      source: "../NotoSansDisplay-Thin.ttf"
    }
  }


  Item {
    id: bgr
    Image {
      id: bagBg
      source: "background.svg"
      width: window.width
      height: window.height-35 //565
    }

    Rectangle {
      id: menuBg
      y: 63
      x: 0
      width: window.width
      height: window.height-101 //495
      color: "black"
    }
  }

  header: Item {
    id: windowHeader
    height: 60
    width: window.width

    Rectangle {
      color: "#000"
      anchors.fill: parent

      Rectangle {
        id: toolButton
        visible: !blockNav && stackView.depth > 1
        anchors.verticalCenter: parent.verticalCenter
        height: parent.height
        width: 45
        gradient: Gradient {
          orientation: Gradient.Horizontal
          GradientStop {
            position: 1.00;
            color: "#000";
          }
          GradientStop {
            position: 0.00;
            color: "#42a84eaf";
          }
        }

        Text {
          text: "◂"
          color: "#ffffff"
          opacity: 0.69
          font.pointSize: 20
          font.styleName: "Light"

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
          font.family: thin.font.family
          font.styleName: thin.font.styleName
//          font.styleName: fonts.thin
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
          visible: false //oxiConnected && stackView.depth == 1
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
      text: "App " + settings.app_version()
      anchors.verticalCenter: parent.verticalCenter

      anchors.right: parent.right
      horizontalAlignment: Text.AlignRight
      verticalAlignment: Text.AlignVCenter
    }
  }

  StackView {
    id: stackView
    width: window.width
    height: window.height
    initialItem: "HomeForm.qml"
    anchors.fill: parent
  }

  property bool oxiConnected: false
  property var oxiVersion: "1.4b2"
  property var backupNumber: 0
  property var blockNav: false
  property bool update_avail: false

  Connections {
    target: hw
    function onIsConnectedSignal(state) {
      oxiConnected = state
    }
    function onVersionSignal(ver) {
      console.log("version bubbled to QML and is: " + ver)
      oxiVersion = ver
    }
  }
  Connections {
    target: settings
  }

  Connections {
    target: updates
    function onUpdateAvailableSignal() {
      update_avail = true
    }
  }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

