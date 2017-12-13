/*****************************************************************************
 * mlgenre.cpp : Medialibrary's genre
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

#include "mlgenre.hpp"

MLGenre::MLGenre(medialibrary::GenrePtr _data, QObject *_parent ) :
    m_data    ( _data ),
    m_id      ( _data->id() ),
    m_name    ( QString::fromStdString( _data->name() ) ),
    m_nbTracks( _data->nbTracks() ),
    m_artists ( QList<std::shared_ptr<MLItem>>() ),
    m_tracks  ( QList<std::shared_ptr<MLItem>>() ),
    m_albums  ( QList<std::shared_ptr<MLItem>>() ),
    MLItem(_parent)
{
    // Fill m_artists
    std::vector<medialibrary::ArtistPtr> a = _data->artists();
    for (int i=0 ; i<a.size() ; i++ )
        m_artists.append( std::make_shared<MLArtist>( a[i] ) );
    // Fill m_tracks
    std::vector<medialibrary::MediaPtr> t = _data->tracks();
    for (int i=0 ; i<t.size() ; i++ )
        m_tracks.append( std::make_shared<MLAlbumTrack>( t[i] ) );
    // Fill m_albums
    std::vector<medialibrary::AlbumPtr> a2 = _data->albums();
    for (int i=0 ; i<a2.size() ; i++ )
        m_albums.append( std::make_shared<MLAlbum>( a2[i] ) );
}

QString MLGenre::getId() const
{
    return QString::number( m_id );
}

QString MLGenre::getName() const
{
    return m_name;
}

QString MLGenre::getNbTracks() const
{
    return QString::number( m_nbTracks );
}

MLItemModel* MLGenre::getArtists() const
{
    return new MLItemModel( m_artists );
}

MLItemModel* MLGenre::getTracks() const
{
    return new MLItemModel( m_tracks );
}

MLItemModel* MLGenre::getAlbums() const
{
    return new MLItemModel( m_albums );
}

QString MLGenre::getPresName() const
{
    return m_name;
}

QString MLGenre::getPresImage() const
{
    return QString();
}

QString MLGenre::getPresInfo() const
{
    return QString();
}

QList<MLAlbumTrack*> MLGenre::getPLTracks() const
{
    QList<MLAlbumTrack*> result;
    std::vector<medialibrary::MediaPtr> t = m_data->tracks();
    for (int i=0 ; i<t.size() ; i++ )
        result.append( new MLAlbumTrack( t[i] ) );
    return result;
}

QList<std::shared_ptr<MLItem>> MLGenre::getDetailsObjects(medialibrary::SortingCriteria sort, bool desc)
{
    QList<std::shared_ptr<MLItem>> result;
    std::vector<medialibrary::AlbumPtr> t = m_data->albums(sort, desc);
    for (int i=0 ; i<t.size() ; i++ )
        result.append( std::make_shared<MLAlbum>( t[i] ) );
    return result;
}

