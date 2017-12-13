/*****************************************************************************
 * plitem.cpp : Playlist item
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

#include "plitem.hpp"

#include "vlc_input_item.h"

PLItem::PLItem(MLAlbumTrack *_item ) :
    pl_item( _item )
{
    inputItem = input_item_New(
        pl_item->getMRL().toLatin1().data(),
        pl_item->getTitle().toLatin1().data()
    );
}

QString PLItem::getTitle()
{
    return pl_item->getTitle();
}

QString PLItem::getAlbumTitle()
{
    return pl_item->getAlbumTitle();
}

QString PLItem::getName()
{
    return QString( "plop" );
}

QString PLItem::getDuration()
{
    return pl_item->getDuration();
}

QString PLItem::getCover()
{
    return pl_item->getCover();
}

void PLItem::activate( playlist_t *pl )
{

    playlist_item_t* playlist_item = playlist_ItemGetByInput(pl, inputItem);

    if( playlist_item )
        playlist_ViewPlay( pl, NULL, playlist_item );
}
