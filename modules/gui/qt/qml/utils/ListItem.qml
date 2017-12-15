/*****************************************************************************
 * ListItem.qml : Item displayed inside a list view
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

MLItem {
    id: root

    property Component cover: Item{}
    property Component line1: Item{}
    property Component line2: Item{}

    signal playClicked
    signal addToPlaylistClicked

    Row {
        id: main_row

        anchors.fill: parent

        spacing: vlc_style.margin_small

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
                width: parent.width

                sourceComponent: line1
            }

            /* Line 2 */
            Loader {
                width: parent.width

                sourceComponent: line2
            }
        }

        /* The icon to add to playlist */
        Image {
            id: add_to_playlist_icon

            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: vlc_style.margin_small
            width: vlc_style.icon_small
            height: vlc_style.icon_small

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
            anchors.rightMargin: vlc_style.margin_small
            width: vlc_style.icon_small
            height: vlc_style.icon_small

            visible: root.active()
            source: "qrc:///toolbar/play_b.svg"

            MouseArea {
                anchors.fill: parent

                onClicked: root.playClicked()
            }
        }
    }
}
