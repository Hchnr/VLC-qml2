import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import ActionType_e 1.0

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
            model: toolbarInformation.CenterList
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                property string imgsrc: ""
                Image {
                    id: image
                    source: imgsrc
                }
                Component.onCompleted: {
                    imgsrc = controlbar.getIconFromName(model.modelData.widgetName)
                }

                onClicked: {
                    console.log(model.modelData.widgetName + " clicked.")
                    switch ( model.modelData.widgetName ) {
                        case "Slower": toolbarInformation.doAction(ActionType_e.SLOWER_ACTION); break;
                        case "Previous": toolbarInformation.doAction(ActionType_e.PREVIOUS_ACTION); break;
                        case "Play": toolbarInformation.doAction(ActionType_e.PLAY_ACTION); togglePlay(); break;
                        case "Next": toolbarInformation.doAction(ActionType_e.NEXT_ACTION); break;
                        case "Faster": toolbarInformation.doAction(ActionType_e.FASTER_ACTION); break;
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
