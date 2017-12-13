/*****************************************************************************
 * mlitemmodel.hpp : Model for medialibrary item
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

#ifndef MLITEMMODEL_HPP
#define MLITEMMODEL_HPP

#include <qt5/QtCore/QAbstractListModel>
#include <qt5/QtCore/QVariant>
#include <qt5/QtCore/QHash>
#include <qt5/QtCore/QByteArray>
#include <qt5/QtCore/QObject>
#include <qt5/QtCore/QList>
#include <qt5/QtCore/QModelIndex>
#include <qt5/QtCore/Qt>

#include <memory>

class MLItem;
class MLAlbum;
class MLArtist;
class MLGenre;

enum MLItemModelRoles {

    // Albums
    GET_ALBUM_ID = Qt::UserRole + 1,
    GET_ALBUM_TITLE,
    GET_ALBUM_RELEASE_YEAR,
    GET_ALBUM_SHORT_SUMMARY,
    GET_ALBUM_COVER,
    GET_ALBUM_TRACKS,
    GET_ALBUM_MAIN_ARTIST,
    GET_ALBUM_ARTISTS,
    GET_ALBUM_NB_TRACKS,
    GET_ALBUM_DURATION,

    // Artists
    GET_ARTIST_ID,
    GET_ARTIST_NAME,
    GET_ARTIST_SHORT_BIO,
    GET_ARTIST_ALBUMS,
    GET_ARTIST_COVER,
    GET_ARTIST_NB_ALBUMS,

    // Genres
    GET_GENRE_ID,
    GET_GENRE_NAME,
    GET_GENRE_NB_TRACKS,
    GET_GENRE_ARTISTS,
    GET_GENRE_TRACKS,
    GET_GENRE_ALBUMS,

    // Tracks
    GET_TRACK_TITLE,
    GET_TRACK_COVER,
    GET_TRACK_NUMBER,
    GET_TRACK_DURATION
};

class MLItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MLItemModel(const QList<std::shared_ptr<MLItem>> item, QObject *parent = nullptr);
    ~MLItemModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariantMap get(int row);

private:
    std::shared_ptr<MLItem> getItem(const QModelIndex &index) const;

    QList<std::shared_ptr<MLItem>> ml_item_list;
};

#endif // MLITEMMODEL_HPP
