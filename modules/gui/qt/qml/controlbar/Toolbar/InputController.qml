import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: inputControl
    width: parent.width
    Text{
        id: timeElapsed
        width: 40
        anchors.left: parent.left
        text: elapsedTimeLabel.strElapsed
    }
    SliderBar{
        id: sliderBar
        height: 10
        width: parent.width - timeElapsed.width - timeRemaining.width
        anchors.horizontalCenter: parent.horizontalCenter
        value: 0
        init : 0
    }
    Text{
        id: timeRemaining
        width: 40
        anchors.right: parent.right
        text: remainingTimeLabel.strRemaining
    }
    Component.onCompleted: {
        console.log(elapsedTimeLabel)
        console.log(remainingTimeLabel)
        console.log(elapsedTimeLabel.strElapsed)
        console.log(remainingTimeLabel.strRemaining)
    }
}
