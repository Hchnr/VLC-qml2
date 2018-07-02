import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import "qrc:///controlbar/Menu/"

ToolBar{
    height: parent.height
    RowLayout{
        height: parent.height
        Repeater {
            model: leftbarList
            delegate: ToolButton {
                Image {
                    source: controlbar.getIconFromName(widgetName)
                }

                SubtitleMenu {
                    id: subtitleMenu
                    y: 110
                }
                onClicked: {
                    console.log(widgetName + " clicked.")
                    switch ( widgetName ) {
                        case "Bookmark": actionsManager.doAction(controlbar.getActionFromName("SNAPSHOT_ACTION")); break;
                        case "Subtitle": actionsManager.doAction(controlbar.getActionFromName("OPEN_SUB_ACTION")); break;
                        case "Random": actionsManager.doAction(controlbar.getActionFromName("RANDOM_ACTION")); break;
                        case "Loop": actionsManager.doAction(controlbar.getActionFromName("LOOP_ACTION")); break;
                    }
                }
            }
        }
    }
}
