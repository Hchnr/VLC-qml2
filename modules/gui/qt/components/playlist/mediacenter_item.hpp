/*****************************************************************************
 * medicenter_item.hpp : Item for the mediacenter
 ****************************************************************************
 * Copyright (C) 2006-2011 the VideoLAN team
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
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

#ifndef VLC_QT_PLAYLIST_ITEM_HPP_
#define VLC_QT_PLAYLIST_ITEM_HPP_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qt.hpp"

#include <QList>
#include <QString>

class MCItem
{
    friend class MLItem; /* super ugly glue stuff */
    friend class VLCModel;
    friend class MCModel;
    friend class MLModel;

public:
    virtual ~MCItem();
    bool hasSameParent( MCItem *other ) { return parent() == other->parent(); }
    bool operator< ( MCItem& );

protected:
    int id() const;
    int childCount() const { return children.count(); }
    int indexOf( MCItem *item ) const { return children.indexOf( item ); }
    int lastIndexOf( MCItem *item ) const { return children.lastIndexOf( item ); }
    MCItem *parent() { return parentItem; }
    input_item_t *inputItem() { return p_input; }
    void insertChild( MCItem *item, int pos = -1 ) { children.insert( pos, item ); }
    void appendChild( MCItem *item ) { insertChild( item, children.count() ); }
    MCItem *child( int id ) const { return children.value( id ); }
    void removeChild( MCItem *item );
    void clearChildren();
    QString getURI() const;
    QString getTitle() const;
    bool readOnly() const;

    QList<MCItem *> children;
    MCItem *parentItem;

private:
    MCItem( playlist_item_t *, MCItem *parent );
    int row();
    void takeChildAt( int );

    MCItem( playlist_item_t * );
    void init( playlist_item_t *, MCItem * );
    int i_playlist_id;
    int i_flags;
    input_item_t *p_input;
};

#endif

