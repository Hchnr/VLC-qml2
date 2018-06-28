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
        model: subtitleMenuItems
        delegate: MenuItem {
            text: name

            onClicked: {
                console.log(name + " clicked.")
                if (name === "English" ) {
                    videoWidget.subtitleText.chosen = videoWidget.subtitleText.en
                }
                else if(name === "Chinese") {
                    videoWidget.subtitleText.chosen = videoWidget.subtitleText.zh
                }
                else if(name === "France") {
                    videoWidget.subtitleText.chosen = videoWidget.subtitleText.fr
                }

            }
        }
    }

    ListModel {
        id: subtitleMenuItems
        ListElement {name: "Open a file"; abbr: ""}
        ListElement {name: "English"; abbr: "[en]"}
        ListElement {name: "Chinese"; abbr: "[zh]"}
        ListElement {name: "France"; abbr: "[fr]"}
    }
}
