import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15

Page {
  width: 800
  height: 520
  id: globalConfigView
  background: null

  title: "Global config"

  ColumnLayout {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    Column {

        Switch {
            text: "Ignore transport controls"
            id: transportIgnoreTgl
            width: 200

            indicator: Rectangle {
                implicitWidth: 48
                implicitHeight: 26
                x: transportIgnoreTgl.width - 50
                y: parent.height / 2 - height / 2
                color: transportIgnoreTgl.checked ? "#a84eaf" : "#7baefb"
                border.color: transportIgnoreTgl.checked ? "#a84eaf" : "#7baefb"
                Text {
                  text: transportIgnoreTgl.checked? "":"off"
                  x: parent.width - width
                  rightPadding: 2
                  color: "black"
                  anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    x: transportIgnoreTgl.checked ? parent.width - width : 0
                    width: 26
                    height: 26
                    color: transportIgnoreTgl.down ? "#cccccc" : "#ffffff"
                    border.color: transportIgnoreTgl.checked ? (transportIgnoreTgl.down ? "#17a81a" : "#21be2b") : "#999999"
                }
            }
            contentItem: Text {
                text: transportIgnoreTgl.text
                font.styleName: "Light"
                font.pointSize: 16
                color: "white"
                rightPadding: 80 // + transportIgnoreTgl.spacing
                verticalAlignment: Text.AlignLeft
            }
        }

        Switch {
            text: "Send MIDI clock"
            id: sendMidiClcTgl
            width: 200

            indicator: Rectangle {
                implicitWidth: 48
                implicitHeight: 26
                x: sendMidiClcTgl.width - 50
                y: parent.height / 2 - height / 2
                color: sendMidiClcTgl.checked ? "#a84eaf" : "#7baefb"
                border.color: sendMidiClcTgl.checked ? "#a84eaf" : "#7baefb"
                Text {
                  text: sendMidiClcTgl.checked? "":"off"
                  x: parent.width - width
                  rightPadding: 2
                  color: "black"
                  anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    x: sendMidiClcTgl.checked ? parent.width - width : 0
                    width: 26
                    height: 26
                    color: sendMidiClcTgl.down ? "#cccccc" : "#ffffff"
                    border.color: sendMidiClcTgl.checked ? (sendMidiClcTgl.down ? "#17a81a" : "#21be2b") : "#999999"
                }
            }
            contentItem: Text {
                text: sendMidiClcTgl.text
                font.styleName: "Light"
                font.pointSize: 16
                color: "white"
                rightPadding: 80 // + transportIgnoreTgl.spacing
                verticalAlignment: Text.AlignLeft
            }
        }

      Switch {
          text: "Send CC when track muted"
          id: sendMidiCcMuteTgl
          background: Rectangle {
            color: "green"
          }

          indicator: Rectangle {
              implicitWidth: 48
              implicitHeight: 26
              x: transportIgnoreTgl.width - 50
              y: parent.height / 2 - height / 2
              color: sendMidiCcMuteTgl.checked ? "#a84eaf" : "#7baefb"
              border.color: sendMidiCcMuteTgl.checked ? "#a84eaf" : "#7baefb"
              Text {
                text: sendMidiCcMuteTgl.checked? "":"off"
                x: parent.width - width
                rightPadding: 2
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
              }

              Rectangle {
                  x: sendMidiCcMuteTgl.checked ? parent.width - width : 0
                  width: 26
                  height: 26
                  color: sendMidiCcMuteTgl.down ? "#cccccc" : "#ffffff"
                  border.color: sendMidiCcMuteTgl.checked ? (transportIgnoreTgl.down ? "#17a81a" : "#21be2b") : "#999999"
              }
          }
          contentItem: Text {
              text: sendMidiCcMuteTgl.text
              font.styleName: "Light"
              font.pointSize: 16
              color: "white"
              rightPadding: 80 // + transportIgnoreTgl.spacing
              verticalAlignment: Text.AlignLeft
          }
      }
      Row {
        padding: 10
        width: 200
        Text {
          id: defaultBpmLbl
          text: qsTr("Default BPM")
          font.styleName: "Light"
          font.pointSize: 16
          color: "white"
        }
        TextField {
          id: defaultBpmIn
          placeholderText: "127"
          placeholderTextColor: "white"
          font.styleName: "Light"
          focus: false
          background: Rectangle {
              color: "#000"
              border.color: "#7a7a95"
              border.width: 1
          }
          width: 48
//          height: 30
          leftPadding: 0
          color: "white"
          maximumLength: 3
          validator: RegularExpressionValidator {
              regularExpression: /[0-9]{1,3}/
          }
        }
      }

//      Rectangle {
//        id: defaultBpmConfig
//        Text {
//          id: defaultBpmLbl2
//          text: qsTr("Default BPM")
//          font.styleName: "Light"
//          font.pointSize: 16
//          color: "white"
//        }
//        TextInput {
//          maximumLength: 3
//          validator: IntValidator{bottom: 42; top: 999;}
//          text: "127"
//        }
//      }

    }
  }
}
