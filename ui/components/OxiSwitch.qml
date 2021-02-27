import QtQuick 2.15
import QtQuick.Controls 2.5

Switch {
    id: swt

    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26
        x: swt.width - 50
        y: parent.height / 2 - height / 2
        color: swt.checked ? "#a84eaf" : "#7baefb"
        border.color: swt.checked ? "#a84eaf" : "#7baefb"
        Text {
          text: swt.checked? "":"off"
          x: parent.width - width
          rightPadding: 2
          color: "black"
          anchors.verticalCenter: parent.verticalCenter
        }
        Text {
          text: swt.checked? "on":""
          x: 0
          leftPadding: 2
          color: "white"
          anchors.verticalCenter: parent.verticalCenter
        }

        Rectangle {
            x: swt.checked ? parent.width - width : 0
            width: 26
            height: 26
            color: swt.down ? "#cccccc" : "#ffffff"
//            border.color: swt.checked ? (swt.down ? "#17a81a" : "#21be2b") : "#999999"
        }
    }
}
