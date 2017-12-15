/*****************************************************************************
 * PlaylistListView.qml : List view that can group similar items
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

Flickable{
    id: root

    /* Parameters */

    // The List/AbstractListModel of items to display
    property var model: []
    // A function that returns an object to compare (used
    // to determine if two items are in the same group)
    property var dataFunc: function( elt ) { return elt; }
    // The component used when item is not grouped
    property Component delegate: Item {}
    // The component used when item is grouped
    property Component delegateGrouped: Item {}
    // The component displayed at the begin of a group
    property Component commonGrouped: Item {}
    // The vertical space between playlist items
    property int vertSpace: 0
    // The horizontal space (between commonGrouped and delegateGrouped)
    property int horiSpace: 0


    /* Functions used by the internal code */

    // Return the number of elements in the model
    // (depends on the type of the model)
    function get_nb() {
        if (model.count !== undefined)
            return model.count;
        if (model.length !== undefined)
            return model.length;
        if (model.rowCount !== undefined)
            return model.rowCount();
        if (model.columnCount !== undefined)
            return model.columnCount();
        console.log('Impossible to get number of items from '+model)
        return 0;
    }

    // Return the element at position i in the model
    // (depends on the type of the model)
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

    // Return the index of the last item in indexFirst's group
    function endOfGroup(indexFirst) {
        if (indexFirst <0 || indexFirst>get_nb()) return indexFirst;

        var indexCur = indexFirst + 1;
        var dataFirst = dataFunc(get_item(indexFirst));
        while (indexCur < get_nb() && root.dataFunc(get_item(indexCur)) === dataFirst) {
            indexCur += 1;
        }
        return indexCur - 1;
    }
    // Check if item at position i is in the same group as the item in position i-1
    // False if it's the first item
    function groupedWithPrev( i ){
        var prev = get_item(i-1);
        var curr = get_item(i);
        if (prev === undefined || curr === undefined) return false;
        return dataFunc( prev ) === dataFunc( curr );
    }
    // Check if item at position i is in the same group as the item in position i+1
    // False if it's the last item
    function groupedWithNext( i ){
        return groupedWithPrev( i+1 );
    }


    contentHeight: main_col.height
    contentWidth: width

    Column {
        id: main_col
        spacing: vertSpace

        Repeater {
            id: repeater
            model : root.model


            Loader {
                // Choose either a simple item, a group of items, or nothing
                width: root.width
                property var model: get_item( index )
                property int currentIndex: index
                sourceComponent : {
                    if (index < 0 || index > get_nb()) return undefined; // index is wrong (happens when model is resetting)
                    if (groupedWithPrev( index )) return undefined; // item part of a group but not first one
                    if (groupedWithNext( index )) return group; // first item of a group
                    return delegate; // item not part of a group
                }

                Component{
                    id: group
                    Column {
                        spacing: vertSpace

                        PlaylistGrouped {
                            // a component displaying a full group of item
                            width: parent.width
                            model: root.model
                            indexFirst: index
                            indexLast: endOfGroup(index)
                            delegateGrouped: root.delegateGrouped
                            commonGrouped: root.commonGrouped
                            vertSpace: root.vertSpace
                            horiSpace: root.horiSpace
                        }
                    }
                }
            }
        }
    }
}
