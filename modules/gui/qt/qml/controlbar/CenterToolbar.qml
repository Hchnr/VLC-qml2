import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import ActionType_e 1.0

import "qrc:///style/"

ToolBar{
    id: centerToolbar
    height: VLCStyle.icon_normal

    property alias centerToolbarRp: centerToolbarRp
    property bool isPlay: true
    RowLayout{
        id: centerLayout
        height: VLCStyle.icon_normal
        property alias centerToolbarRp: centerToolbarRp
        Repeater {
            id: centerToolbarRp
            model: toolbarInformation.CenterList
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                property string imgsrc: ""
                property string imgplay: toolbarInformation.playingStatus?
                                                    controlbar.getIconFromName("Pause")
                                                  : controlbar.getIconFromName("Play");
                Rectangle {
                    anchors.fill: parent
                    border.color: parent.hovered? VLCStyle.colorButtonBorderHighlight
                                                : VLCStyle.colorButtonBorder
                    border.width: 1
                    color: "transparent"
                    Image {
                        id: image
                        source: imgsrc
                        anchors.fill: parent
                    }
                }
                checkable: model.modelData.checkable
                checked: model.modelData.checked
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: controlbar.getTipFromName(model.modelData.widgetName)

                Component.onCompleted: {
                    imgsrc = controlbar.getIconFromName(model.modelData.widgetName);
                    if (  model.modelData.widgetName === "Play" )
                        image.source = Qt.binding(function() { return imgplay; } );
                }

                onClicked: {
                    toolbarInformation.doAction(model.modelData.buttonAction)
                }
            }
        }

    }
}
