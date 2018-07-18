import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3

import "qrc:///style/"

Rectangle {
    id: controlbar
    color: "white"
    // width: parent.width
    height: sliderBar.height + toolBar.height
    property alias sliderBar: sliderBar
    property alias toolBar: toolBar

    Column{
        id: bottomBar
        anchors.bottom: parent.bottom
        width: parent.width

        InputController {
            id: sliderBar
            Layout.fillWidth: true
            Layout.minimumWidth: 800
            height: VLCStyle.heightSeekBar
        }

        RowLayout {
            id: toolBar
            width: parent.width
            height: VLCStyle.heightToolbar
            property alias centerToolbar: centerToolbar
            LeftToolbar{
                id: leftToolbar
                anchors.left: parent.left
            }

            CenterToolbar{
                id: centerToolbar
                anchors.horizontalCenter: parent.horizontalCenter
            }

            RowLayout {
                anchors.right: parent.right
                SliderBar{
                    id: soundSlider
                    visible: (toolBar.width > 676)
                    width:VLCStyle.widthVolumeSlider
                    anchors.right: rightToolbar.left
                    value: toolbarInformation.VolumeModel.volume
                    onMoved: {
                        toolbarInformation.VolumeModel.onVolumeChanged(value)
                    }
                }
                RightToolbar{
                    id: rightToolbar
                    anchors.right: parent.right
                }
            }
        }
    }

    /* get the icon path of widgets */
    function getIconFromName(name) {
        var m = {"Bookmark": "qrc:///toolbar/tv.svg",
              "Subtitle": "qrc:///menu/messages.svg",
              "Random": "qrc:///buttons/playlist/shuffle_on.svg",
              "Loop": "qrc:///buttons/playlist/repeat_all.svg",
              "Slower": "qrc:///toolbar/slower.svg",
              "Previous": "qrc:///toolbar/dvd_prev.svg",
              "Play": "qrc:///toolbar/play_b.svg",
              "Next": "qrc:///toolbar/dvd_next.svg",
              "Faster": "qrc:///toolbar/faster.svg",
              "Fullscreen": "qrc:///toolbar/fullscreen.svg",
              "Playlist": "qrc:///toolbar/playlist.svg",
              "TBD": "qrc:///toolbar/extended.svg",
              "Pause": "qrc:///toolbar/pause_b.svg"
        }
        return m[name]
    }

    /* get tool tip */
    function getTipFromName(name) {
        var m = {"Bookmark": "Bookmark",
              "Subtitle": "Subtitle",
              "Random": "Random",
              "Loop": "Click to toggle between loop all, loop one and no loop",
              "Slower": "Slower",
              "Previous": "Previous / Backward",
              "Play": "Play / Pause",
              "Next": "Next / Forward",
              "Faster": "Faster",
              "Fullscreen": "Fullscreen",
              "Playlist": "Playlist",
              "TBD": "Extended",
              "Pause": "Play / Pause"
        }
        return m[name]
    }

    /*
    Component.onCompleted: {
        console.log(toolbarInformation.getVolumeModel());
        console.log(toolbarInformation.getVolumeModel().volume );
    }
    */
}
