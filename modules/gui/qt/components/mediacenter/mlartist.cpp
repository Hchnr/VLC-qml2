/*****************************************************************************
 * mlartist.cpp : Medialibrary's artist
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

#include "mlartist.hpp"

MLArtist::MLArtist(medialibrary::ArtistPtr _data, QObject *_parent) :
    m_id      ( _data->id() ),
    m_name    ( QString::fromStdString( _data->name() ) ),
    m_shortBio( QString::fromStdString( _data->shortBio() ) ),
    m_albums  ( QList<std::shared_ptr<MLItem>>() ),
    m_cover   ( QString::fromStdString( _data->artworkMrl() ) ),
    m_data    ( _data ),
    MLItem(_parent)
{
    // Fill m_albums
    std::vector<medialibrary::AlbumPtr> a = _data->albums();
    for (int i=0 ; i<a.size() ; i++)
        m_albums.append( std::make_shared<MLAlbum>( a[i]) );
}

QString MLArtist::getId() const
{
    return QString::number( m_id );
}

QString MLArtist::getName() const
{
    return m_name;
}

QString MLArtist::getShortBio() const
{
    return m_shortBio;
}

MLItemModel* MLArtist::getAlbums() const
{
    return new MLItemModel( m_albums );
}

QString MLArtist::getCover() const
{
    return m_cover;
}

QString MLArtist::getNbAlbums() const
{
    return QString::number( m_albums.count() );
}

QString MLArtist::getPresName() const
{
    return m_name;
}

QString MLArtist::getPresImage() const
{
    return m_cover;
}

QString MLArtist::getPresInfo() const
{
    return m_shortBio;
}

QList<MLAlbumTrack*> MLArtist::getPLTracks() const
{
    QList<MLAlbumTrack*> result;
    std::vector<medialibrary::MediaPtr> t = m_data->media();
    for (int i=0 ; i<t.size() ; i++ )
        result.append( new MLAlbumTrack( t[i] ) );
    return result;
}

QList<std::shared_ptr<MLItem>> MLArtist::getDetailsObjects(medialibrary::SortingCriteria sort, bool desc)
{
    QList<std::shared_ptr<MLItem>> result;
    medialibrary::QueryParameters queryparam{sort, desc};
    std::vector<medialibrary::AlbumPtr> t = m_data->albums(&queryparam)->all();
    for (int i=0 ; i<t.size() ; i++ )
        result.append( std::make_shared<MLAlbum>( t[i] ) );
    return result;
}
