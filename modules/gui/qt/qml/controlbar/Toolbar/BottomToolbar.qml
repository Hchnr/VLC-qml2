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
    // signal testStop()

    function getActionFromName(name) {
        var t = {
            "PLAY_ACTION":0, "STOP_ACTION":1,
            "OPEN_ACTION":2,"PREVIOUS_ACTION":3,
            "NEXT_ACTION":4,"SLOWER_ACTION":5,
            "FASTER_ACTION":6,"FULLSCREEN_ACTION":7,
            "FULLWIDTH_ACTION":8,"EXTENDED_ACTION":9,
            "PLAYLIST_ACTION":10,"SNAPSHOT_ACTION":11,
            "RECORD_ACTION":12,"FRAME_ACTION":13,
            "ATOB_ACTION":14,"REVERSE_ACTION":15,
            "SKIP_BACK_ACTION":16,"SKIP_FW_ACTION":17,
            "QUIT_ACTION":18,"RANDOM_ACTION":19,
            "LOOP_ACTION":20,"INFO_ACTION":21,
            "OPEN_SUB_ACTION":22,
        }
        return t[name]
    }


    // get the icon path of widgets
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
    /*
    Component.onCompleted: {
        console.log(toolbarInformation.getVolumeModel());
        console.log(toolbarInformation.getVolumeModel().volume );
    }
    */
    Timer{
        interval: 3000;
        repeat: true;
        running: true;
        onTriggered: {
            console.log(leftToolbar.width+","+centerToolbar.width+","+soundSlider.width+","+rightToolbar.width);
            console.log(toolBar.width);
            console.log(toolBar.minimumWidth);
            console.log(bottomBar.width);
        }
    }

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
}
