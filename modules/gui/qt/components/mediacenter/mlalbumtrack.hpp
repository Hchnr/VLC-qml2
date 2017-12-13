/*****************************************************************************
 * mlalbumtrack.hpp : Medialibrary's album track
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
#include <medialibrary/IMedia.h>
#include <medialibrary/IAlbumTrack.h>
#include <medialibrary/IAlbum.h>
#include <medialibrary/Types.h>

#include <memory>

#include "mlitem.hpp"
#include "components/utils/mlitemmodel.hpp"

class MLAlbumTrack : public MLItem
{
    Q_OBJECT

public:
    MLAlbumTrack( medialibrary::MediaPtr _data, QObject *_parent = nullptr);

    Q_INVOKABLE QString getTitle() const;
    Q_INVOKABLE QString getAlbumTitle() const;
    Q_INVOKABLE QString getCover() const;
    Q_INVOKABLE QString getTrackNumber() const;
    Q_INVOKABLE QString getDuration() const;
    Q_INVOKABLE QString getMRL() const;

    Q_INVOKABLE QString getPresName() const;
    Q_INVOKABLE QString getPresImage() const;
    Q_INVOKABLE QString getPresInfo() const;
    Q_INVOKABLE QList<MLAlbumTrack *> getPLTracks() const;
    QList<std::shared_ptr<MLItem>> getDetailsObjects(medialibrary::SortingCriteria sort = medialibrary::SortingCriteria::Default, bool desc = false);

private:
    QString m_title;
    QString m_albumTitle;
    QString m_cover;
    unsigned int m_trackNumber;
    int64_t m_duration;
    QString m_mrl;

    medialibrary::MediaPtr m_data;

signals:

public slots:
};
