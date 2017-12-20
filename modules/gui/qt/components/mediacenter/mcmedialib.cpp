/*****************************************************************************
 * mcmedialib.cpp : Medialibrary object
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

#include "mcmedialib.hpp"

#include <vlc_playlist.h>
#include <vlc_input_item.h>

MCMediaLib::MCMediaLib(
        intf_thread_t *_intf,
        std::shared_ptr<PLModel> _pl_model,
        QObject *_parent
    ) : m_intf( _intf ),
        m_currentCat ( CAT_MUSIC_ALBUM ),
        m_oldCat ( CAT_MUSIC_ALBUM ),
        m_currentSort( medialibrary::SortingCriteria::Default ),
        m_isDesc( false ),
        m_isNightMode( false ),
        m_ml( NewMediaLibrary() ),
        m_cb( new medialibrary::ExCallback() ),
        m_gridView( true ),
        m_PLModel( _pl_model ),
        QObject( _parent )
{
    m_ml->initialize("/home/moamoak/vlc-bdd.db", "/home/moamoak/vlc-thumb", m_cb);
    m_ml->start();

    retrieveAlbums();
}

// Are we exploring a specific item or just browsing generic category
QVariant MCMediaLib::hasPresentation() {
    if (m_currentMainObj)
        return true;
    else
        return false;
}

// Remove presentation to get back to the list of items that were in place before
void MCMediaLib::backPresentation() {
    switch (m_oldCat)
    {
    case CAT_MUSIC_ALBUM:
    {
        m_currentCat = CAT_MUSIC_ALBUM;
        retrieveAlbums();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    case CAT_MUSIC_ARTIST:
    {
        m_currentCat = CAT_MUSIC_ARTIST;
        retrieveArtists();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    case CAT_MUSIC_GENRE:
    {
        m_currentCat = CAT_MUSIC_GENRE;
        retrieveGenres();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    case CAT_MUSIC_TRACKS:
    {
        m_currentCat = CAT_MUSIC_TRACKS;
        retrieveTracks();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    case CAT_VIDEO:
    {
        m_currentCat = CAT_VIDEO;
        retrieveMovies();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    case CAT_NETWORK:
    {
        m_currentCat = CAT_NETWORK;
        retrieveSeries();
        if (m_currentMainObj) m_currentMainObj = nullptr;
        break;
    }
    default:
        break;
    }
}

// Which category should be displayed
QVariant MCMediaLib::getCategory()
{
    return QVariant( m_currentCat );
}

// Get the list of items that should be displayed
QVariant MCMediaLib::getObjects()
{
    MLItemModel* obj =  new MLItemModel(m_currentObj);
    // Trick needed else the ownership is passed to QML and obj might be destroyed
    // cf. http://doc.qt.io/qt-5/qtqml-cppintegration-data.html#data-ownership
    QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
    return QVariant::fromValue<MLItemModel*>( obj );
}

// Should the items be displayed as a grid or as list ?
QVariant MCMediaLib::isGridView()
{
    return QVariant( m_gridView );
}

// Toogle between grid and list view for the displayed items
void MCMediaLib::toogleView()
{
    m_gridView = !m_gridView;
}

// Is the night mode activated
QVariant MCMediaLib::isNightMode()
{
    return QVariant( m_isNightMode );
}

// Toogle between nightmode and daymode
void MCMediaLib::toogleNightMode()
{
    m_isNightMode = !m_isNightMode;
}

// A specific item has been selected -> update the list of obejcts and the presentation
void MCMediaLib::select( const int &item_id )
{
    if (item_id >= 0 && item_id <= m_currentObj.count())
    {
        if (!m_currentMainObj)
            m_oldCat = m_currentCat;

        m_currentMainObj = m_currentObj.at(item_id);
        m_currentObj = m_currentMainObj->getDetailsObjects(m_currentSort, m_isDesc);

        switch (m_currentCat)
        {
        case CAT_MUSIC_ALBUM:
            m_currentCat = CAT_MUSIC_TRACKS;
            break;

        case CAT_MUSIC_ARTIST:
            m_currentCat = CAT_MUSIC_ALBUM;
            break;

        case CAT_MUSIC_GENRE:
            m_currentCat = CAT_MUSIC_ALBUM;
            break;

        default:
            break;
        }
    }

}

// A specific item has been asked to be added to the playlist
void MCMediaLib::addToPlaylist( const int &item_id )
{
    if (item_id >= 0 && item_id <= m_currentObj.count())
    {
        std::shared_ptr<MLItem> selected_item = m_currentObj.at(item_id);
        QList<MLAlbumTrack*> tracks = selected_item->getPLTracks();

        for (int i=0 ; i<tracks.size() ; i++)
        {
            std::shared_ptr<PLItem> pl_item = std::make_shared<PLItem>(tracks.at(i));
            m_PLModel->appendItem(pl_item, false);
        }
    }
}

// A specific sub-item has been asked to be added to the playlist
void MCMediaLib::addToPlaylist( const int &item_id, const int &subitem_id )
{
    if (item_id >= 0 && item_id <= m_currentObj.count())
    {
        std::shared_ptr<MLItem> selected_item = m_currentObj.at(item_id);
        QList<std::shared_ptr<MLItem>> subitems = selected_item->getDetailsObjects(m_currentSort, m_isDesc);
        if (subitem_id >= 0 && subitem_id <= subitems.count())
        {
            std::shared_ptr<MLItem> selected_subitem = subitems.at(subitem_id);
            QList<MLAlbumTrack*> tracks = selected_subitem->getPLTracks();

            for (int i=0 ; i<tracks.size() ; i++)
            {
                std::shared_ptr<PLItem> pl_item = std::make_shared<PLItem>(tracks.at(i));
                m_PLModel->appendItem(pl_item, false);
            }
        }
    }
}

// A specific item has been asked to be played,
// so it's added to the playlist and played
void MCMediaLib::addAndPlay( const int &item_id )
{
    if (item_id >= 0 && item_id <= m_currentObj.count())
    {
        std::shared_ptr<MLItem> selected_item = m_currentObj.at(item_id);
        QList<MLAlbumTrack*> tracks = selected_item->getPLTracks();

        for (int i=0 ; i<tracks.size() ; i++)
        {
            std::shared_ptr<PLItem> pl_item = std::make_shared<PLItem>(tracks.at(i));
            m_PLModel->appendItem(pl_item, i==0);
        }
    }
}

// A specific sub-item has been asked to be played,
// so it's added to the playlist and played
void MCMediaLib::addAndPlay( const int &item_id, const int &subitem_id )
{
    if (item_id >= 0 && item_id <= m_currentObj.count())
    {
        std::shared_ptr<MLItem> selected_item = m_currentObj.at(item_id);
        QList<std::shared_ptr<MLItem>> subitems = selected_item->getDetailsObjects(m_currentSort, m_isDesc);
        if (subitem_id >= 0 && subitem_id <= subitems.count())
        {
            std::shared_ptr<MLItem> selected_subitem = subitems.at(subitem_id);
            QList<MLAlbumTrack*> tracks = selected_subitem->getPLTracks();

            for (int i=0 ; i<tracks.size() ; i++)
            {
                std::shared_ptr<PLItem> pl_item = std::make_shared<PLItem>(tracks.at(i));
                m_PLModel->appendItem(pl_item, i==0);
            }
        }
    }
}

// The object that should be presented in the presentation banner
QVariant MCMediaLib::getPresObject()
{
    if (m_currentMainObj)
        return QVariant::fromValue( m_currentMainObj.get() );
    else
        return QVariant();
}

// When a source (or sub-source) is selected (Music / Music>Albums / Videos / ...)
void MCMediaLib::selectSource( const QString &name )
{
    if (name == "music" && m_currentCat != CAT_MUSIC_ALBUM)
    {
        msg_Dbg( m_intf, "Switching to music-general view");
        m_currentCat = CAT_MUSIC_ALBUM;
        retrieveAlbums();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "music-albums" && m_currentCat != CAT_MUSIC_ALBUM)
    {
        msg_Dbg( m_intf, "Switching to music-albums view");
        m_currentCat = CAT_MUSIC_ALBUM;
        retrieveAlbums();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "music-artists" && m_currentCat != CAT_MUSIC_ARTIST)
    {
        msg_Dbg( m_intf, "Switching to music-artists view");
        m_currentCat = CAT_MUSIC_ARTIST;
        retrieveArtists();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "music-genre" && m_currentCat != CAT_MUSIC_GENRE)
    {
        msg_Dbg( m_intf, "Switching to music-genre view");
        m_currentCat = CAT_MUSIC_GENRE;
        retrieveGenres();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "music-tracks" && m_currentCat != CAT_MUSIC_TRACKS)
    {
        msg_Dbg( m_intf, "Switching to music-track view");
        m_currentCat = CAT_MUSIC_TRACKS;
        retrieveTracks();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "video" && m_currentCat != CAT_VIDEO)
    {
        msg_Dbg( m_intf, "Switching to video-general view");
        m_currentCat = CAT_VIDEO;
        retrieveMovies();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
    else if (name == "network" && m_currentCat != CAT_NETWORK)
    {
        msg_Dbg( m_intf, "Switching to network-general view");
        m_currentCat = CAT_NETWORK;
        retrieveSeries();
        if (m_currentMainObj) m_currentMainObj = nullptr;
    }
}

// When a sort has been selected, we need to recalculate the model
void MCMediaLib::sort( const QString &criteria )
{
    if (criteria == "Alphabetic asc")
    {
        msg_Dbg( m_intf, "Sorting by ascending alphabetic order");
        m_currentSort = medialibrary::SortingCriteria::Alpha;
        m_isDesc = false;
    }
    else if (criteria == "Alphabetic desc")
    {
        msg_Dbg( m_intf, "Sorting by descending alphabetic order");
        m_currentSort = medialibrary::SortingCriteria::Alpha;
        m_isDesc = true;
    }
    else if (criteria == "Duration asc")
    {
        msg_Dbg( m_intf, "Sorting by ascending duration order");
        m_currentSort = medialibrary::SortingCriteria::Duration;
        m_isDesc = false;
    }
    else if (criteria == "Duration desc")
    {
        msg_Dbg( m_intf, "Sorting by descending duration order");
        m_currentSort = medialibrary::SortingCriteria::Duration;
        m_isDesc = true;
    }
    else if (criteria == "Date asc")
    {
        msg_Dbg( m_intf, "Sorting by ascending date order");
        m_currentSort = medialibrary::SortingCriteria::ReleaseDate;
        m_isDesc = false;
    }
    else if (criteria == "Date desc")
    {
        msg_Dbg( m_intf, "Sorting by descending date order");
        m_currentSort = medialibrary::SortingCriteria::ReleaseDate;
        m_isDesc = true;
    }
    else if (criteria == "Artist asc")
    {
        msg_Dbg( m_intf, "Sorting by ascending artist order");
        m_currentSort = medialibrary::SortingCriteria::Artist;
        m_isDesc = false;
    }
    else if (criteria == "Artist desc")
    {
        msg_Dbg( m_intf, "Sorting by descending artist order");
        m_currentSort = medialibrary::SortingCriteria::Artist;
        m_isDesc = false;
    }
    if (m_currentMainObj)
    {
        m_currentObj = m_currentMainObj->getDetailsObjects(m_currentSort, m_isDesc);
    }
    else
    {
        sortCurrent();
    }
}

// Recalculate the list of root objects that should be displayed according to the current category and sort
void MCMediaLib::sortCurrent()
{
    switch (m_currentCat)
    {
    case CAT_MUSIC_ALBUM:
        retrieveAlbums();
        break;
    case CAT_MUSIC_ARTIST:
        retrieveArtists();
        break;
    case CAT_MUSIC_GENRE:
        retrieveGenres();
        break;
    case CAT_MUSIC_TRACKS:
        retrieveTracks();
        break;
    case CAT_VIDEO:
        retrieveMovies();
        break;
    case CAT_NETWORK:
        retrieveSeries();
        break;
    default:
        break;
    }
}

// Retriever to fetch items in medialib : Recalculate a specific list of root objects
void MCMediaLib::retrieveAlbums()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
    std::vector<medialibrary::AlbumPtr> a = m_ml->albums(m_currentSort, m_isDesc);
    for ( int i=0 ; i<a.size() ; i++ )
    {
        std::shared_ptr<MLAlbum> item = std::make_shared<MLAlbum>( a[i] );
        m_currentObj.append( item );
    }
}

void MCMediaLib::retrieveArtists()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
    std::vector<medialibrary::ArtistPtr> a = m_ml->artists(m_currentSort, m_isDesc);
    for ( int i=0 ; i<a.size() ; i++ )
    {
        std::shared_ptr<MLArtist> item = std::make_shared<MLArtist>( a[i] );
        m_currentObj.append( item );
    }
}

void MCMediaLib::retrieveGenres()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
    std::vector<medialibrary::GenrePtr> g = m_ml->genres(m_currentSort, m_isDesc);
    for ( int i=0 ; i<g.size() ; i++ )
    {
        std::shared_ptr<MLGenre> item = std::make_shared<MLGenre>( g[i] );
        m_currentObj.append( item );
    }
}

void MCMediaLib::retrieveTracks()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
    std::vector<medialibrary::MediaPtr> t = m_ml->audioFiles(m_currentSort, m_isDesc);
    for ( int i=0 ; i<t.size() ; i++ )
    {
        std::shared_ptr<MLAlbumTrack> item = std::make_shared<MLAlbumTrack>( t[i] );
        m_currentObj.append( item );
    }
}

void MCMediaLib::retrieveMovies()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
/* NOT IMPLEMENTED YET IN API
 */
}

void MCMediaLib::retrieveSeries()
{
    m_currentObj = QList<std::shared_ptr<MLItem>>();
/* NOT IMPLEMENTED YET IN API
 */
}

