/*****************************************************************************
 * MCDisplay.qml : The main component to display the mediacenter
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
import QtQuick.Layouts 1.3

Rectangle {
    // Notify the view has beeen changed
    function changedView() {
        viewLoader.item.changedView();
    }

    // Notify the category has been changed
    function changedCategory() {
        viewLoader.sourceComponent = chooseCat();
        reloadData();
        console.log( "Changed category : "+medialib.getCategory() );
    }

    // Function to get which component to display according to the category
    function chooseCat() {
        var cat = medialib.getCategory();
        if (cat === 0)
            return albumsDisplayComponent;
        else if (cat === 1)
            return artistsDisplayComponent;
        else if (cat === 2)
            return genresDisplayComponent;
        else if (cat === 3)
            return tracksDisplayComponent;
        else
            return albumsDisplayComponent;
    }

    // Force the data inside the displayed view to de reloaded
    function reloadData() {
        viewLoader.item.reloadData();
    }

    // Force to recalculate if the presentation needs to be displayed
    function reloadPresentation() {
        if ( medialib.hasPresentation() ) {
            presentationLoader_id.sourceComponent = presentationComponent_id;
        } else {
            presentationLoader_id.sourceComponent = noPresentationComponent_id;
       }
    }

    // Force to recalculate the colors
    function changedNightMode() {
        color = medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode
        if ( medialib.hasPresentation() ) {
            presentationLoader_id.item.changedNightMode();
        }
        reloadData();
    }

    color: medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode

    ColumnLayout {
        anchors.fill : parent

        /* The Presentation Bar */
        Loader {
            id: presentationLoader_id

            z:10
            Layout.fillWidth: true
            height: item.height
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height

            sourceComponent: medialib.hasPresentation() ? presentationComponent_id : noPresentationComponent_id

            // If the presentation bar should be displayed
            Component {
                id: presentationComponent_id

                Presentation {
                    height: vlc_style.heightBar_xlarge
                    Layout.preferredHeight: height
                    Layout.minimumHeight: height
                    Layout.maximumHeight: height

                    obj: medialib.getPresObject();
                }
            }
            // If the presentation bar should be hidden
            Component {
                id: noPresentationComponent_id

                Item {
                    height: 0
                    Layout.preferredHeight: height
                    Layout.minimumHeight: height
                    Layout.maximumHeight: height
                }
            }
        }

        /* The data elements */
        Loader {
            id: viewLoader

            z: 0
            Layout.fillWidth: true
            Layout.fillHeight: true

            sourceComponent: chooseCat()

            // Display some 'Artists' items
            Component {
                id: albumsDisplayComponent

                MusicAlbumsDisplay { }
            }

            // Display some 'Albums' items
            Component {
                id: artistsDisplayComponent

                MusicArtistsDisplay { }
            }

            // Display some 'Genres' items
            Component {
                id: genresDisplayComponent

                MusicGenresDisplay { }
            }

            // Display some 'Tracks' items
            Component {
                id: tracksDisplayComponent

                MusicTracksDisplay { }
            }
        }
    }
}
