/*****************************************************************************
 * plitem.hpp : Playlist item
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

#ifndef PLITEM_HPP
#define PLITEM_HPP

#include "qt.hpp"
#include <qt5/QtCore/QString>
#include "components/mediacenter/mlalbumtrack.hpp"


class PLItem
{
public:
    PLItem( MLAlbumTrack* _item);
    QString getTitle();
    QString getAlbumTitle();
    QString getName();
    QString getDuration();
    QString getCover();

    input_item_t* getInputItem() { return inputItem; }

    void activate( playlist_t* pl );

private:
    MLAlbumTrack* getItem() { return pl_item; }

private:
    MLAlbumTrack* pl_item;

    input_item_t* inputItem;
};

#endif // PLITEM_HPP
