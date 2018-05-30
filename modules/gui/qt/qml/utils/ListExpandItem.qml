/*****************************************************************************
 * ListExpandItem.qml : Item in the expanded zone displayed when an album is
 *     clicked in the list view
 ****************************************************************************
 * Copyright (C) 2006-2011 VideoLAN and AUTHORS
 * $Id$
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
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

import QtQuick 2.0
import "qrc:///style/"

MLItem {
    id: root

    property Component cover: Item{}
    property Component line1: Item{}
    property Component line2: Item{}
    property Component expand: Item{}

    signal playClicked
    signal addToPlaylistClicked

    onItemClicked: root.state = root.state === "expanded" ? "" : "expanded"
    force_disable: root.state === "expanded"

    Row {
        id: main_row

        anchors.fill: parent

        spacing: VLCStyle.margin_xsmall

        /* The cover */
        Loader {
            id: cover_loader

            sourceComponent: cover
        }

        Column {
            height: parent.height
            width: parent.width - cover_loader.width - parent.spacing*3 - add_to_playlist_icon.width - add_and_play_icon.anchors.rightMargin - add_and_play_icon.width

            /* Line 1 */
            Loader {
                id: line1_loader

                width: parent.width

                sourceComponent: line1
            }

            /* Line 2 */
            Loader {
                id: line2_loader

                width: parent.width

                sourceComponent: line2
            }

            /* Expand */
            Loader {
                id: expand_loader

                width: parent.width

                visible: false
                sourceComponent: expand
            }
        }

        /* The icon to add to playlist */
        Image {
            id: add_to_playlist_icon

            anchors.verticalCenter: parent.verticalCenter
            width: VLCStyle.icon_small
            height: VLCStyle.icon_small

            visible: root.active()
            source: "qrc:///buttons/playlist/playlist_add.svg"

            MouseArea {
                anchors.fill: parent

                onClicked: root.addToPlaylistClicked()
            }
        }

        /* The icon to add to playlist and play */
        Image {
            id: add_and_play_icon

            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: VLCStyle.margin_small
            width: VLCStyle.icon_small
            height: VLCStyle.icon_small

            visible: root.active()
            source: "qrc:///toolbar/play_b.svg"

            MouseArea {
                anchors.fill: parent

                onClicked: root.playClicked()
            }
        }
    }

    states: State {
        name: "expanded"
        PropertyChanges { target: expand_loader; visible: true }
        PropertyChanges { target: cover_loader.item; state: "expanded"}
        PropertyChanges { target: root; height: Math.max( cover_loader.item.height, line1_loader.item.height + line2_loader.item.height + expand_loader.item.height ) }
    }
}
