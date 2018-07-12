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
        init : 0

        handleMousearea.onPressed: {
            toolbarInformation.SeekSlider.isSlidingNow = true
        }
        handleMousearea.onReleased: {
            toolbarInformation.SeekSlider.isSlidingNow = false
        }
        Timer{
            interval: 150;
            repeat: true;
            running: true;
            onTriggered: {
                if ( toolbarInformation.SeekSlider.isSlidingNow )
                    toolbarInformation.SeekSlider.sliderPos = sliderBar.handleMousearea.getPosSliderBar()
                }
        }
    }
    Text{
        id: timeRemaining
        width: 40
        anchors.right: parent.right
        text: toolbarInformation.LabelRemaining.strRemaining
    }

}
