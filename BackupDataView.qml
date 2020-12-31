import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15

Page {
    width: 800
    height: 520
    id: backupDataView
    background: null

    title: qsTr("Backup and restore")

    ListModel {
        id: backupsModel
        ListElement {
            name: "Initial backup"
            oxiVersion: "1.0"
            date: "24. Dec 2020"
        }
        ListElement {
            name: "Automatic backup before 1.2b4"
            oxiVersion: "1.1"
            date: "26. Dec 2020"
        }
        ListElement {
            name: "Automatic backup before 1.2"
            oxiVersion: "1.2b4"
            date: "27. Dec 2020"
        }
        ListElement {
            name: "Backup before the show"
            oxiVersion: "1.2"
            date: "30. Dec 2020"
        }
    }

    Component {
        id: backupDelegate
        Column {
            width: 320
            Text {
                text: name
                font.pointSize: 16
                color: "white"
                font.styleName: "Light"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        let itm = backupsModel.get(index)
                        stackView.push("BackupItemView.qml", {"name": itm.name, "oxiVersion": itm.oxiVersion, "date": itm.date, "id":index})
                    }
                }
            }
            Row {
                spacing: 5
                Text {
                    text: date
                    font.pointSize: 10
                    color: "white"
                    font.styleName: "Light"
                }
                Text {
                    text: "Version " + oxiVersion
                    font.pointSize: 10
                    color: "white"
                    font.styleName: "Light"
                }
            }
        }
    }

    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter
//        topPadding: 40
        anchors.verticalCenter: parent.verticalCenter
//        Layout.topMargin: 80
        ListView {
            width: 320
            height: 370
            clip: true
            model: backupsModel
            delegate: backupDelegate
        }

        Button {
            id: backupBtn
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            hoverEnabled: true
            contentItem: Text {
                text:  qsTr("Create new backup")
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
            onClicked: function() {
                stackView.push("BackupStartDialogue.qml")
            }
        }
    }
}
