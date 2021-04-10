import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Shapes 1.15
import QtQuick.Layouts 1.15

Page {
  width: 1200
  height: 520
  id: backupItemView
  background: null
  ColumnLayout {
    anchors.fill: parent
    RowLayout {

      Rectangle {
        width: 10
        height: 400
        color: "#555555"
      }
      ColumnLayout {
        width: 310
        Rectangle {
      //      border.color: "white"
      //      border.width: 1
//          Layout.fillWidth: true
          width: cvGrid.width - cvGridDesc.width
          color: "black"
      //      width: 300
          height: 20
          Text {
            id: outputDescLbl
            text: qsTr("CV")
            color: "white"
            Layout.fillWidth: true
            height: 18
            anchors.centerIn: parent
          }
        }
        Row {
          id: cvGrid
          spacing: 8
          Layout.alignment: Qt.AlignHCenter
          Repeater {
            model: 8
            Column {
              spacing: 10
              Text {
                id: outputNum
                text: qsTr("" + (parseInt(index) + 1))
                color: "white"
              }
              Repeater {
                model: 8
                Button {
                  width: 28
                  height: 28
                  background: Rectangle {
                    color: "white"
                  }
                }
              }
            }
          }
          Column {
            id: cvGridDesc
            topPadding: 26
            spacing: 10
            Repeater {
              model: [
                "Sequencer 1",
                "Sequencer 2",
                "Sequencer 3",
                "Sequencer 4",
                "Pitch",
                "Velocity",
                "Enveleope",
                "LFO"
              ]
              Text {
                id: cvFuncDesc
                text: qsTr(modelData)
                color: "white"
                height: 28
              }
            }
          }
        }
      }
    }
  }
}
