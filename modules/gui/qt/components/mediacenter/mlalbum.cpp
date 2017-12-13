/*****************************************************************************
 * mlalbum.hcp : Medialibrary's album
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

#include "mlalbum.hpp"

MLAlbum::MLAlbum(medialibrary::AlbumPtr _data , QObject *_parent) :
    m_id          ( _data->id() ),
    m_title       ( QString::fromStdString( _data->title() ) ),
    m_releaseYear ( _data->releaseYear() ),
    m_shortSummary( QString::fromStdString( _data->shortSummary() ) ),
    m_cover       ( QString::fromStdString( _data->artworkMrl() ) ),
    m_tracks      ( QList<std::shared_ptr<MLItem>>() ),
    m_mainArtist  ( QString::fromStdString( _data->albumArtist()->name() ) ),
    m_otherArtists( QList<QString>() ),
    m_nbTracks    ( _data->nbTracks() ),
    m_duration    ( _data->duration() ),
    m_data        ( _data ),
    MLItem( _parent )
{
    // Fill m_tracks
    std::vector<medialibrary::MediaPtr> t = _data->tracks();
    for (int i=0 ; i<t.size() ; i++ )
        m_tracks.append( std::make_shared<MLAlbumTrack>( t[i] ) );
    // Fill m_otherArtists
    std::vector<medialibrary::ArtistPtr> a = _data->artists( false );
    for (int i=0 ; i<a.size() ; i++ )
        m_otherArtists.append( QString::fromStdString( a[i]->name() ) );
}

QString MLAlbum::getId() const
{
    return QString::number( m_id );
}

QString MLAlbum::getTitle() const
{
    return m_title;
}

QString MLAlbum::getReleaseYear() const
{
    return QString::number( m_releaseYear );
}

QString MLAlbum::getShortSummary() const
{
    return m_shortSummary;
}

QString MLAlbum::getCover() const
{
    return m_cover;
}

MLItemModel* MLAlbum::getTracks() const
{
    return new MLItemModel( m_tracks );
}

QString MLAlbum::getArtist() const
{
    return m_mainArtist;
}

QList<QString> MLAlbum::getArtists() const
{
    return m_otherArtists;
}

QString MLAlbum::getNbTracks() const
{
    return QString::number( m_nbTracks );
}

QString MLAlbum::getDuration() const
{
    unsigned int sec = m_duration / 1000;
    unsigned int min = sec / 60;
    unsigned int hour = min / 60;
    std::string sec_str = std::to_string( sec - min * 60 );
    std::string min_str = std::to_string( min - hour * 60 );
    std::string hour_str = std::to_string( hour );
    QString sec_disp = QString::fromStdString(
        std::string(2-sec_str.length(), '0').append(sec_str)
    );
    QString min_disp = QString::fromStdString(
        std::string(2-min_str.length(), '0').append(min_str)
    );
    QString hour_disp = QString::fromStdString(
        std::string(2-hour_str.length(), '0').append(hour_str)
    );

    if ( hour > 0 )
        return hour_disp + ":" + min_disp + ":" + sec_disp;
    else
        return min_disp + ":" + sec_disp;
}

QString MLAlbum::getPresName() const
{
    return m_title;
}

QString MLAlbum::getPresImage() const
{
    return m_cover;
}

QString MLAlbum::getPresInfo() const
{
    return m_shortSummary;
}

QList<MLAlbumTrack*> MLAlbum::getPLTracks() const
{
    QList<MLAlbumTrack*> result;
    std::vector<medialibrary::MediaPtr> t = m_data->tracks();
    for (int i=0 ; i<t.size() ; i++ )
        result.append( new MLAlbumTrack( t[i] ) );
    return result;
}

QList<std::shared_ptr<MLItem>> MLAlbum::getDetailsObjects(medialibrary::SortingCriteria sort, bool desc)
{
    QList<std::shared_ptr<MLItem>> result;
    medialibrary::QueryParameters param { sort, desc };
    std::vector<medialibrary::MediaPtr> t = m_data->tracks(&param)->all();
    for (int i=0 ; i<t.size() ; i++ )
        result.append( std::make_shared<MLAlbumTrack>( t[i] ) );
    return result;
}
