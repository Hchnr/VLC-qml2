/*****************************************************************************
 * PlaylistGrouped.qml : Group of similar items, used by PLaylistListView
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

Row {
    id: root

    property var model
    property int indexFirst
    property int indexLast
    property Component delegateGrouped
    property Component commonGrouped
    property alias vertSpace: group_col.spacing
    property alias horiSpace: root.spacing

    function get_item(i) {
        if (i<0 || i> get_nb() )
            return undefined;
        if (model.get && model.get(i))
            return model.get(i);
        if (model.itemData)
            return model.itemData(i);
        if (model[i])
            return model[i];
        console.log('Impossible to get item data from '+model+' at index '+i);
        return undefined;
    }

    Loader {
        sourceComponent: commonGrouped
        property int currentIndex: indexFirst
        property var model: get_item( indexFirst )
    }

    Column {
        id: group_col

        Repeater {
            model: indexLast - indexFirst + 1

            Loader {
                width: root.width
                property var model: get_item( indexFirst + index )
                property int currentIndex: indexFirst + index
                sourceComponent: delegateGrouped
            }
        }
    }
}
