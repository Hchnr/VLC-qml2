import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.3

import "qrc:///style/"

Rectangle {
    id: controlbar
    color: VLCStyle.bgColor
    height: sliderBar.height + toolBar.height
    property alias sliderBar: sliderBar
    property alias toolBar: toolBar

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: toolbarInformation.fullscreenModel.enterEvent();
        onExited: toolbarInformation.fullscreenModel.leaveEvent();
        onMouseXChanged: toolbarInformation.fullscreenModel.moveEvent();
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
            visible: toolbarInformation.fullscreenModel.isVisiable
            onVisibleChanged: toolbarInformation.adjust(toolbarInformation.fullscreenModel.isVisiable);

            property alias centerToolbar: centerToolbar
            LeftToolbar{
                id: leftToolbar
                anchors.left: parent.left
            }

            CenterToolbar{
                id: centerToolbar
                anchors.horizontalCenter: parent.horizontalCenter
            }


            SliderBar{
                id: soundSlider
                visible: (parent.width > centerToolbar.width + width)
                width:VLCStyle.widthVolumeSlider
                anchors.right: rightToolbar.visible ? rightToolbar.left
                                                    : parent.right
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
