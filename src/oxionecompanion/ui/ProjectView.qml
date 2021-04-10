import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

Page {
  width: 1200
  height: 520
  id: backupItemView
  background: null
  title: "Project manager"

  ListModel {
    id: projectModel
    ListElement {
      name: "Demo Project"
      prj_id: 1
    }
    ListElement {
      name: "First jam"
      prj_id: 2
    }
    ListElement {
      name: "Modular stuff"
      prj_id: 3
    }
    ListElement {
      name: "Live act 3"
      prj_id: 5
    }
    ListElement {
      name: "Live act 4"
      prj_id: 5
    }
  }

  RowLayout {
    anchors.fill: parent
    spacing: 8
    anchors.margins: 12
    ColumnLayout {
      ListView {
        id: projList
        model: projectModel
        height: 400
        width: 100
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
        currentIndex: 1
        highlightFollowsCurrentItem: true
        header: Text {
          id: projDesc
          text: qsTr("Projects")
          color: "white"
          font.pointSize: 14
          font.styleName: "Light"
        }
        delegate: Rectangle {
          height: 35
          Text {
            color: projList.ListView.isCurrentItem ? "white" : "green"
            text: model.name
            font.pointSize: 12
            font.styleName: "Light"
          }
          border.color: "white"
          border.width: 1
          MouseArea {
              anchors.fill: parent
              onClicked: {
                projList.currentIndex = model.index
                console.log("current item should be " + model.index)
              }
          }
        }
      }
    }
    ColumnLayout {
      Rectangle {
        color: "#555555"
        height: 400
        Layout.fillWidth: true
      }
    }
  }
}
