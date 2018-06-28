import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import "qrc:///Toolbar/"

Window {
    id: fakePlayer
    visible: true
    width: 1000
    height: 960
    minimumWidth: 700
    title: qsTr("A Fake Interface")

    property bool isFullScreen: false
    property bool isToolbarVisible: true
    property bool isPlay: false
    property int initHeight: 960

    // get the icon path of widgets
    function getIconFromName(name) {
        var m = {"Bookmark": "image/buttons-svg/tv.svg",
              "Subtitle": "image/buttons-svg/messages.svg",
              "Random": "image/buttons-svg/shuffle_on.svg",
              "Loop": "image/buttons-svg/repeat_all.svg",
              "Slower": "image/buttons-svg/slower.svg",
              "Previous": "image/buttons-svg/dvd_prev.svg",
              "Play": "image/buttons-svg/play.svg",
              "Next": "image/buttons-svg/dvd_next.svg",
              "Faster": "image/buttons-svg/faster.svg",
              "Fullscreen": "image/buttons-svg/renderer.svg",
              "Playlist": "image/buttons-svg/playlist.svg",
              "TBD": "image/buttons-svg/stop.svg",
              "Stop": "image/buttons-svg/stop.svg",
        }
        return m[name]
    }

    color: "black"
    VideoWidget{
        id:videoWidget
        anchors.fill: parent
    }

    BottomToolbar {
        id: bottomBar
        anchors.bottom: parent.bottom
        color: "white"
    }

    function togglePlay() {
        bottomBar.toolBar.centerToolbar
        if (fakePlayer.isPlay) {
            image.source = fakePlayer.getIconFromName("Stop")
        }
        else {
            image.source = fakePlayer.getIconFromName("Play")
        }
        fakePlayer.isPlay = !fakePlayer.isPlay;
    }
}
