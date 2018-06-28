import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

ToolBar{
    height: parent.height
    RowLayout{
        height: parent.height
        Repeater {
            model: rightbarList
            delegate: ToolButton {
                // Text { text: model.modelData.widgetName }
                Image {
                    source: controlbar.getIconFromName(widgetName)
                }

                onClicked: {
                    if (widgetName === "TBD") {
                        emit: controlbar.testStop()
                    }
                }

            }
        }
    }
}

