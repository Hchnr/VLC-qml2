/*****************************************************************************
 * MainInterface.qml : Main QML component displaying the mediacenter, the
 *     playlist and the sources selection
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import "qrc:///style/"

import "qrc:///mediacenter/" as MC
import "qrc:///playlist/" as PL


Item {
    // The functions the C++ part can call
    function reloadData() { mcDisplay.reloadData();}
    function changedCategory() { mcDisplay.changedCategory(); }
    function changedView() { mcDisplay.changedView(); }
    function reloadPresentation() { mcDisplay.reloadPresentation(); }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        ColumnLayout {
            id: column

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Layout.fillWidth: true
            Layout.minimumWidth: VLCStyle.minWidthMediacenter
            spacing: 0

            /* Source selection*/
            BannerSources {
                id: sourcesBanner

                // function triggered when a source is selected
                function selectSource ( name ) {
                    medialib.selectSource(name);
                    subSourcesBanner.update();
                }

                z : 10
                height: VLCStyle.heightBar_normal
                Layout.preferredHeight: height
                Layout.minimumHeight: height
                Layout.maximumHeight: height
                Layout.fillWidth: true

                need_toggleView_button: true
            }

            /* Sub-source selection */
            SubBannerSources {
                id: subSourcesBanner

                // function triggered when a source is selected
                function selectSource ( name ) {
                    medialib.selectSource(name);
                    subSourcesBanner.update();
                }

                z : 10
                height: VLCStyle.heightBar_normal
                Layout.preferredHeight: height
                Layout.minimumHeight: height
                Layout.maximumHeight: height
                Layout.fillWidth: true
            }

            /* MediaCenter */
            MC.MCDisplay {
                id: mcDisplay

                z: 0
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

        }

        /* Playlist */
        PL.PLDisplay {
            id: plDisplay

            z: 20
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Layout.maximumWidth: VLCStyle.maxWidthPlaylist

            default_width: VLCStyle.defaultWidthPlaylist
        }
    }




}
