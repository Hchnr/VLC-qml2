/*****************************************************************************
 * mcmedialib.hpp : Medialibrary object
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

#include <Qt>
#include <QAbstractListModel>
#include <QVariant>
#include <QHash>
#include <QByteArray>
#include <QList>
#include <QQuickWidget>
#include <QQuickItem>
#include <QMetaObject>
#include <QMetaMethod>
#include <QQmlEngine>

#include <memory>

#include "qt.hpp"
#include "mlalbum.hpp"
#include "mlmovie.hpp"
#include "mlserie.hpp"
#include "mlgenre.hpp"
#include "mlartist.hpp"
#include "components/utils/mlitemmodel.hpp"

#include "components/playlist/plitem.hpp"
#include "components/playlist/plmodel.hpp"

#include <medialibrary/IMediaLibrary.h>
#include <medialibrary/IAlbum.h>
#include <medialibrary/Types.h>
#include "excallback.hpp"

enum MCMediaLibRoles {
    GET_ID = Qt::UserRole + 1,
    GET_TITLE,
    GET_RELEASE_YEAR,
    GET_SHORT_SUMMARY,
    GET_COVER,
    GET_TRACKS,
    GET_ARTIST,
    GET_NB_TRACKS,
    GET_DURATION,
    GET_ALBUM,
    GET_NAME,
    GET_SHORT_BIO,
    GET_ALBUMS,
    GET_NB_ALBUMS
};

enum MCMediaLibCategory {
    CAT_MUSIC_ALBUM,
    CAT_MUSIC_ARTIST,
    CAT_MUSIC_GENRE,
    CAT_MUSIC_TRACKS,
    CAT_VIDEO,
    CAT_NETWORK
};

class MCMediaLib : public QObject
{
    Q_OBJECT

public:
    MCMediaLib(
        intf_thread_t* _intf,
        QQuickWidget* _qml_item,
        std::shared_ptr<PLModel> _pl_model,
        QObject* _parent = nullptr
    );

    Q_INVOKABLE QVariant hasPresentation();
    Q_INVOKABLE void backPresentation();
    Q_INVOKABLE QVariant getCategory();
    Q_INVOKABLE QVariant getObjects();
    Q_INVOKABLE QVariant isGridView();
    Q_INVOKABLE void toogleView();
    Q_INVOKABLE QVariant isNightMode();
    Q_INVOKABLE void toogleNightMode();
    Q_INVOKABLE void select(const int &item_id);
    Q_INVOKABLE void addToPlaylist(const int &item_id);
    Q_INVOKABLE void addToPlaylist(const int &item_id, const int &subitem_id);
    Q_INVOKABLE void addAndPlay(const int &item_id);
    Q_INVOKABLE void addAndPlay(const int &item_id, const int &subitem_id);
    Q_INVOKABLE QVariant getPresObject();
    Q_INVOKABLE void selectSource(const QString &name );
    Q_INVOKABLE void sort(const QString &criteria );

private:
    void retrieveAlbums();
    void retrieveArtists();
    void retrieveGenres();
    void retrieveTracks();
    void retrieveMovies();
    void retrieveSeries();

    intf_thread_t* m_intf;
    QQuickWidget *m_qmlItem;
    std::shared_ptr<PLModel> m_PLModel;

    bool m_gridView;

    MCMediaLibCategory m_currentCat;
    MCMediaLibCategory m_oldCat;
    medialibrary::SortingCriteria m_currentSort;
    bool m_isDesc;
    bool m_isNightMode;
    QList<std::shared_ptr<MLItem>> m_currentObj;
    std::shared_ptr<MLItem> m_currentMainObj;

    /* Medialibrary */
    medialibrary::IMediaLibrary* m_ml;
    medialibrary::ExCallback* m_cb;

    void sortCurrent();

    void invokeQML(const char *func );
};
