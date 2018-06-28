import QtQuick 2.0

import "qrc:///Toolbar/"
import "qrc:///Menu/"

Image{
    anchors.fill: parent
    source: "./Toolbar/image/cover.jpg"
    fillMode: Image.PreserveAspectFit
    clip: true
    // anchors.verticalCenter: parent.verticalCenter
    property alias subtitleText: subtitleText

    MouseArea{
        anchors.fill: parent
        hoverEnabled: true

        ContextMenu {
            id: contextMenu
        }
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button == Qt.LeftButton)
                console.log("left")
            else if (mouse.button == Qt.RightButton) {
                console.log("right")
                contextMenu.x = mouse.x
                contextMenu.y = mouse.y
                contextMenu.open()
            }
        }

        onPositionChanged: {
            if(bottomBar.toolBar.visible) {
                mouseTimer.restart()
            }
            else {
                bottomBar.toolBar.visible = true
                console.log(videoWidget.height)
                videoWidget.height = fakePlayer.height - bottomBar.toolBar.height - bottomBar.sliderBar.height
                bottomBar.height = bottomBar.toolBar.height + bottomBar.sliderBar.height
                mouseTimer.restart()
                console.log("moved.")
            }
        }
    }
    // no movement then hide toolbar
    Timer{
        id: mouseTimer
        interval: 3000;
        repeat: true;
        running: true;
        onTriggered: {
            bottomBar.toolBar.visible = false
            console.log(videoWidget.height)
            videoWidget.height = fakePlayer.height - bottomBar.sliderBar.height
             bottomBar.height = bottomBar.sliderBar.height
            mouseTimer.stop()
        }
    }
    Subtitle {
        id: subtitleText
        height: 80
        width: parent.width-40
    }
}


