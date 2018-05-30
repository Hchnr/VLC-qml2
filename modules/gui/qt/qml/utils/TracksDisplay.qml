/*****************************************************************************
 * TracksDisplay.qml : A component to display a listview of tracks
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

Item {
    property var tracks: []
    property int parentIndex: 0

    ListView {
        id: expand_track_id

        x: VLCStyle.margin_xlarge
        height: parent.height
        width: parent.width - x

        spacing: VLCStyle.margin_xxxsmall
        model: tracks

        delegate: ListItem {
            height: VLCStyle.fontSize_normal
            width: parent.width

            line1: Text{
                text: (model.track_title || "Unknown track")+" - "+model.track_duration
                elide: Text.ElideRight
                font.pixelSize: VLCStyle.fontSize_normal
                color: VLCStyle.textColor
            }

            onItemClicked: console.log("Clicked on : "+model.track_title)
            onPlayClicked: {
                console.log('Clicked on play : '+model.track_title);
                medialib.addAndPlay(parentIndex, index);
            }
            onAddToPlaylistClicked: {
                console.log('Clicked on addToPlaylist : '+model.track_title);
                medialib.addToPlaylist(parentIndex, index);
            }
        }

        ScrollBar.vertical: ScrollBar { }
    }
}
