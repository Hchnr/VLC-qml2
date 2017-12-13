/*****************************************************************************
 * plmodel.hpp : Playlist model
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

#ifndef PLMODEL_H
#define PLMODEL_H

#include <qt5/QtCore/Qt>
#include <qt5/QtCore/QAbstractListModel>
#include <qt5/QtCore/QVariant>
#include <qt5/QtCore/QHash>
#include <qt5/QtCore/QByteArray>
#include <qt5/QtCore/QList>

#include <memory>

#include "qt.hpp"
#include "input_manager.hpp"
#include "plitem.hpp"

enum PLModelRoles {
    TITLE_ROLE = Qt::UserRole + 1,
    ALBUM_TITLE_ROLE,
    DURATION_ROLE,
    COVER_ROLE,
    CURRENT_ROLE
};

class PLModel : public QAbstractListModel
{
    Q_OBJECT

public:
    PLModel(intf_thread_t *_p_intf, QObject *parent = nullptr);

    /* Subclassing QAbstractListModel */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /* Handling the playlist items */
    void removeItem(int index);
    void appendItem(std::shared_ptr<PLItem> item, bool play);

    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE void remove_item(int index);
    Q_INVOKABLE void play_item(int index);

private:
    std::shared_ptr<PLItem> getItem(const QModelIndex &index ) const;

    intf_thread_t *p_intf;
    QList<std::shared_ptr<PLItem>> plitems;

};

#endif // PLMODEL_H
