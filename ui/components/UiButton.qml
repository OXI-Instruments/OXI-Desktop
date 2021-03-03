import QtQuick 2.0
import QtQuick.Controls 2.12

import "../style"

Button {
  property alias txt: txt.text
  property double txtOpacity: 1.0
  property double bgBri: 0
//  height: 60
  hoverEnabled: true
  contentItem: Text {
    id: txt
    text: ""
    opacity: txtOpacity
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    font.pointSize: 16
    color: OxiTheme.colors.font_color
    font.styleName: "Light"//OxiTheme.fonts.light
  }
  background: Rectangle {
    color: Qt.rgba(bgBri, bgBri, bgBri, 1)
    border.color: OxiTheme.colors.border_color
    border.width: 1
  }
}
