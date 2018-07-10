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

        handleMousearea.onPressed: {
            toolbarInformation.getSeekSlider().isSlidingNow = true
            console.log(toolbarInformation.getSeekSlider().isSlidingNow)
        }
        handleMousearea.onReleased: {
            handleMousearea.onReleasedSliderBar()
            toolbarInformation.getSeekSlider().onSliderPosChanged(value)
            toolbarInformation.getSeekSlider().isSlidingNow = false
            console.log(toolbarInformation.getSeekSlider().isSlidingNow)
        }
    }
    Text{
        id: timeRemaining
        width: 40
        anchors.right: parent.right
        text: toolbarInformation.getLabelRemaining().strRemaining
    }

}
