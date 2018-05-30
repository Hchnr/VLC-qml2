/*****************************************************************************
 * MusicTracksDisplay.qml : Component to display when category is "tracks"
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

import "qrc:///utils/" as Utils
import "qrc:///style/"

Loader {
    id: viewLoader

    // notify when the view has changed
    function changedView() {
        viewLoader.sourceComponent = medialib.isGridView() ? gridViewComponent_id : listViewComponent_id;
        console.log("View changed");
    }
    // Force the data to be reloaded
    function reloadData() {
        viewLoader.item.model = medialib.getObjects();
        console.log( "Data reloaded" );
    }

    sourceComponent: medialib.isGridView() ? gridViewComponent_id : listViewComponent_id

    /* Grid View */
    Component {
        id: gridViewComponent_id

        GridView {
            id: gridView_id

            cellWidth: VLCStyle.cover_normal
            cellHeight: VLCStyle.cover_normal + VLCStyle.fontSize_small + VLCStyle.margin_xsmall

            model: medialib.getObjects()
            delegate : Utils.GridItem {
                width: gridView_id.cellWidth
                height: gridView_id.cellHeight

                cover: Image { source: model.track_cover || VLCStyle.noArtCover }
                name: model.track_title || "Unknown track"

                onItemClicked: console.log('Clicked on details : '+model.track_title)
                onPlayClicked: {
                    console.log('Clicked on play : '+model.track_title);
                    medialib.addAndPlay(index)
                }
                onAddToPlaylistClicked: {
                    console.log('Clicked on addToPlaylist : '+model.track_title);
                    medialib.addToPlaylist(index);
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }

    /* List View */
    Component {
        id: listViewComponent_id
        ListView {
            spacing: VLCStyle.margin_xxxsmall

            model: medialib.getObjects()
            delegate : Utils.ListItem {
                height: VLCStyle.heightBar_small
                width: parent.width

                line1: Text{
                    text: (model.track_title || "Unknown track")+" - "+model.track_duration
                    font.bold: true
                    elide: Text.ElideRight
                    color: VLCStyle.textColor
                }

                onItemClicked: console.log("Clicked on : "+model.track_title)
                onPlayClicked: {
                    console.log('Clicked on play : '+model.track_title);
                    medialib.addAndPlay(index)
                }
                onAddToPlaylistClicked: {
                    console.log('Clicked on addToPlaylist : '+model.track_title);
                    medialib.addToPlaylist(index);
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}
