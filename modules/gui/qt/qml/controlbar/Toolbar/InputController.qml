import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: inputControl
    width: parent.width
    Text{
        id: timeElapsed
        width: 40
        anchors.left: parent.left
        text: toolbarInformation.LabelElapsed.strElapsed
    }
    SliderBar{
        id: sliderBar
        height: 10
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
        width: 40
        anchors.right: parent.right
        text: toolbarInformation.LabelRemaining.strRemaining
    }

}
