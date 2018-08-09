import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import ActionType_e 1.0
import LoopStatus_e 1.0

import "qrc:///style/"

Rectangle {
    color: VLCStyle.bgColor
    width: toolBar.width
    height: toolBar.height
    visible: (parent.width > leftToolbar.width + centerToolbar.width
              + soundSlider.width * 2 + rightToolbar.width)
    ToolBar{
        id:toolBar
        anchors.fill: parent
        height: VLCStyle.icon_normal
        RowLayout{
            height: VLCStyle.icon_normal
            Repeater {
                model: toolbarInformation.LeftList
                delegate: VLCButton {}
            }
        }
    }
}
