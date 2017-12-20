/*****************************************************************************
 * mediacenter_item.cpp : Manage mediacenter item
 ****************************************************************************
 * Copyright © 2006-2011 the VideoLAN team
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          Maël Kervella <dev@maelkervella.eu>
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>

#include "qt.hpp"
#include "mediacenter_item.hpp"
#include <vlc_input_item.h>

/*************************************************************************
 * Playlist item implementation
 *************************************************************************/

void MCItem::clearChildren()
{
    qDeleteAll( plitem_children );
    plitem_children.clear();
}

void MCItem::removeChild( MCItem *item )
{
    plitem_children.removeOne( item );
    delete item;
}

/*
   Playlist item is just a wrapper, an abstraction of the playlist_item
   in order to be managed by PLModel

   PLItem have a parent, and id and a input Id
*/

void MCItem::init( playlist_item_t *_playlist_item, MCItem *p_parent )
{
    parentItem = p_parent;          /* Can be NULL, but only for the rootItem */
    i_playlist_id = _playlist_item->i_id;           /* Playlist item specific id */
    p_input = _playlist_item->p_input;
    i_flags = _playlist_item->i_flags;
    input_item_Hold( p_input );
}

/*
   Constructors
   Call the above function init
   */
MCItem::MCItem( playlist_item_t *p_item, MCItem *p_parent )
{
    init( p_item, p_parent );
}

MCItem::MCItem( playlist_item_t * p_item )
{
    init( p_item, NULL );
}

MCItem::~MCItem()
{
    input_item_Release( p_input );
    qDeleteAll( plitem_children );
    plitem_children.clear();
}

int MCItem::id() const
{
    return i_playlist_id;
}

void MCItem::takeChildAt( int index )
{
    MCItem *child = plitem_children[index];
    child->parentItem = NULL;
    plitem_children.removeAt( index );
}

/* This function is used to get one's parent's row number in the model */
int MCItem::row()
{
    if( parentItem )
        return parentItem->indexOf( this );
    return 0;
}

bool MCItem::operator< ( MCItem& other )
{
    MCItem *item1 = this;
    while( item1->parentItem )
    {
        MCItem *item2 = &other;
        while( item2->parentItem )
        {
            if( item1 == item2->parentItem ) return true;
            if( item2 == item1->parentItem ) return false;
            if( item1->parentItem == item2->parentItem )
                return item1->parentItem->indexOf( item1 ) <
                       item1->parentItem->indexOf( item2 );
            item2 = item2->parentItem;
        }
        item1 = item1->parentItem;
    }
    return false;
}

QString MCItem::getURI() const
{
    QString uri;
    vlc_mutex_lock( &p_input->lock );
    uri = QString( p_input->psz_uri );
    vlc_mutex_unlock( &p_input->lock );
    return uri;
}

QString MCItem::getTitle() const
{
    QString title;
    char *fb_name = input_item_GetTitle( p_input );
    if( EMPTY_STR( fb_name ) )
    {
        free( fb_name );
        fb_name = input_item_GetName( p_input );
    }
    title = qfu(fb_name);
    free(fb_name);
    return title;
}

bool MCItem::readOnly() const
{
    return i_flags & PLAYLIST_RO_FLAG;
}
