import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import "qrc:///style/"

import ActionType_e 1.0

ToolBar{
    height: VLCStyle.icon_normal
    visible: (parent.width > leftToolbar.width + centerToolbar.width
              + soundSlider.width * 2 + rightToolbar.width)

    RowLayout{
        height: VLCStyle.icon_normal
        Repeater {
            model: toolbarInformation.RightList
            delegate: ToolButton {
                Image {
                    source: controlbar.getIconFromName(model.modelData.widgetName)
                }
                checkable: model.modelData.checkable
                checked: model.modelData.checked
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: controlbar.getTipFromName(model.modelData.widgetName)

                onClicked: {
                    toolbarInformation.doAction(model.modelData.buttonAction)
                }

            }
        }
    }
}

