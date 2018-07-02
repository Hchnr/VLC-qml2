import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

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
                    case "TBD": actionsManager.doAction(controlbar.getActionFromName("STOP_ACTION")); break;
                    case "Playlist": actionsManager.doAction(controlbar.getActionFromName("PLAYLIST_ACTION")); break;
                    case "Fullsreen": actionsManager.doAction(controlbar.getActionFromName("FULLSCREEN_ACTION")); break;
                    }
                }

            }
        }
    }
}

