/*****************************************************************************
 * BannerSources.qml : Banner to display sources (Music, Video, Network, ...)
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
import "qrc:///style/"

Rectangle {
    id: pLBannerSources

    property bool need_toggleView_button: false

    // Triggered when the toogleView button is selected
    function toggleView () {
        medialib.toogleView();
    }
    // Triggered when the toogleView button is selected
    function toogleNightmode () {
        VLCStyle.toggleNightMode();
    }

    // Force the source to be redrawn
    function update() {
        sourcesButtons.model = undefined;
        sourcesButtons.model = buttonModel;
    }
    // Trigerred when a source is clicked
    // To be implemented by the parent
    function selectSource( name ) {
        return ;
    }

    color: VLCStyle.bannerColor

    Row {
        anchors.fill: parent

        /* Repeater to display each button */
        Repeater {
            id: sourcesButtons

            model: buttonModel
            delegate: buttonView
        }

        /* Model telling the text to display */
        // The associated image and the name to send to medialib
        // in order to notify to change the medialib model
        ListModel {
            id: buttonModel

            ListElement {
                displayText: "Music"
                pic: "qrc:///sidebar/music.svg"
                name: "music"
            }
            ListElement {
                displayText: "Video"
                pic: "qrc:///sidebar/movie.svg"
                name: "video"
            }
            ListElement {
                displayText: "Network"
                pic: "qrc:///sidebar/screen.svg"
                name: "network"
            }
        }

        /* Button for the sources */
        Component {
            id: buttonView

            Rectangle {
                id: rect

                height: parent.height
                width: txt.implicitWidth + icon.width + VLCStyle.margin_small*3

                color: VLCStyle.bannerColor

                /* Icon for this source */
                Image {
                    id: icon

                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                        rightMargin: VLCStyle.margin_xsmall
                        leftMargin: VLCStyle.margin_small
                    }
                    height: VLCStyle.icon_normal
                    width: VLCStyle.icon_normal

                    source: model.pic
                    fillMode: Image.PreserveAspectFit
                }

                /* Name of this source */
                Text {
                    id: txt

                    anchors {
                        left: icon.right
                        verticalCenter: parent.verticalCenter
                        rightMargin: VLCStyle.margin_small
                        leftMargin: VLCStyle.margin_xsmall
                    }

                    text: model.displayText
                    font.pixelSize: VLCStyle.fontSize_normal
                    color: VLCStyle.textColor
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: selectSource( model.name )
                    hoverEnabled: true
                    onEntered: { rect.color = VLCStyle.hoverBannerColor; }
                    onExited: { rect.color = VLCStyle.bannerColor; }
                }
            }
        }
    }

    /* button to choose the view displayed (list or grid) */
    Image {
        id: view_selector

        anchors.right: parent.right
        anchors.rightMargin: VLCStyle.margin_normal
        anchors.verticalCenter: parent.verticalCenter
        height: VLCStyle.icon_normal
        width: VLCStyle.icon_normal

        fillMode: Image.PreserveAspectFit
        source: "qrc:///toolbar/tv.svg"

        enabled: need_toggleView_button
        visible: need_toggleView_button

        MouseArea {
            anchors.fill: parent

            enabled: need_toggleView_button
            onClicked: toggleView()
        }
    }

    /* button to toogle between night and day mode */
    Image {
        anchors.right: view_selector.left
        anchors.rightMargin: VLCStyle.margin_small
        anchors.verticalCenter: parent.verticalCenter
        height: VLCStyle.icon_normal
        width: VLCStyle.icon_normal

        fillMode: Image.PreserveAspectFit
        source: "qrc:///prefsmenu/advanced/intf.svg"

        MouseArea {
            anchors.fill: parent

            enabled: need_toggleView_button
            onClicked: toogleNightmode()
        }
    }
}
