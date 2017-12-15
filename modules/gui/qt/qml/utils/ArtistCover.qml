/*****************************************************************************
 * ArtistCover.qml : A component to display up to 4 albums covers in a grid layout
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

/********************************************************
 * If 2 or 3 items, some cover will be cropped to fit in
 * not square spaces
 * If 1 album : |---------| If 2 albums : |----|----|
 *              |         |               |    |    |
 *              |         |               |    |    |
 *              |         |               |    |    |
 *              |---------|               |----|----|
 * If 3 albums : |----|----| If 4+ albums : |----|----|
 *               |    |    |                |    |    |
 *               |    |----|                |----|----|
 *               |    |    |                |    |    |
 *               |----|----|                |----|----|
 ********************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    property var albums: undefined
    property int nb_albums: 0

    // Calculate the height the cover at index i
    // should take according to the number of items
    function calc_height(i) {
        if (nb_albums === 1) {
            return gridCover_id.height;
        } else if (nb_albums === 2) {
            return gridCover_id.height;
        } else if (nb_albums === 3) {
            if (i === 0) {
                return gridCover_id.height;
            } else {
                return gridCover_id.height/2 - gridCover_id.rowSpacing/2;
            }
        } else {
            return gridCover_id.height/2 - gridCover_id.rowSpacing/2;
        }
    }
    // Calculate the width the cover at index i
    // should take according to the number of items
    function calc_width(i) {
        if (nb_albums === 1) {
            return gridCover_id.width;
        } else if (nb_albums === 2) {
            return gridCover_id.width/2 - gridCover_id.columnSpacing/2;
        } else if (nb_albums === 3) {
            return gridCover_id.width/2 - gridCover_id.columnSpacing/2;
        } else {
            return gridCover_id.width/2 - gridCover_id.columnSpacing/2;
        }
    }
    // Calculate the number of row the cover at index i
    // should span according to the number of items
    function calc_rowSpanning(i) {
        if (nb_albums === 1) {
            return 2;
        } else if (nb_albums === 2) {
            return 2;
        } else if (nb_albums === 3) {
            if (i === 0) {
                return 2;
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }

    GridLayout {
        id: gridCover_id

        anchors.fill: parent

        columns: 2
        columnSpacing: vlc_style.margin_xxxsmall
        rowSpacing: vlc_style.margin_xxxsmall

        Repeater {
            model: albums

            /* One cover */
            Image {
                id: img

                Layout.rowSpan: calc_rowSpanning(index)
                Layout.preferredHeight: calc_height(index)
                Layout.preferredWidth: calc_width(index)

                source: model.album_cover || vlc_style.noartCover
                fillMode: Image.PreserveAspectCrop
                visible: index < 4

            }
        }
    }

    /* "..." label */
    // If there are more than 4 albums, display "..." to signal there are more
    Text {
        id: moreText

        anchors.right: parent.right
        anchors.bottom: parent.bottom

        text: "..."
        font.pixelSize: 30
        color: medialib.isNightMode() ? vlc_style.textColor_nightmode : vlc_style.textColor_daymode
        style: Text.Outline
        styleColor: medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode

        visible: nb_albums > 4
    }
}
