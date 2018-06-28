import QtQuick 2.6
import QtQuick.Controls 2.1

Menu {
/*
    MenuItem {
        text: "Cut"
        shortcut: "Ctrl+X"
        onTriggered: {}
    }
*/

    Repeater {
        model: contextMenuItems
        delegate: MenuItem {
            text: name
            hoverEnabled: true
            onClicked: {
                if (name === "Play/Pause") {
                    console.log("====")
                    bottomBar.toolBar.centerToolbar.centerToolbarRp.itemAt(2).onclicked()
                    // bottomBar.toolBar.centerToolbar.centerToolbarRp.itemAt(2)
                }
            }

        }
    }
    MenuSeparator{}

    ListModel {
        id: contextMenuItems
        ListElement {name: "Play/Pause"}
        ListElement {name: "Stop"}
        ListElement {name: "audio"}
        ListElement {name: "video"}
        ListElement {name: "playback"}
    }
}

