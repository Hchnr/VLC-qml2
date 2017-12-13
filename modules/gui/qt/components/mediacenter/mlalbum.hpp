/*****************************************************************************
 * mlalbum.hpp : Medialibrary's album
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
#include <medialibrary/IAlbum.h>
#include <medialibrary/IArtist.h>
#include <medialibrary/Types.h>

#include <memory>

#include "mlalbumtrack.hpp"
#include "mlitem.hpp"
#include "components/utils/mlitemmodel.hpp"

class MLAlbum : public MLItem
{
    Q_OBJECT

public:
    MLAlbum( medialibrary::AlbumPtr _data, QObject *_parent = nullptr);

    Q_INVOKABLE QString getId() const;
    Q_INVOKABLE QString getTitle() const;
    Q_INVOKABLE QString getReleaseYear() const;
    Q_INVOKABLE QString getShortSummary() const;
    Q_INVOKABLE QString getCover() const;
    Q_INVOKABLE MLItemModel* getTracks() const;
    Q_INVOKABLE QString getArtist() const;
    Q_INVOKABLE QList<QString> getArtists() const;
    Q_INVOKABLE QString getNbTracks() const;
    Q_INVOKABLE QString getDuration() const;

    Q_INVOKABLE QString getPresName() const;
    Q_INVOKABLE QString getPresImage() const;
    Q_INVOKABLE QString getPresInfo() const;
    Q_INVOKABLE QList<MLAlbumTrack *> getPLTracks() const;
    QList<std::shared_ptr<MLItem>> getDetailsObjects(medialibrary::SortingCriteria sort = medialibrary::SortingCriteria::Default, bool desc = false);

private:
    int64_t m_id;
    QString m_title;
    unsigned int m_releaseYear;
    QString m_shortSummary;
    QString m_cover;
    QList<std::shared_ptr<MLItem>> m_tracks;
    QString m_mainArtist;
    QList<QString> m_otherArtists;
    uint32_t m_nbTracks;
    unsigned int m_duration;

    medialibrary::AlbumPtr m_data;
};
