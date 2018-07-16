import QtQuick 2.0
import QtQuick.Controls 2.2

import "qrc:///style/"
Slider {
    id: control
    value: 0.0

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: VLCStyle.widthSliderImplicit
        implicitHeight: VLCStyle.heightSliderImplicit
        width: control.availableWidth
        height: implicitHeight
        radius: VLCStyle.margin_xxsmall
        color: VLCStyle.bgColorSlider

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: VLCStyle.colorSlider
            radius: VLCStyle.margin_xxsmall
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: VLCStyle.widthHandle
        implicitHeight: VLCStyle.heightHandle
        radius: VLCStyle.margin_xxsmall
        color: control.pressed ? VLCStyle.colorSliderHandlePressed : VLCStyle.colorSliderHandle
        border.color: VLCStyle.colorSliderHandleBorder
    }
}
