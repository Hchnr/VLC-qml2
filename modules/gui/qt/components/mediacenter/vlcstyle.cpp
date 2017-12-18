/*****************************************************************************
 * vlcstyle.cpp : A group of property to define a style for VLC
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

#include "vlcstyle.hpp"

VLCStyle::VLCStyle() : QQmlPropertyMap()
{
#define prop(name,value) insert(name, QVariant(value))
    prop("margin_xxxsmall", 2);
    prop("margin_xxsmall", 4);
    prop("margin_xsmall", 8);
    prop("margin_small", 12);
    prop("margin_normal", 16);
    prop("margin_large", 24);
    prop("margin_xlarge", 32);

    prop("fontSize_xsmall", 8);
    prop("fontSize_small", 10);
    prop("fontSize_normal", 12);
    prop("fontSize_large", 14);
    prop("fontSize_xlarge", 16);

    prop("heightAlbumCover_xsmall", 32);
    prop("heightAlbumCover_small", 64);
    prop("heightAlbumCover_normal", 128);
    prop("heightAlbumCover_large", 256);
    prop("heightAlbumCover_xlarge", 512);

    prop("icon_xsmall", 8);
    prop("icon_small", 16);
    prop("icon_normal", 32);
    prop("icon_large", 64);
    prop("icon_xlarge", 128);

    prop("cover_xsmall", 64);
    prop("cover_small", 96);
    prop("cover_normal", 128);
    prop("cover_large", 160);
    prop("cover_xlarge", 192);

    prop("heightBar_xsmall", 8);
    prop("heightBar_small", 16);
    prop("heightBar_normal", 32);
    prop("heightBar_large", 64);
    prop("heightBar_xlarge", 128);
    prop("heightBar_xxlarge", 256);

    prop("minWidthMediacenter", 500);
    prop("maxWidthPlaylist", 400);
    prop("defaultWidthPlaylist", 300);
    prop("closedWidthPlaylist", 20);

    prop("bgColor_daymode", "#FFFFFF");
    prop("bgColor_nightmode", "#000000");
    prop("bgColor_removeFromPlaylist", "#CC0000");
    prop("hoverBgColor_daymode", "#F0F0F0");
    prop("hoverBgColor_nightmode", "#0F0F0F");
    prop("textColor_daymode", "#000000");
    prop("textColor_nightmode", "#FFFFFF");
    prop("textColor_activeSource", "#FF0000");
    prop("textColor_removeFromPlaylist", "#FFFFFF");
    prop("bannerColor_daymode", "#e6e6e6");
    prop("bannerColor_nightmode", "#191919");
    prop("hoverBannerColor_daymode", "#d6d6d6");
    prop("hoverBannerColor_nightmode", "#292929");
    prop("vlc_orange", "#FF950D");

    prop("widthSortBox", 150);

    prop("delayToolTipAppear", 500);
    prop("timingPlaylistClose", 1000);
    prop("timingPlaylistOpen", 1000);
    prop("timingGridExpandOpen", 200);
    prop("timingListExpandOpen", 200);

    prop("noartCover", "qrc:///noart.png");
#undef prop
}
