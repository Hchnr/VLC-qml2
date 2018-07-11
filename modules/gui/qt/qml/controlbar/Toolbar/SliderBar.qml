import QtQuick 2.0

Item {
    id: slider
    height: 26

    property real min: 0
    property real max: 1
    property real value: min + (max - min) * mousearea.value
    property real init: min+(max-min)/2
    property string name: "Slider"
    property color color: "#c62f2f"
    property real minLabelWidth: 44
    property alias handleMousearea: mousearea

    function setValue(v) {
       if (min < max)
          handle.x = Math.round( v / (max - min) *
                                (mousearea.drag.maximumX - mousearea.drag.minimumX)
                                + mousearea.drag.minimumX);
    }

    Rectangle{
        id: foo
        width: parent.width - 8
               // - sliderName.width
        color: "#eee"
        height: 7
        radius: 3
        antialiasing: true
        border.color: Qt.darker(color, 1.2)
        anchors.left: parent.left;
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 24
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            height: parent.height
            anchors.left: parent.left
            anchors.right: handle.horizontalCenter
            color: slider.color
            radius: 3
            border.width: 1
            border.color: Qt.darker(color, 1.3)
            opacity: 0.8
        }
        Rectangle {
            id: handle
            x: slider.value * (mousearea.drag.maximumX - mousearea.drag.minimumX)
                             + mousearea.drag.minimumX;
            width: 14;
            height: 14;
            radius: 10;
            color: "#FAFAFA"
            border{
                width: 1;
                color: "gray";
            }

            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                id: mousearea
                anchors.fill: parent
                anchors.margins: -4
                drag.target: parent
                drag.axis: Drag.XAxis
                drag.minimumX: -handle.width / 2;
                drag.maximumX: foo.width - handle.width / 2;
                property real value: (handle.x - drag.minimumX) / (drag.maximumX - drag.minimumX)

                function onReleasedSliderBar() {
                    slider.value = getPosSliderBar();
                }
                function getPosSliderBar() {
                    return (handle.x - drag.minimumX) / (drag.maximumX - drag.minimumX);
                }
            }
        }
    }
}
