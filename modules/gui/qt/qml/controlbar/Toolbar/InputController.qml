import QtQuick 2.0
import QtQuick.Layouts 1.3

import "qrc:///style/"

Rectangle {
    id: inputControl
    width: parent.width
    Text{
        id: timeElapsed
        width: VLCStyle.widthTimeLabel
        anchors.left: parent.left
        text: toolbarInformation.LabelElapsed.strElapsed
    }
    SliderBar{
        id: sliderBar
        height: VLCStyle.heightSeekBar
        width: parent.width - timeElapsed.width - timeRemaining.width
        anchors.horizontalCenter: parent.horizontalCenter

        value: toolbarInformation.SeekSlider.sliderPos

        onMoved: {
            toolbarInformation.SeekSlider.isSlidingNow = true
            toolbarInformation.SeekSlider.sliderPos = value
            toolbarInformation.SeekSlider.isSlidingNow = false
        }
    }
    Text{
        id: timeRemaining
        width: VLCStyle.widthTimeLabel
        anchors.right: parent.right
        text: toolbarInformation.LabelRemaining.strRemaining
    }

}
