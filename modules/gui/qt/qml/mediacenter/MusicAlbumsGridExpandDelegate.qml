/*****************************************************************************
 * MusicAlbumsGridExpandDelegate.qml : Item in the expanded zone displayed
 *     when an album is clicked
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

import "qrc:///utils/" as Utils
import "qrc:///style/"

Row {
    id: root

    x: VLCStyle.margin_xsmall

    spacing: VLCStyle.margin_xsmall

    /* A bigger cover for the album */
    Image {
        id: expand_cover_id

        width: VLCStyle.cover_large
        height: VLCStyle.cover_large

        source: model.album_cover || VLCStyle.noArtCover
    }

    Column {
        id: expand_infos_id

        width: root.width - expand_cover_id.width - root.spacing

        spacing: VLCStyle.margin_xsmall

        /* The title of the albums */
        // Needs a rectangle too prevent the tracks from overlapping the title when scrolled
        Rectangle {
            id: expand_infos_titleRect_id

            height: expand_infos_title_id.implicitHeight
            width: expand_infos_id.width

            color: VLCStyle.bgColor

            Text {
                id: expand_infos_title_id

                text: "<b>"+(model.album_title || "Unknown title")+"</b>"
                color: VLCStyle.textColor
            }
        }

        /* The list of the tracks available */
        Utils.TracksDisplay {
            x: 30
            z: expand_infos_titleRect_id.z - 1
            height: Math.min(
                root.height - expand_infos_titleRect_id.height - expand_infos_id.spacing,
                model.album_nb_tracks * (
                    expand_infos_id.spacing + VLCStyle.margin_xxxsmall + VLCStyle.fontSize_normal
                ) - VLCStyle.margin_xxxsmall - expand_infos_id.spacing
            )
            width: expand_infos_id.width - x

            tracks: model.album_tracks
            parentIndex: currentIndex
        }
    }
}
