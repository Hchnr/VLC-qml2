import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import CommonParameter 1.0

ToolBar{
    height: parent.height
    RowLayout{
        height: parent.height
        Repeater {
            model: rightbarList
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                Image {
                    source: controlbar.getIconFromName(widgetName)
                }

                onClicked: {
                    console.log(widgetName + " clicked.")
                    switch ( widgetName )
                    {
                    case "TBD": actionsManager.doAction(CommonParameter.STOP_ACTION); break;
                    case "Playlist": actionsManager.doAction(CommonParameter.PLAYLIST_ACTION); break;
                    case "Fullsreen": actionsManager.doAction(CommonParameter.FULLSCREEN_ACTION); break;
                    }
                }

            }
        }
    }
}

