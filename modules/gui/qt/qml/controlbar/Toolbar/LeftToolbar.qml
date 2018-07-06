import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import ActionType_e 1.0

import "qrc:///controlbar/Menu/"

ToolBar{
    height: parent.height
    RowLayout{
        height: parent.height
        Repeater {
            model: toolbarInformation.getLeftList()
            delegate: ToolButton {
                Image {
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }

                SubtitleMenu {
                    id: subtitleMenu
                    y: 110
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
