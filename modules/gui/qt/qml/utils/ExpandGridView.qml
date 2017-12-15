/*****************************************************************************
 * ExpandGridView.qml : Grid view that can expand an array when an item is clicked
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

Flickable {
    id: root

    property int rowSpacing: 0          // Space between two rows
    property int colSpacing: 0          // Space between two columns
    property int expandSpacing: 0       // Space between a row and the expanded zone
    property bool fillWidth: false      // Should the space between cols been maximized ?
    property bool fillHeight: false     // Should the space between rows been maximized ?
    // Should the width of the expanded zone be the root's width (true)
    // or the row's width (false) (different when there is space left at end of row) ?
    property bool expandFillWidth: false

    property int cellHeight: 0          // Height of a cell
    property int cellWidth: 0           // Width of a cell
    property int expandHeight: 0        // Height of the expanded zone
    // Should the height of the expanded zone be expandHeight or should it be compacted
    // so it's the minimum between expandHeight and the height of the expandDelegate ?
    property bool expandCompact : false
    // Should the height of the expanded zone be always the height of the epxand Delegate ?
    // (if true, properties expandHeight and expandCompact becom useless)
    property bool expandAdaptHeight: false

    property int expandDelay: 0         // The delay before the expanding animation starts (in ms)
    property int collapseDelay: 0       // The delay before the collapsing animation starts (in ms)
    property int expandDuration: 0      // The time the expanding animation lasts (in ms)
    property int collapseDuration: 0    // The time the collapsing animation lasts (in ms)

    property var model                  // The model (data) to use
    property Component delegate         // The delegate for the cells
    property Component expandDelegate   // The delegate for the expanding zone
    
    QtObject {
        id: priv
        property int itemsPerRow: Math.floor( width/(cellWidth+colSpacing) )
        property int nbLines: Math.ceil( nb_item(root.model) / itemsPerRow )
        property int expandItemIndex: -1
    }

    function nb_item(data) {
        if (data.count !== undefined)
            return data.count;
        if (data.length !== undefined)
            return data.length;
        if (data.rowCount !== undefined)
            return data.rowCount();
        if (data.columnCount !== undefined)
            return data.columnCount();
        console.log('Impossible to get number of items from '+data)
        return 0;
    }

    function calc_begin(row_num) {
        return row_num * priv.itemsPerRow;
    }
    function calc_end(row_num) {
        return Math.min(
            calc_begin(row_num+1) - 1,
            nb_item(model) - 1
        );
    }
    function calc_row_spacing() {
        if (fillHeight) {
            if (priv.expandItemIndex != -1)
                return Math.floor( (root.height-expandHeight-expandSpacing-priv.nbLines*cellHeight) / (priv.nbLines-1) )
            else
                return Math.floor( (root.height-priv.nbLines*cellHeight) / (priv.nbLines-1) )
        } else {
            return rowSpacing
        }
    }
    function calc_content_height () {
        if (fillHeight) {
            return height;
        } else {
            if (priv.expandItemIndex != -1)
                return priv.nbLines*(cellHeight+rowSpacing) - rowSpacing + expandSpacing + expandHeight;
            else
                return priv.nbLines*(cellHeight+rowSpacing) - rowSpacing;
        }
    }

    function toggleExpandAll( i ) {
        if (i != priv.expandItemIndex && i >= 0 && i <= nb_item(model) )
            priv.expandItemIndex = i;
        else
            priv.expandItemIndex = -1;
    }

    contentHeight: calc_content_height();
    contentWidth: width

    onModelChanged: {
        row_repeater.model = 0;
        row_repeater.model = Qt.binding(function() { return priv.nbLines } );
    }
    
    Column {
        anchors.fill: parent

        spacing : calc_row_spacing()

        Repeater {
            id: row_repeater
            model: priv.nbLines

            ExpandGridViewRow {
                beginIndex: calc_begin(index)
                endIndex: calc_end(index)
                expandItemIndex: priv.expandItemIndex

                model: root.model
                delegate: root.delegate
                expandDelegate: root.expandDelegate

                colSpacing: root.colSpacing
                rowSpacing: root.rowSpacing
                expandSpacing: root.expandSpacing
                fillWidth: root.fillWidth
                fillHeight: root.fillHeight
                expandFillWidth: root.expandFillWidth
                rootMaxWidth: root.width

                cellHeight: root.cellHeight
                cellWidth: root.cellWidth
                expandHeight: root.expandHeight
                expandCompact: root.expandCompact
                expandAdaptHeight : root.expandAdaptHeight

                expandDelay: root.expandDelay
                collapseDelay: root.collapseDelay
                expandDuration: root.expandDuration
                collapseDuration: root.collapseDuration

                itemsPerRow: priv.itemsPerRow

                function toggleExpand(i) {
                    toggleExpandAll(i);
                }
            }
        }
    }
}
