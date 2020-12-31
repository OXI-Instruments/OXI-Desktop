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
            start: Qt.point(0,0)
            end: Qt.point(680, 600)
            gradient: Gradient {
                GradientStop { position: 0.2; color: "#a84eaf" }
                GradientStop { position: 0.8; color: "#7baefb" }
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
//        color: "#3e2424"
        color: "#000"
        anchors.fill: parent

        ToolButton {
            id: toolButton
            visible: !blockNav
            anchors.verticalCenter: parent.verticalCenter

            contentItem: Text {
                text: stackView.depth > 1 ? "<back" : ""
                color: "#ebebf1"
                font.pointSize: 18
                font.styleName: "Light"
                lineHeight: 0.9
            }
            onClicked: {
                if (blockNav === false) {
                    stackView.pop()
                }
            }
        }
        Column {
          anchors.fill: parent
          anchors.horizontalCenter: parent.horizontalCenter
          anchors.verticalCenter: parent.verticalCenter
          Rectangle {
            id: titleContainer
            width: parent.width
            height: toolBarTitle.height
            color: "#00000000"
            //color: "#2c2c6f"

            Text {
              id: toolBarTitle
              anchors.centerIn: parent

              text: stackView.currentItem.title + " "
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignTop
              font.pointSize: 24
              lineHeight: 0.8
              color: "#ffffff"
              font.styleName: "Thin"
            }
          }
          Rectangle {
            width: parent.width
            height: oxiConnected && stackView.depth == 1? oxiVersionTitleText.height : 0
//            color: "#708b68"
            color: "#00000000"
            Text {
              id: oxiVersionTitleText
              text: oxiVersion
              anchors.centerIn: parent
              horizontalAlignment: Text.AlignHCenter
//                verticalAlignment: Text.AlignVCenter
//              fontSizeMode: Text.Fit
              font.pointSize: 10
              lineHeight: 0.35
              color: "#ffffff"
              font.styleName: "Thin"
              visible: oxiConnected && stackView.depth == 1
            }
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
            text: "0.2b4"
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
