import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import "qrc:///style/"

import ActionType_e 1.0

ToolBar{
    height: VLCStyle.icon_normal
    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.RightList
            delegate: ToolButton {
                Image {
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: controlbar.getTipFromName(model.modelData.widgetName)

                onClicked: {
                    console.log(model.modelData.widgetName + " clicked.")
                    switch ( model.modelData.widgetName )
                    {
                    case "TBD": toolbarInformation.doAction(ActionType_e.STOP_ACTION); break;
                    case "Playlist": toolbarInformation.doAction(ActionType_e.PLAYLIST_ACTION); break;
                    case "Fullsreen": toolbarInformation.doAction(ActionType_e.FULLSCREEN_ACTION); break;
                    }
                }

            }
        }
    }
}

