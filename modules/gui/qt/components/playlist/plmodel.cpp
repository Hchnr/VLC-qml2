/*****************************************************************************
 * plmodel.cpp : Playlist model
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

#include "plmodel.hpp"


#include <qt5/QtCore/QString>

#include <vlc_playlist.h>
#include <vlc_input_item.h>

PLModel::PLModel(intf_thread_t *_p_intf, QObject *parent)
    : QAbstractListModel(parent)
{
    p_intf = _p_intf;
    plitems = QList<std::shared_ptr<PLItem>>();
}

void PLModel::move(const int &from, const int &to)
{
    msg_Dbg(p_intf, "plop");
    msg_Dbg(p_intf, QString::number(from).toLatin1().data());
    msg_Dbg(p_intf, QString::number(to).toLatin1().data());

//    std::shared_ptr<PLItem> item = plitems.at(from);
//    removeItem(from);
//    playlist_InsertInputItemTree( THEPL,
//        item,
//        THEPL->p_media_library,
//        to,
//        false );

    vlc_playlist_locker locker(THEPL);

    playlist_item_t* item = playlist_ItemGetByInput(
        THEPL, plitems.at(from)->getInputItem()
    );
    // Need a full-model reset, else only the removed row will
    // be reloaded in views and if this items leaves a group with
    // only one element it can't be detected

    beginResetModel();
    {
        QList<input_item_t*> inputItems;
        inputItems.append(item->p_input);
        dropMove(inputItems, item->p_parent, -1);

        plitems.move(from, to);
    }
    endResetModel();

}

void PLModel::dropMove( QList<input_item_t*> inputItems, playlist_item_t *p_parent, int row)
{
    playlist_AssertLocked(THEPL);

    playlist_item_t **pp_items;
    pp_items = (playlist_item_t **)
               calloc( inputItems.count(), sizeof( playlist_item_t* ) );
    if ( !pp_items ) return;

    if( !p_parent || row > p_parent->i_children )
    {
        free( pp_items );
        return;
    }

    int new_pos = row == -1 ? p_parent->i_children : row;
    int i = 0;

    foreach( input_item_t *p_input, inputItems )
    {
        playlist_item_t *p_item = playlist_ItemGetByInput( THEPL, p_input );
        if( !p_item ) continue;

        /* Better not try to move a node into itself.
           Abort the whole operation in that case,
           because it is ambiguous. */
        playlist_item_t *climber = p_parent;
        while( climber )
        {
            if( climber->p_input == p_input )
            {
                free( pp_items );
                return;
            }
            climber = climber->p_parent;
        }

        pp_items[i] = p_item;
        i++;
    }
    playlist_TreeMoveMany( THEPL, i, pp_items, p_parent, new_pos );

    free( pp_items );
}

QVariant PLModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant (QString("This is a header"));
}

int PLModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return plitems.count();
}

QVariant PLModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case TITLE_ROLE :
    {
        std::shared_ptr<PLItem> item = getItem(index);

        if (item == nullptr) return QVariant();
        if ( ! item->getTitle().isEmpty() )
            return QVariant( item->getTitle() );
        else
            return QVariant( item->getName() );
    }
    case ALBUM_TITLE_ROLE :
    {
        std::shared_ptr<PLItem> item = getItem(index);

        if (item == nullptr) return QVariant();
        return QVariant( item->getAlbumTitle() );
    }
    case DURATION_ROLE :
    {
        std::shared_ptr<PLItem> item = getItem(index);

        if (item == nullptr) return QVariant();
        return QVariant( item->getDuration() );
    }
    case COVER_ROLE :
    {
        std::shared_ptr<PLItem> item = getItem(index);

        if (item == nullptr) return QVariant();
        return QVariant( item->getCover() );
    }
    case CURRENT_ROLE :
        return QVariant( false );
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PLModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::FontRole] = "font";
    roles[Qt::DisplayRole] = "display";
    roles[Qt::DecorationRole] = "decoration";
    roles[Qt::BackgroundRole] = "background";
    roles[TITLE_ROLE] = "title";
    roles[ALBUM_TITLE_ROLE] = "album_title";
    roles[DURATION_ROLE] = "duration";
    roles[COVER_ROLE] = "cover";
    roles[CURRENT_ROLE] = "current";
    return roles;
}

Qt::ItemFlags PLModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled;
}

/**** Add and remove items to the playlist ****/

void PLModel::removeItem( int index )
{
    if( index < 0 || index > rowCount() ) return;

    vlc_playlist_locker locker(THEPL);

    // Need a full-model reset, else only the removed row will
    // be reloaded in views and if this items leaves a group with
    // only one element it can't be detected
    beginResetModel();
    {
        // Doc at /src/playlist/tree.c L.92
        playlist_NodeDelete(
            THEPL,
            playlist_ItemGetByInput(
                THEPL,
                plitems.at(index)->getInputItem()
            )
        );

        plitems.removeAt(index);
    }
    endResetModel();
}

void PLModel::appendItem( std::shared_ptr<PLItem> item, bool play )
{
    if( !item ) return;

    // Need a full-model reset, else only the added row will
    // be reloaded in views and if this items forms a new group,
    // it can't be detected
    beginResetModel();
    {
        plitems.append( item );

        // Doc at /src/playlist/item.c L.488
        playlist_AddInput(THEPL, item->getInputItem(), play, true);
    }
    endResetModel();
}

/* Retrieve the item at the given index */
std::shared_ptr<PLItem> PLModel::getItem( const QModelIndex & index ) const
{
    int r = index.row();
    if (index.isValid() && r >= 0 && r < rowCount())
        return plitems.at(r);
    else
        return nullptr;
}

/**** Invokable functions ****/

/* Convenient function that create an object that can be passed to QML
 * and acts like a standard model item */
QVariantMap PLModel::get(int row) {
    QHash<int,QByteArray> names = roleNames();
    QHashIterator<int, QByteArray> i(names);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}

/* Play the item at index given */
void PLModel::play_item(int index)
{
    if (index < 0 || index >= rowCount()) return;
    plitems.at(index)->activate( THEPL );
}

/* Remove the item at index given from the playlist */
void PLModel::remove_item(int index)
{
    removeItem( index );
}



