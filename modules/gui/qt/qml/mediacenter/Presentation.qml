/*****************************************************************************
 * Presentation.qml : Details of the clicked item displayed at the top
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

Rectangle {
    id: root

    property var obj: undefined

    function changedNightMode() {
        color = medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode;
        title_id.color = medialib.isNightMode() ? vlc_style.textColor_nightmode : vlc_style.textColor_daymode;
        info_id.color = medialib.isNightMode() ? vlc_style.textColor_nightmode : vlc_style.textColor_daymode;
    }

    color : medialib.isNightMode() ? vlc_style.bgColor_nightmode : vlc_style.bgColor_daymode

    RowLayout {
        id: row_id

        anchors.fill: parent
        anchors.margins: vlc_style.margin_normal

        spacing: vlc_style.margin_normal

        /* A button to go back one step */
        Image {
            height: vlc_style.icon_normal
            Layout.maximumHeight: height
            Layout.minimumHeight: height
            Layout.preferredHeight: height

            source: "qrc:///toolbar/dvd_prev.svg"
            fillMode: Image.PreserveAspectFit

            MouseArea {
                anchors.fill: parent

                onClicked: medialib.backPresentation()
            }
        }

        /* The cover/image associated with the item */
        Image {
            id : image_id

            Layout.fillHeight: true
            height: vlc_style.heightAlbumCover_large
            Layout.maximumWidth: height
            Layout.minimumWidth: height
            Layout.preferredWidth: height

            source: obj.getPresImage() || vlc_style.noartCover
            fillMode: Image.PreserveAspectFit
        }

        ColumnLayout {
            id: col_id

            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: vlc_style.margin_normal

            /* the title/main name of the item */
            Text {
                id: title_id

                Layout.fillWidth: true
                Layout.maximumHeight: implicitHeight
                Layout.minimumHeight: implicitHeight
                Layout.preferredHeight: implicitHeight

                text: "<b>"+( obj.getPresName() || "Unknwon artist" )+"</b>"
                elide: Text.ElideRight
                color: medialib.isNightMode() ? vlc_style.textColor_nightmode : vlc_style.textColor_daymode
            }

            /* A description for the item */
            Text {
                id: info_id

                Layout.fillWidth: true
                Layout.fillHeight: true

                text: obj.getPresInfo()
                wrapMode: Text.WordWrap
                elide: Text.ElideRight
                color: medialib.isNightMode() ? vlc_style.textColor_nightmode : vlc_style.textColor_daymode
            }
        }
    }
}
