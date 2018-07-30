import QtQuick 2.0
import QtQuick.Controls 2.0

import "qrc:///style/"

ToolButton {
    id: button
    property string name: model.modelData.widgetName

    checkable: model.modelData.checkable
    checked: model.modelData.checked
    ToolTip.visible: hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
    ToolTip.text: controlbar.getTipFromName(name)

    Rectangle {
        anchors.fill: parent
        border.color: parent.hovered? VLCStyle.colorButtonBorderHighlight
                                    : VLCStyle.colorButtonBorder
        border.width: 1
        color: "transparent"
        Image {
            id: image
            source: controlbar.getIconFromName(name);
            anchors.fill: parent
        }
    }

    onClicked: onClickedForAll()
    Component.onCompleted: onCompletedForAll()

    function onCompletedForAll() {
        switch(name) {
        case "Play":
                image.source = Qt.binding(function() {
                    return toolbarInformation.playingStatus?
                            controlbar.getIconFromName("Pause")
                          : controlbar.getIconFromName("Play");
                } );;  break;
        }
    }

    function onClickedForAll() {
        switch(name) {
        case "Loop": toggleLoopButton(); break;
        }

        toolbarInformation.doAction(model.modelData.buttonAction)
    }

    function toggleLoopButton() {
        var loop = !leftButton.checked ;
        var repeat = (image.source == "qrc:///buttons/playlist/repeat_one.svg");

        if( repeat )
        {
            button.checked = false;
            image.source = "qrc:///buttons/playlist/repeat_all.svg";
        }
        else if( loop )
        {
            button.checked = true;
            image.source = "qrc:///buttons/playlist/repeat_one.svg";
        }
        else
        {
            button.checked = true;
            image.source = "qrc:///buttons/playlist/repeat_all.svg";
        }
    }

}

