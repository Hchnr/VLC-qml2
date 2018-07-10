import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: inputControl
    width: parent.width
    Text{
        id: timeElapsed
        width: 40
        anchors.left: parent.left
        text: toolbarInformation.getLabelElapsed().strElapsed
    }
    SliderBar{
        id: sliderBar
        height: 10
        width: parent.width - timeElapsed.width - timeRemaining.width
        anchors.horizontalCenter: parent.horizontalCenter

        value: toolbarInformation.getSeekSlider().sliderPos
        init : 0
        handle.onReleased: {
            handle.onReleasedSliderBar()
            toolbarInformation.getSeekSlider().onSliderPosChanged(value)
        }
    }
    Text{
        id: timeRemaining
        width: 40
        anchors.right: parent.right
        text: toolbarInformation.getLabelRemaining().strRemaining
    }

}
