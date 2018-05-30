/*****************************************************************************
 * PLDisplay.qml : Display of the playlist
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
import QtQuick.Controls 2.0
import "qrc:///style/"

Rectangle {
    id: plDisplay

    property int default_width: VLCStyle.defaultWidthPlaylist

    // Force the data inside the listview to de reloaded
    function reloadData() {
        listView.model = [];
        listView.model = playlist;
    }

    width: default_width
    color: VLCStyle.bgColor

    /* Button to show/hide the playlist */
    Image {
        id: toogleBar

        x: -width
        width: VLCStyle.icon_small
        height: VLCStyle.icon_small

        fillMode: Image.PreserveAspectFit
        source: "qrc:///toolbar/playlist.svg"
        mirror: plDisplay.width <= VLCStyle.closedWidthPlaylist ? true : false

        MouseArea {
            anchors.fill: parent
            onClicked: { plDisplay.width <= VLCStyle.closedWidthPlaylist ? openAnimation.running = true : closeAnimation.running = true }
        }
    }

    /* List of items in the playlist */
    DropArea {

        height: parent.height
        width: parent.width

        keys: [ "playlist" ]

        PlaylistListView {
            id: listView

            height: parent.height
            width: parent.width

            vertSpace: VLCStyle.margin_xxxsmall
            horiSpace: VLCStyle.margin_xxsmall
            model: playlist
            dataFunc: function( elt ) { return elt.album_title; }
            delegate: PLItem {
                draggedItemParent: plDisplay
            }
            delegateGrouped: PLItem {
                showCover: false
                draggedItemParent: plDisplay
            }
            commonGrouped: Image {
                width: VLCStyle.cover_xsmall
                height: VLCStyle.cover_xsmall

                fillMode: Image.PreserveAspectFit
                source: model !== undefined ? (model.cover || VLCStyle.noArtCover) : VLCStyle.noArtCover
            }

            ScrollBar.vertical: ScrollBar { }
        }

    }

    /* Hiding animation of the playlist */
    PropertyAnimation {
        id: closeAnimation

        target: plDisplay
        properties: "width"
        duration: VLCStyle.timingPlaylistClose
        to: 0
        easing.type: Easing.InOutCubic
    }

    /* Showing animation of the playlist */
    PropertyAnimation {
        id: openAnimation

        target: plDisplay
        properties: "width"
        duration: VLCStyle.timingPlaylistOpen
        to: default_width
        easing.type: Easing.InOutCubic
    }

}


