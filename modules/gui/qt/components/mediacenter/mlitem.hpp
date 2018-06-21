/*****************************************************************************
 * mlitem.hpp : Medialibrary item
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

#include <qt5/QtCore/QObject>
#include <qt5/QtCore/QString>
#include <qt5/QtCore/QList>

#include <memory>

#include <medialibrary/IMediaLibrary.h>

class MLAlbumTrack;

class MLItem : public QObject
{
    Q_OBJECT
public:
    MLItem(QObject *_parent = nullptr);

    Q_INVOKABLE virtual QString getPresName() const = 0;
    Q_INVOKABLE virtual QString getPresImage() const = 0;
    Q_INVOKABLE virtual QString getPresInfo() const = 0;
    Q_INVOKABLE virtual QList<MLAlbumTrack *> getPLTracks() const = 0;
    virtual QList<std::shared_ptr<MLItem>> getDetailsObjects(medialibrary::SortingCriteria sort = medialibrary::SortingCriteria::Default, bool desc = false) = 0;
};
