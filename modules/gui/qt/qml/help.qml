/*****************************************************************************
 * help.qml : Help dialog display
 ****************************************************************************
 * Copyright (C) 2007 the VideoLAN team
 * $Id: 45191c4b3e2e9fc2285cbedc72bcde0cc5811cd5 $
 *
 * Authors: Maël Kervella <dev@maelkervella.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2

Item {
    id: item1
    width: 400
    height: 450

    Rectangle {
        id: rectangle
        color: "#efebe7"
        anchors.fill: parent

        Button {
            id: button
            x: 310
            y: 150
            text: qsTr("Close")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            onClicked: dialog.hide()
        }

        TextArea {
            id: textArea
            x: 10
            text: helpText
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.bottom: button.top
            anchors.bottomMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            readOnly: true
            textFormat: TextEdit.AutoText
            onLinkActivated: Qt.openUrlExternally(link)
            wrapMode: Text.WordWrap
        }
    }
}
