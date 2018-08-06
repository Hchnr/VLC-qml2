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
    ToolTip.text: model.modelData.tip

    Rectangle {
        anchors.fill: parent
        border.color: parent.hovered? VLCStyle.colorButtonBorderHighlight
                                    : VLCStyle.colorButtonBorder
        border.width: 1
        color: "transparent"
        Image {
            id: image
            source: model.modelData.imgSrc
            anchors.fill: parent
        }
    }

    onClicked: {
        toolbarInformation.doAction(model.modelData.buttonAction);
        console.log(name + " clicked. " + model.modelData.buttonAction);
        console.log(model.modelData.imgSrc+" : "+model.modelData.checked);
    }
}

