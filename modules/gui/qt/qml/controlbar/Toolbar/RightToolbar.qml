import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import ActionType_e 1.0

ToolBar{
    height: parent.height
    RowLayout{
        height: parent.height
        Repeater {
            model: toolbarInformation.getRightList()
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                Image {
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }

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

