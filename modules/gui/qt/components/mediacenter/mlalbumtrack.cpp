/*****************************************************************************
 * mlalbumtrack.cpp : Medialibrary's album track
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

#include "mlalbumtrack.hpp"

MLAlbumTrack::MLAlbumTrack(medialibrary::MediaPtr _data, QObject *_parent ):
    m_data       ( _data ),
    m_title      ( QString::fromStdString( _data->title() ) ),
    m_trackNumber( _data->albumTrack()->trackNumber() ),
    m_duration   ( _data->duration() ),
    MLItem( _parent )
{
    // Only select album-related info if it's an album track
    if (_data->subType() == medialibrary::IMedia::SubType::AlbumTrack)
    {
        m_albumTitle = QString::fromStdString( _data->albumTrack()->album()->title() );
        m_cover = QString::fromStdString( _data->albumTrack()->album()->artworkMrl() );
    }
    // Look for a file of type 'Main' (this is the file to play)
    const std::vector<medialibrary::FilePtr>& files = _data->files();
    int i=0;
    while( i<files.size() && files[i]->type() == medialibrary::IFile::Type::Main )
        i++;
    if ( i >= files.size() )
        m_mrl = QString::fromStdString( files[0]->mrl() );
    else
        m_mrl = QString::fromStdString( files[i]->mrl() );
}

QString MLAlbumTrack::getTitle() const
{
    return m_title;
}

QString MLAlbumTrack::getAlbumTitle() const
{
    return m_albumTitle;
}

QString MLAlbumTrack::getCover() const
{
    return m_cover;
}

QString MLAlbumTrack::getTrackNumber() const
{
    return QString::number( m_trackNumber );
}

QString MLAlbumTrack::getDuration() const
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

QString MLAlbumTrack::getMRL() const
{
    return m_mrl;
}

QString MLAlbumTrack::getPresName() const
{
    return m_title;
}

QString MLAlbumTrack::getPresImage() const
{
    return "qrc:///noart.png";
}

QString MLAlbumTrack::getPresInfo() const
{
    return "";
}

QList<MLAlbumTrack*> MLAlbumTrack::getPLTracks() const
{
    QList<MLAlbumTrack*> result;
    result.append( new MLAlbumTrack(m_data, this->parent() ) );
    return result;
}

QList<std::shared_ptr<MLItem>> MLAlbumTrack::getDetailsObjects(medialibrary::SortingCriteria sort, bool desc)
{
    return QList<std::shared_ptr<MLItem>>();
}
