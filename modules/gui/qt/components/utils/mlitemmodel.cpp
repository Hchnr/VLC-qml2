/*****************************************************************************
 * mlitemmodel.cpp : Model for medialibrary item
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

#include "mlitemmodel.hpp"

#include "components/mediacenter/mlitem.hpp"


MLItemModel::MLItemModel(const QList<std::shared_ptr<MLItem>> item, QObject *parent):
    QAbstractListModel( parent )
{
    ml_item_list = QList<std::shared_ptr<MLItem>>();
    for (int i=0 ; i<item.count() ; i++)
        ml_item_list.append(item.at(i));
}

MLItemModel::~MLItemModel()
{ }

int MLItemModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return ml_item_list.count();
}

QVariant MLItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    std::shared_ptr<MLItem> ml_item = getItem(index);
    switch (role)
    {
    default :
        return QVariant();
    }

}

QHash<int, QByteArray> MLItemModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    return roles;
}

QVariantMap MLItemModel::get(int row) {
    QHash<int,QByteArray> names = roleNames();
    QList<int> keys = names.keys();
    QVariantMap res;
    for (int i=0; i<keys.count(); i++)
    {
        int k = keys[i];
        QByteArray v = names.value(k);
        res[v] = index(row, 0).data(k);
    }
    return res;
}

std::shared_ptr<MLItem> MLItemModel::getItem(const QModelIndex &index) const
{
    int r = index.row();
    if (index.isValid())
        return ml_item_list.at(r);
    else
        return NULL;
}
