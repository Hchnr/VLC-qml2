import QtQuick 2.0

Text {
    anchors.horizontalCenter: parent.horizontalCenter
    y: parent.height - 40 - subtitleText.height

    font.family: "Ubuntu"
    font.pixelSize: 28
    color: "white"
    verticalAlignment: Text.AlignTop
    horizontalAlignment: Text.AlignHCenter
    wrapMode: Text.WordWrap
    text: qsTr("subtitle here. subtitle here. subtitle here. subtitle here.")

    Component.onCompleted: {
        // should load srt file here
    }
    property string en: "Don't you do it.\nDon't you fuckin' do it.\n"
    property string fr: "Les gens qui sont dans nos rêves la nuit,\n Les gens qui sont dans nos rêves la nuit, faudrait toujours les appeler le matin au réveil. La vie serait plus simple\n"
    property string zh: "曾经有一份真挚爱情放在我面前但是我没有珍惜,\n 等到失去时我才后悔莫及.\n"
    property string chosen: en
    function getSubtitle(index) {
        var texts = chosen;
        var textList = texts.split('\n');
        return textList[index];
    }

    Timer{
        id: subtitleTimer
        interval: 2000;
        repeat: true;
        running: true;
        property int i: 0
        onTriggered: {
            parent.text = getSubtitle(i)
            if(i==0) i=1;
            else     i=0;
        }
    }
}


