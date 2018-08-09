import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import ActionType_e 1.0

import "qrc:///style/"

Rectangle {
    color: VLCStyle.bgColor
    width: toolBar.width
    height: toolBar.height
    ToolBar{
        id: toolBar
        anchors.fill: parent
        height: VLCStyle.icon_normal
        RowLayout{
            height: VLCStyle.icon_normal
            Repeater {
                model: toolbarInformation.CenterList
                delegate: VLCButton{}
            }
        }
    }
}


