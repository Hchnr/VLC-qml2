import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import ActionType_e 1.0
import LoopStatus_e 1.0

import "qrc:///style/"

ToolBar{
    id: toolbar
    height: VLCStyle.icon_normal
    visible: (parent.width > leftToolbar.width + centerToolbar.width
              + soundSlider.width * 2 + rightToolbar.width)

    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.LeftList

            delegate: VLCButton {
                id: leftButton
            }
        }
    }
}
