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

Rectangle {
    id: plDisplay

    property int default_width: vlc_style.defaultWidthPlaylist

    // Force to recalculate the colors
    function changedNightMode() {
        color = medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode
        reloadData();
    }

    // Force the data inside the listview to de reloaded
    function reloadData() {
        listView.model = [];
        listView.model = playlist;
    }

    width: default_width
    color: medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode

    /* Button to show/hide the playlist */
    Image {
        id: toogleBar

        x: -width
        width: vlc_style.icon_small
        height: vlc_style.icon_small

        fillMode: Image.PreserveAspectFit
        source: "qrc:///toolbar/playlist.svg"
        mirror: plDisplay.width <= vlc_style.closedWidthPlaylist ? true : false

        MouseArea {
            anchors.fill: parent
            onClicked: { plDisplay.width <= vlc_style.closedWidthPlaylist ? openAnimation.running = true : closeAnimation.running = true }
        }
    }

    /* List of items in the playlist */
    PlaylistListView {
        id: listView

        height: parent.height
        width: parent.width

        vertSpace: vlc_style.margin_xxxsmall
        horiSpace: vlc_style.margin_xxsmall
        model: playlist
        dataFunc: function( elt ) { return elt.album_title; }
        delegate: PLItem {}
        delegateGrouped: PLItem { showCover: false }
        commonGrouped: Image {
            width: vlc_style.cover_xsmall
            height: vlc_style.cover_xsmall

            fillMode: Image.PreserveAspectFit
            source: model !== undefined ? (model.cover || vlc_style.noartCover) : vlc_style.noartCover
        }

        ScrollBar.vertical: ScrollBar { }
    }

    /* Hiding animation of the playlist */
    PropertyAnimation {
        id: closeAnimation

        target: plDisplay
        properties: "width"
        duration: vlc_style.timingPlaylistClose
        to: 0
        easing.type: Easing.InOutCubic
    }

    /* Showing animation of the playlist */
    PropertyAnimation {
        id: openAnimation

        target: plDisplay
        properties: "width"
        duration: vlc_style.timingPlaylistOpen
        to: default_width
        easing.type: Easing.InOutCubic
    }

}


