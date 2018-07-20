import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Styles 1.4

import ActionType_e 1.0
import LoopStatus_e 1.0

import "qrc:///style/"

ToolBar{
    id: toolbar
    height: VLCStyle.icon_normal
    visible: (parent.width > leftToolbar.width + centerToolbar.width
              + soundSlider.width * 2 + rightToolbar.width)

    property bool randomStatus: toolbarInformation.randomStatus

    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.LeftList

            delegate: ToolButton {
                id: leftButton
                Image {
                    id: img
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: controlbar.getTipFromName(model.modelData.widgetName)

                Component.onCompleted: {
                    if(model.modelData.widgetName === "Random") {
                        leftButton.checkable = true;
                        leftButton.checked = Qt.binding(function() {
                            return toolbar.randomStatus; } );
                    }
                    else if( model.modelData.widgetName === "Loop" ) {
                        leftButton.checkable = true;
                        leftButton.checked = Qt.binding(function() {
                            return (toolbarInformation.loopStatus !== LoopStatus_e.NORMAL);
                        });
                        img.source = Qt.binding(function() {
                            if(toolbarInformation.loopStatus === LoopStatus_e.REPEAT_ONE)
                                return "qrc:///buttons/playlist/repeat_one.svg";
                            else
                                return "qrc:///buttons/playlist/repeat_all.svg";
                        });
                    }
                }

                onClicked: {
                    console.log(model.modelData.widgetName + " clicked.")
                    console.log(model.modelData.buttonAction)
                    toolbarInformation.doAction(model.modelData.buttonAction)
                }
            }
        }
    }
}
