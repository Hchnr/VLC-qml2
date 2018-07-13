import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import ActionType_e 1.0

import "qrc:///style/"
import "qrc:///controlbar/Menu/"

ToolBar{
    height: VLCStyle.icon_normal
    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.LeftList
            delegate: ToolButton {
                Image {
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }
                onClicked: {
                    console.log(model.modelData.widgetName + " clicked.")
                    switch ( model.modelData.widgetName ) {
                        case "Bookmark": toolbarInformation.doAction(ActionType_e.SNAPSHOT_ACTION); break;
                        case "Subtitle": toolbarInformation.doAction(ActionType_e.OPEN_SUB_ACTION); break;
                        case "Random": toolbarInformation.doAction(ActionType_e.RANDOM_ACTION); break;
                        case "Loop": toolbarInformation.doAction(ActionType_e.LOOP_ACTION); break;
                    }
                }
            }
        }
    }
}
