/*****************************************************************************
 * about.qml : About dialog display
 ****************************************************************************
 * Copyright (C) 2007 the VideoLAN team
 * $Id: 45191c4b3e2e9fc2285cbedc72bcde0cc5811cd5 $
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

/* Root Item */
Item {
    id: root
    width: 1000
    height: 450

    /* Main window definition */
    Rectangle {
        id: background
        color: "#f5f5f5"
        anchors.fill: parent

        GridLayout {
            id: mainLayout
            columnSpacing: 10
            rowSpacing: 10
            rows: 5
            columns: 4
            anchors.fill: parent

            Rectangle {
                id: spacerTop
                width: 200
                height: 200
                color: "#ffffff"
                Layout.maximumHeight: 0
                Layout.minimumHeight: 0
                Layout.fillWidth: true
                Layout.rowSpan: 1
                Layout.columnSpan: 4
            }

            Rectangle {
                id: spacerLeft
                width: 200
                height: 200
                color: "#ffffff"
                Layout.maximumWidth: 0
                Layout.minimumWidth: 0
                Layout.fillHeight: true
                Layout.rowSpan: 3
            }

            /* VLC icon */
            Image {
                id: image
                Layout.maximumWidth: 256
                Layout.minimumWidth: 256
                Layout.fillHeight: false
                Layout.fillWidth: false
                fillMode: Image.PreserveAspectFit
                sourceSize.height: 0
                source: "../logo/vlc256.png"
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.row: 1
                Layout.column: 1
                Layout.rowSpan: 3
            }

            /* Title */
            Text {
                id: title
                text: titleText
                Layout.alignment: Qt.AlignVCenter
                Layout.row: 1
                Layout.column: 2
                textFormat: Text.RichText
            }

            Rectangle {
                id: spacerRight
                width: 200
                height: 200
                color: "#00000000"
                Layout.minimumWidth: 0
                Layout.maximumWidth: 0
                Layout.fillHeight: true
                Layout.rowSpan: 3
            }

            /* Version */
            Text {
                id: version
                color: "#ff000000"
                text: versionText
                font.pointSize: 12
                font.family: "Courier"
                font.weight: Font.Light
                Layout.alignment: Qt.AlignVCenter
                Layout.row: 2
                Layout.column: 2
                textFormat: Text.RichText

                MouseArea {
                    anchors.fill: parent
                    anchors.margins: -10
                    onClicked: version.state == "buildinfo" ? version.state = "" : version.state = "buildinfo"
                }

                states: [
                    State {
                        name: "buildinfo"
                        PropertyChanges { target: version ; text: buildinfoText }
                    }
                ]

            }

            /* Blabla */
            TextArea {
                id: blabla
                text: mainBlablaText
                backgroundVisible: false
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.row: 3
                Layout.column: 2
                readOnly: true
                textFormat: TextEdit.AutoText
                onLinkActivated: Qt.openUrlExternally(link)
                wrapMode: Text.WordWrap

                /* States for the blabla text */
                states: [
                    State {
                        name: "authors"
                        PropertyChanges {
                            target: blabla
                            text: authorsBlablaText
                        }
                    },
                    State {
                        name: "license"
                        PropertyChanges {
                            target: blabla
                            text: licenseBlablaText
                        }
                    },
                    State {
                        name: "credits"
                        PropertyChanges {
                            target: blabla
                            text: creditsBlablaText
                        }
                    }
                ]

            }

            /* Footer to change infos */
            Rectangle {
                id: footer
                width: 200
                height: 200
                color: "#e6e6e6"
                Layout.maximumHeight: 60
                Layout.minimumHeight: 60
                Layout.fillHeight: false
                Layout.fillWidth: true
                Layout.columnSpan: 4
                Layout.row: 4
                Layout.column: 0

                RowLayout {
                    id: footerLayout
                    anchors.fill: parent

                    /* Authors button */
                    Text {
                        id: authors
                        text: authorsText
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        textFormat: Text.RichText

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10
                            onClicked: blabla.state == "authors" ? blabla.state = "" : blabla.state = "authors"
                        }

                    }

                    /* License button */
                    Text {
                        id: license
                        text: licenseText
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        textFormat: Text.RichText

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10
                            onClicked: blabla.state == "license" ? blabla.state = "" : blabla.state = "license"
                        }

                    }

                    /* Credits button */
                    Text {
                        id: credits
                        text: creditsText
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        textFormat: Text.RichText

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10
                            onClicked: blabla.state == "credits" ? blabla.state = "" : blabla.state = "credits"
                        }
                    }
                }
            }
        }
    }
}
