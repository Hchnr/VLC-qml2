import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

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
                    console.log(widgetName + " clicked.")
                    switch ( widgetName ) {
                        case "Slower": actionsManager.doAction(controlbar.getActionFromName("SLOWER_ACTION")); break;
                        case "Previous": actionsManager.doAction(controlbar.getActionFromName("PREVIOUS_ACTION")); break;
                        case "Play": actionsManager.doAction(controlbar.getActionFromName("PLAY_ACTION")); togglePlay(); break;
                        case "Next": actionsManager.doAction(controlbar.getActionFromName("NEXT_ACTION")); break;
                        case "Faster": actionsManager.doAction(controlbar.getActionFromName("FASTER_ACTION")); break;
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
