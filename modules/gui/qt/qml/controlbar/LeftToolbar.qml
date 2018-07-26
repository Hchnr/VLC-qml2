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

    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.LeftList

            delegate: ToolButton {
                id: leftButton
                Image {
                    id: img
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                    anchors.fill: parent
                }
                checkable: model.modelData.checkable
                checked: model.modelData.checked
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: controlbar.getTipFromName(model.modelData.widgetName)

                onClicked: {
                    toolbarInformation.doAction(model.modelData.buttonAction)

                    if( model.modelData.widgetName === "Loop" ) {
                        /* Toggle Normal -> Loop -> Repeat -> Normal ... */
                        toggleLoopButton();
                    }
                }

                function toggleLoopButton() {
                    var loop = !leftButton.checked ;
                    var repeat = (img.source == "qrc:///buttons/playlist/repeat_one.svg");

                    if( repeat )
                    {
                        leftButton.checked = false;
                        img.source = "qrc:///buttons/playlist/repeat_all.svg";
                    }
                    else if( loop )
                    {
                        leftButton.checked = true;
                        img.source = "qrc:///buttons/playlist/repeat_one.svg";
                    }
                    else
                    {
                        leftButton.checked = true;
                        img.source = "qrc:///buttons/playlist/repeat_all.svg";
                    }
                }
            }
        }
    }
}
