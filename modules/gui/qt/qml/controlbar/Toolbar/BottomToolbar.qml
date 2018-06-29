import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3

Rectangle {
    id: controlbar
    color: "white"
    // width: parent.width
    height: sliderBar.height + toolBar.height
    property alias sliderBar: sliderBar
    property alias toolBar: toolBar
    // signal testStop()

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
        }
        return m[name]
    }
    Component.onCompleted: {
        controls.doAction2(1)
        console.log("do this at root node.")
    }

    Column{
        anchors.bottom: parent.bottom
        width: parent.width
        SliderBar{
            id: sliderBar
            width: parent.width
            height: 10
            value: 0
            init : 0
        }
        RowLayout{
            id: toolBar
            width: parent.width
            height: 50
            property alias centerToolbar: centerToolbar
            LeftToolbar{
                anchors.left: parent.left
            }

            CenterToolbar{
                id: centerToolbar
                anchors.horizontalCenter: parent.horizontalCenter
            }

            RowLayout {
                anchors.right: parent.right
                SliderBar{
                    anchors.right: rightToolbar.left
                }
                RightToolbar{
                    id: rightToolbar
                    anchors.right: parent.right
                }
            }
        }
    }
}
