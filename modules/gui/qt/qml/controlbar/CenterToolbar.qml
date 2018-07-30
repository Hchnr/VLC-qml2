import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import ActionType_e 1.0

import "qrc:///style/"

ToolBar{
    id: centerToolbar
    height: VLCStyle.icon_normal

    property alias centerToolbarRp: centerToolbarRp
    property bool isPlay: true
    RowLayout{
        id: centerLayout
        height: VLCStyle.icon_normal
        property alias centerToolbarRp: centerToolbarRp
        Repeater {
            id: centerToolbarRp
            model: toolbarInformation.CenterList
            delegate: VLCButton{}
        }
    }
}
