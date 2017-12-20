/*****************************************************************************
 * mediacenter_model.hpp : Model for the mediacenter
 ****************************************************************************
 * Copyright (C) 2006-2011 the VideoLAN team
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jakob Leben <jleben@videolan.org>
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

#ifndef VLC_QT_PLAYLIST_MODEL_HPP
#define VLC_QT_PLAYLIST_MODEL_HPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_input.h>
#include <vlc_playlist.h>
#include "vlc_model.hpp"
#include "mediacenter_item.hpp"

#include <QObject>
#include <QEvent>
#include <QSignalMapper>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>
#include <QAction>

class MCItem;
class PlMimeData;

class MCModel : public VLCModel
{
    Q_OBJECT


public:
    MCModel( playlist_t *, intf_thread_t *,
             playlist_item_t *, QObject *parent = 0 );
    virtual ~MCModel();

    /* Qt main PLModel */
    static MCModel* getMCModel( intf_thread_t *p_intf )
    {
        if(!p_intf->p_sys->pl_model )
        {
            playlist_Lock( THEPL );
            playlist_item_t *p_root = THEPL->p_playing;
            playlist_Unlock( THEPL );
            p_intf->p_sys->pl_model = new MCModel( THEPL, p_intf, p_root, NULL );
        }

        return p_intf->p_sys->pl_model;
    }

    /*** QAbstractItemModel subclassing ***/

    /* Data structure */
    QVariant data( const QModelIndex &index, const int role ) const Q_DECL_OVERRIDE;
    bool setData( const QModelIndex &index, const QVariant & value, int role = Qt::EditRole ) Q_DECL_OVERRIDE;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags( const QModelIndex &index ) const Q_DECL_OVERRIDE;
    QModelIndex index( const int r, const int c, const QModelIndex &parent ) const Q_DECL_OVERRIDE;
    QModelIndex parent( const QModelIndex &index ) const Q_DECL_OVERRIDE;

    /* Drag and Drop */
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
    QMimeData* mimeData( const QModelIndexList &indexes ) const Q_DECL_OVERRIDE;
    virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action,
              int row, int column, const QModelIndex &target ) Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;

    /* Sort */
    void sort( const int column, Qt::SortOrder order = Qt::AscendingOrder ) Q_DECL_OVERRIDE;

    /*** VLCModelSubInterface subclassing ***/
    virtual void rebuild( playlist_item_t * p = NULL ) Q_DECL_OVERRIDE;
    virtual void doDelete( QModelIndexList selected ) Q_DECL_OVERRIDE;
    virtual void createNode( QModelIndex index, QString name ) Q_DECL_OVERRIDE;
    virtual void renameNode( QModelIndex index, QString name ) Q_DECL_OVERRIDE;
    virtual void removeAll() Q_DECL_OVERRIDE;

    /* Lookups */
    virtual QModelIndex rootIndex() const Q_DECL_OVERRIDE;
    virtual void filter( const QString& search_text, const QModelIndex & root, bool b_recursive ) Q_DECL_OVERRIDE;
    virtual QModelIndex currentIndex() const Q_DECL_OVERRIDE;
    virtual QModelIndex indexByPLID( const int i_plid, const int c ) const Q_DECL_OVERRIDE;
    virtual QModelIndex indexByInputItem( const input_item_t *, const int c ) const Q_DECL_OVERRIDE;
    virtual bool isTree() const Q_DECL_OVERRIDE;
    virtual bool canEdit() const Q_DECL_OVERRIDE;
    virtual bool action( QAction *action, const QModelIndexList &indexes ) Q_DECL_OVERRIDE;
    virtual bool isSupportedAction( actions action, const QModelIndex & ) const Q_DECL_OVERRIDE;

    Q_INVOKABLE virtual void activateItem(const QModelIndex &index ) Q_DECL_OVERRIDE;
    void activateItem( MCItem *item );
    void activateItem( playlist_item_t *p_item ) ;
    void exploreDir( MCItem *item );

protected:
    /* VLCModel subclassing */
    virtual bool isParent( const QModelIndex &index, const QModelIndex &current) const Q_DECL_OVERRIDE;
    virtual bool isLeaf( const QModelIndex &index ) const Q_DECL_OVERRIDE;
    virtual MCItem *getItem( const QModelIndex & index ) const Q_DECL_OVERRIDE;

private:
    /* General */
    MCItem *rootItem;

    playlist_t *p_playlist;

    /* Custom model private methods */
    /* Lookups */
    QModelIndex index( MCItem *, const int c ) const;

    /* Shallow actions (do not affect core playlist) */
    void updateTreeItem( MCItem * );
    void removeItem ( MCItem * );
    void recurseDelete( QList<MCItem*> children, QModelIndexList *fullList );
    void takeItem( MCItem * ); //will not delete item
    void insertChildren( MCItem *node, QList<MCItem*>& items, int i_pos );
    /* ...of which  the following will not update the views */
    void updateChildren( MCItem * );
    void updateChildren( playlist_item_t *, MCItem * );

    /* Deep actions (affect core playlist) */
    void dropAppendCopy( const PlMimeData * data, MCItem *target, int pos );
    void dropMove( const PlMimeData * data, MCItem *target, int new_pos );

    /* */
    void sort( QModelIndex caller, QModelIndex rootIndex, const int column, Qt::SortOrder order );

    /* Lookups */
    MCItem *findByPLId( MCItem *, int i_plitemid ) const;
    MCItem *findByInput( MCItem *, const input_item_t * ) const;
    MCItem *findByInputLocked( MCItem *, const input_item_t * ) const;
    enum pl_nodetype
    {
        ROOTTYPE_CURRENT_PLAYING,
        ROOTTYPE_MEDIA_LIBRARY,
        ROOTTYPE_OTHER
    };
    pl_nodetype getPLRootType() const;

    /* */
    QString latestSearch;
    QFont   customFont;

private slots:
    void processInputItemUpdate( input_item_t *);
    void processInputItemUpdate();
    void processItemRemoval( int i_pl_itemid );
    void processItemAppend( int i_pl_itemid, int i_pl_itemidparent );
};

class PlMimeData : public QMimeData
{
    Q_OBJECT

public:
    PlMimeData() {}
    virtual ~PlMimeData();
    void appendItem( input_item_t *p_item );
    QList<input_item_t*> inputItems() const;
    QStringList formats () const;

private:
    QList<input_item_t*> _inputItems;
    QMimeData *_mimeData;
};

#endif
