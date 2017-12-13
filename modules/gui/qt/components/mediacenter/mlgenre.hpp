/*****************************************************************************
 * mlgenre.hpp : Medialibrary's genre
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

#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <medialibrary/IGenre.h>
#include <medialibrary/IArtist.h>
#include <medialibrary/IAlbum.h>
#include <medialibrary/IAlbumTrack.h>
#include <medialibrary/Types.h>

#include <memory>

#include "mlartist.hpp"
#include "mlalbum.hpp"
#include "mlalbumtrack.hpp"
#include "mlitem.hpp"
#include "components/utils/mlitemmodel.hpp"

class MLGenre : public MLItem
{
    Q_OBJECT

public:
    MLGenre( medialibrary::GenrePtr _data, QObject *_parent = nullptr);

    Q_INVOKABLE QString getId() const;
    Q_INVOKABLE QString getName() const;
    Q_INVOKABLE QString getNbTracks() const;
    Q_INVOKABLE MLItemModel* getArtists() const;
    Q_INVOKABLE MLItemModel* getTracks() const;
    Q_INVOKABLE MLItemModel* getAlbums() const;

    Q_INVOKABLE QString getPresName() const;
    Q_INVOKABLE QString getPresImage() const;
    Q_INVOKABLE QString getPresInfo() const;
    Q_INVOKABLE QList<MLAlbumTrack *> getPLTracks() const;
    QList<std::shared_ptr<MLItem>> getDetailsObjects(medialibrary::SortingCriteria sort = medialibrary::SortingCriteria::Default, bool desc = false);

private:
    int64_t m_id;
    QString m_name;
    uint32_t m_nbTracks;
    QList<std::shared_ptr<MLItem>> m_artists;
    QList<std::shared_ptr<MLItem>> m_tracks;
    QList<std::shared_ptr<MLItem>> m_albums;

    medialibrary::GenrePtr m_data;

};
