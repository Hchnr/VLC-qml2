import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import CommonParameter 1.0

ToolBar{
    id: centerToolbar
    height: parent.height
    property alias centerToolbarRp: centerToolbarRp
    property bool isPlay: true
    RowLayout{
        id: centerLayout
        height: parent.height
        property alias centerToolbarRp: centerToolbarRp
        Repeater {
            id: centerToolbarRp
            model: centerbarList
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                property string imgsrc: ""
                Image {
                    id: image
                    source: imgsrc
                }
                Component.onCompleted: {
                    imgsrc = controlbar.getIconFromName(widgetName)
                }

                onClicked: {
                    switch ( widgetName ) {
                        case "Slower": actionsManager.doAction(CommonParameter.SLOWER_ACTION); break;
                        case "Previous": actionsManager.doAction(CommonParameter.PREVIOUS_ACTION); break;
                        case "Play": actionsManager.doAction(CommonParameter.PLAY_ACTION); break;
                        case "Next": actionsManager.doAction(CommonParameter.NEXT_ACTION); break;
                        case "Faster": actionsManager.doAction(CommonParameter.FASTER_ACTION); break;
                    }
                }
                function togglePlay()
                {
                    if ( centerToolbar.isPlay ) {
                        imgsrc = controlbar.getIconFromName("Pause");
                    }
                    else {
                        imgsrc = controlbar.getIconFromName("Play");
                    }
                    centerToolbar.isPlay = ! centerToolbar.isPlay;
                }
            }
        }

    }
}
