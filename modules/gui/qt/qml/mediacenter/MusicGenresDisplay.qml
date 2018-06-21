/*****************************************************************************
 * MusicGenresDisplay.qml : Component to display when category is "genres"
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

                cover: Utils.GenreCover { albums: model.genre_albums }
                name: model.genre_name || "Unknown genre"

                onItemClicked: {
                    console.log('Clicked on details : '+model.genre_name);
                    medialib.select( index );
                }
                onPlayClicked: {
                    console.log('Clicked on play : '+model.genre_name);
                    medialib.addAndPlay(index)
                }
                onAddToPlaylistClicked: {
                    console.log('Clicked on addToPlaylist : '+model.genre_name);
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
            spacing: 2

            model: medialib.getObjects()
            delegate : Utils.ListItem {
                height: VLCStyle.heightBar_small
                width: parent.width

                line1: Text{
                    text: (model.genre_name || "Unknown genre")+" - "+model.genre_nb_tracks+" tracks"
                    font.bold: true
                    elide: Text.ElideRight
                    color: VLCStyle.textColor
                }

                onItemClicked: {
                    console.log("Clicked on : "+model.genre_name);
                    medialib.select( index );
                }
                onPlayClicked: {
                    console.log('Clicked on play : '+model.genre_name);
                    medialib.addAndPlay(index)
                }
                onAddToPlaylistClicked: {
                    console.log('Clicked on addToPlaylist : '+model.genre_name);
                    medialib.addToPlaylist(index);
                }
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}
