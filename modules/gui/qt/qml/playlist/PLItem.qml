/*****************************************************************************
 * PLItem.qml : PLaylist item displayed
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

    // Should the cover be displayed
    property alias showCover: cover.visible

    // This item will become the parent of the dragged item during the drag operation
    property alias draggedItemParent: draggable_item.draggedItemParent

    // Is this item the one currently playing
    function is_current() {
        if (model === undefined) return False
        return model.current
    }
    // Check if the title can be retrieved from the model
    function get_title() {
        if (model === undefined) return ""
        return model.title
    }
    // Check if the duration can be retrieved from the model
    function get_duration() {
        if (model === undefined) return ""
        return model.duration
    }

    // Calculate the correct color for the background
    function calc_bgColor() {
        if (delete_mouseArea.containsMouse)
            return VLCStyle.bgColor_removeFromPlaylist;
        else if (draggable_item.containsMouse)
            return VLCStyle.hoverBgColor;
        else return VLCStyle.bgColor;

    }
    // Calculate the text to display
    function calc_text() {
        if (get_duration()) {
            if (get_title()) return '[' + get_duration() + '] ' + get_title();
            else return '[' + get_duration() + '] ';
        }
        else {
            if (get_title()) return get_title();
            else return "";
        }
    }
    // Calculate the correct color for the text to display
    function calc_textColor() {
        if (delete_mouseArea.containsMouse)
            return VLCStyle.textColor_removeFromPlaylist
        return VLCStyle.textColor
    }

    height: bg.height

    spacing: 0

    /* Cover of the associated album */
    Image {
        id: cover

        width: visible ? VLCStyle.icon_normal : 0
        height: visible ? VLCStyle.icon_normal : 0

        fillMode: Image.PreserveAspectFit
        source: model !== undefined ? (model.cover || VLCStyle.noArtCover) : VLCStyle.noArtCover
    }

    /* Button to remove this item from playlist */
    Image {
        id: removeButton

        height: VLCStyle.icon_small
        width: VLCStyle.icon_small

        source: "qrc:///toolbar/clear.svg"
        fillMode: Image.PreserveAspectFit

        MouseArea {
            id: delete_mouseArea

            anchors.fill: parent

            hoverEnabled: true
            onClicked: playlist.remove_item(currentIndex)
        }
    }

    Utils.DraggableItem {
        id : draggable_item

        onMoveItemRequested: { playlist.move(from, to) }
        onDoubleClicked: playlist.play_item(currentIndex)

        Rectangle {
            id: bg

            width : root.width - removeButton.width
            height: Math.max( textInfo.implicitHeight, cover.height )

            color: calc_bgColor();

            /* Title/name of the item */
            Text {
                id: textInfo

                x: VLCStyle.margin_small

                text: calc_text()
                color: calc_textColor()
            }
        }
    }
}
