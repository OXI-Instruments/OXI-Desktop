import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15

Page {
    width: 800
    height: 520
    id: backupStartDialogue
    background: null

    title: qsTr("Backup and restore")

    Column {
        id: formColumn
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 3

        TextField {
            id: backupUserName
            placeholderText: "My initial backup"
            placeholderTextColor: "white"
            font.styleName: "Thin"
            focus: true
            background: Rectangle {
                color: "#000"
//                border.color: "#7a7a95"
//                border.width: 1
            }
            width: 260
            height: 30
            leftPadding: 0
            color: "white"
            maximumLength: 32
            validator: RegularExpressionValidator {
                regularExpression: /[0-9A-Za-z\.\-_\ ]+/
            }

        }

        Shape {
            width: 260
            height: 2
            ShapePath {
                strokeWidth: 1
                strokeColor: "#7a7a95"
                startX: 0
                startY: 0
                PathLine {
                    x: 260
                    y: 0
                }
            }
        }

        Text {
            text: "\u2015 created on " + new Date().toLocaleDateString(Qt.locale(), "d. MMM yyyy") + " Oxi Version " + oxiVersion
            color: "white"
            font.pointSize: 10
            font.styleName: "Light"
            leftPadding: 3
            bottomPadding: 10

        }

        Button {
            id: backupBtn
            hoverEnabled: true
            contentItem: Text {
                text:  backupNumber > 0? qsTr("Create backup") : qsTr("Create your first backup")
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
            onClicked: function() {
                stackView.push("LoadingScreen.qml", {"progressText": "Backing up your data", "title":"Data backup"})
            }
        }
    }

}
