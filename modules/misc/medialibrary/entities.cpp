/*****************************************************************************
 * entities.cpp: medialibrary C++ -> C entities conversion & management
 *****************************************************************************
 * Copyright © 2015-2016 VLC authors, VideoLAN and VideoLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "medialibrary.h"

#include <medialibrary/IMedia.h>
#include <medialibrary/IFile.h>
#include <medialibrary/IMovie.h>
#include <medialibrary/IShow.h>
#include <medialibrary/IShowEpisode.h>
#include <medialibrary/IArtist.h>
#include <medialibrary/IAlbum.h>
#include <medialibrary/IAlbumTrack.h>
#include <medialibrary/IGenre.h>
#include <medialibrary/ILabel.h>

void Release( ml_album_track_t& )
{
}

bool Convert( const medialibrary::IAlbumTrack* input, ml_album_track_t& output )
{
    output.i_artist_id = input->artistId();
    output.i_album_id = input->albumId();
    output.i_disc_nb = input->discNumber();
    output.i_genre_id = input->genreId();
    output.i_track_nb = input->trackNumber();
    return true;
}

bool Convert( const medialibrary::IShowEpisode* input, ml_show_episode_t& output )
{
    output.i_episode_nb = input->episodeNumber();
    output.i_season_number = input->seasonNumber();
    if ( input->name().empty() == false )
    {
        output.psz_name = strdup( input->name().c_str() );
        if ( unlikely( output.psz_name == nullptr ) )
            return false;
    }
    if ( input->artworkMrl().empty() == false )
    {
        output.psz_artwork_mrl = strdup( input->artworkMrl().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }
    if ( input->shortSummary().empty() == false )
    {
        output.psz_summary = strdup( input->shortSummary().c_str() );
        if ( unlikely( output.psz_summary == nullptr ) )
            return false;
    }
    if ( input->tvdbId().empty() == false )
    {
        output.psz_tvdb_id = strdup( input->tvdbId().c_str() );
        if ( unlikely( output.psz_tvdb_id == nullptr ) )
            return false;
    }
    return true;
}

void Release( ml_show_episode_t& episode )
{
    free( episode.psz_artwork_mrl );
    free( episode.psz_name );
    free( episode.psz_summary );
    free( episode.psz_tvdb_id );
}

bool Convert( const medialibrary::IMovie* input, ml_movie_t& output )
{
    output.psz_title = strdup( input->title().c_str() );
    if ( unlikely( output.psz_title == nullptr ) )
        return false;
    if ( input->imdbId().empty() == false )
    {
        output.psz_imdb_id = strdup( input->imdbId().c_str() );
        if ( unlikely( output.psz_imdb_id == nullptr ) )
            return false;
    }
    if ( input->artworkMrl().empty() == false )
    {
        output.psz_artwork_mrl = strdup( input->artworkMrl().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }
    if ( input->shortSummary().empty() == false )
    {
        output.psz_summary = strdup( input->shortSummary().c_str() );
        if ( unlikely( output.psz_summary == nullptr ) )
            return false;
    }
    return true;
}

void Release( ml_movie_t& movie )
{
    free( movie.psz_title );
    free( movie.psz_summary );
    free( movie.psz_artwork_mrl );
    free( movie.psz_imdb_id );
}

void Release( ml_media_t& media )
{
    ReleaseList( media.p_files );
    free( media.psz_title );
    free( media.psz_artwork_mrl );
    switch( media.i_subtype )
    {
        case ML_MEDIA_SUBTYPE_ALBUMTRACK:
            Release( media.album_track );
            break;
        case ML_MEDIA_SUBTYPE_SHOW_EPISODE:
            Release( media.show_episode );
            break;
        case ML_MEDIA_SUBTYPE_MOVIE:
            Release( media.movie );
            break;
        default:
            vlc_assert_unreachable();
    }
}

bool Convert( const medialibrary::IMedia* input, ml_media_t& output )
{
    output.pf_release = static_cast<void(*)(ml_media_t*)>( &ReleaseRef );
    output.i_id = input->id();

    switch ( input->type() )
    {
        case medialibrary::IMedia::Type::Audio:
            output.i_type = ML_MEDIA_TYPE_AUDIO;
            switch( input->subType() )
            {
                case medialibrary::IMedia::SubType::AlbumTrack:
                {
                    auto albumTrack = input->albumTrack();
                    if ( albumTrack == nullptr )
                        return false;
                    if ( Convert( albumTrack.get(), output.album_track ) == false )
                        return false;
                }
                default:
                    vlc_assert_unreachable();
            }
            break;
        case medialibrary::IMedia::Type::Video:
        {
            output.i_type = ML_MEDIA_TYPE_VIDEO;
            switch( input->subType() )
            {
                case medialibrary::IMedia::SubType::Movie:
                {
                    auto movie = input->movie();
                    if ( movie == nullptr )
                        return false;
                    if ( Convert( movie.get(), output.movie ) == false )
                        return false;
                    break;
                }
                case medialibrary::IMedia::SubType::ShowEpisode:
                {
                    auto episode = input->showEpisode();
                    if ( episode == nullptr )
                        return false;
                    if ( Convert( episode.get(), output.show_episode ) == false )
                        return false;
                    break;
                }
                case medialibrary::IMedia::SubType::AlbumTrack:
                case medialibrary::IMedia::SubType::Unknown:
                    vlc_assert_unreachable();
            }
            break;
        }
        case medialibrary::IMedia::Type::External:
            output.i_type = ML_MEDIA_TYPE_EXTERNAL;
            break;
        case medialibrary::IMedia::Type::Unknown:
            vlc_assert_unreachable();
    }
    output.i_year = input->releaseDate();
    output.i_duration = input->duration();
    output.b_is_favorite = input->isFavorite();
    output.i_playcount = input->playCount();

    output.psz_title = strdup( input->title().c_str() );
    if ( unlikely( output.psz_title == nullptr ) )
        return false;

    auto files = input->files();
    output.p_files = ml_convert_list<ml_file_list_t>( files );
    if ( output.p_files == nullptr )
        return false;

    if ( input->isThumbnailGenerated() == true )
    {
        output.psz_artwork_mrl = strdup( input->thumbnail().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }

    return true;
}

void Release( ml_file_t& file )
{
    free( file.psz_mrl );
}

bool Convert( const medialibrary::IFile* input, ml_file_t& output )
{
    output.pf_release = static_cast<void(*)(ml_file_t*)>( &ReleaseRef );
    switch ( input->type() )
    {
        case medialibrary::IFile::Type::Main:
            output.i_type = ML_FILE_TYPE_MAIN;
            break;
        case medialibrary::IFile::Type::Part:
            output.i_type = ML_FILE_TYPE_PART;
            break;
        case medialibrary::IFile::Type::Soundtrack:
            output.i_type = ML_FILE_TYPE_SOUNDTRACK;
            break;
        case medialibrary::IFile::Type::Subtitles:
            output.i_type = ML_FILE_TYPE_SUBTITLE;
            break;
        case medialibrary::IFile::Type::Playlist:
            output.i_type = ML_FILE_TYPE_PLAYLIST;
            break;
        default:
            vlc_assert_unreachable();
    }
    output.psz_mrl = strdup( input->mrl().c_str() );
    if ( unlikely( output.psz_mrl == nullptr ) )
        return false;
    output.b_external = input->isExternal();
    return true;
}

void Release( ml_album_t& album )
{
    free( album.psz_artist );
    free( album.psz_artwork_mrl );
    free( album.psz_summary );
    free( album.psz_title );
    ReleaseList( album.p_featuring );
}

bool Convert( const medialibrary::IAlbum* input, ml_album_t& output )
{
    output.pf_release = static_cast<void(*)(ml_album_t*)>( &ReleaseRef );
    output.i_id = input->id();
    auto featuring = input->artists( false )->all();
    output.p_featuring = ml_convert_list<ml_artist_list_t>( featuring );
    output.i_nb_tracks = input->nbTracks();
    output.i_duration = input->duration();
    output.i_year = input->releaseYear();
    if ( input->title().empty() == false )
    {
        output.psz_title = strdup( input->title().c_str() );
        if ( unlikely( output.psz_title == nullptr ) )
            return false;
    }
    if ( input->shortSummary().empty() == false )
    {
        output.psz_summary = strdup( input->shortSummary().c_str() );
        if ( unlikely( output.psz_summary == nullptr ) )
            return false;
    }
    if ( input->artworkMrl().empty() == false )
    {
        output.psz_artwork_mrl = strdup( input->artworkMrl().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }
    auto artist = input->albumArtist();
    if ( artist != nullptr )
    {
        output.i_artist_id = artist->id();
        switch ( artist->id() )
        {
            case medialibrary::UnknownArtistID:
                output.psz_artist = strdup( _( "Unknown Artist" ) );
                break;
            case medialibrary::VariousArtistID:
                output.psz_artist = strdup( _( "Various Artist" ) );
                break;
            default:
                output.psz_artist = strdup( artist->name().c_str() );
                break;
        }
        if ( unlikely( output.psz_artist == nullptr ) )
            return false;
    }
    return true;
}

void Release( ml_artist_t& artist  )
{
    free( artist.psz_artwork_mrl );
    free( artist.psz_name );
    free( artist.psz_shortbio );
    free( artist.psz_mb_id );
}

bool Convert( const medialibrary::IArtist* input, ml_artist_t& output )
{
    output.pf_release = static_cast<void(*)(ml_artist_t*)>( &ReleaseRef );
    output.i_id = input->id();
    output.i_nb_album = input->nbAlbums();
    output.i_nb_tracks = input->nbTracks();
    switch ( input->id() )
    {
        case medialibrary::UnknownArtistID:
            output.psz_name = strdup( _( "Unknown Artist" ) );
            break;
        case medialibrary::VariousArtistID:
            output.psz_name = strdup( _( "Various Artist" ) );
            break;
        default:
            output.psz_name = strdup( input->name().c_str() );
            break;
    }
    if ( unlikely( output.psz_name == nullptr ) )
        return false;
    if ( input->shortBio().empty() == false )
    {
        output.psz_shortbio = strdup( input->shortBio().c_str() );
        if ( unlikely( output.psz_shortbio == nullptr ) )
            return false;
    }
    if ( input->artworkMrl().empty() == false )
    {
        output.psz_artwork_mrl = strdup( input->artworkMrl().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }
    if ( input->musicBrainzId().empty() == false )
    {
        output.psz_mb_id = strdup( input->musicBrainzId().c_str() );
        if ( unlikely( output.psz_mb_id == nullptr ) )
            return false;
    }
    return true;
}

void Release( ml_genre_t& genre )
{
    free( genre.psz_name );
}

bool Convert( const medialibrary::IGenre* input, ml_genre_t& output )
{
    output.pf_release = static_cast<void(*)(ml_genre_t*)>( &ReleaseRef );
    output.i_id = input->id();
    output.i_nb_tracks = input->nbTracks();
    assert( input->name().empty() == false );
    output.psz_name = strdup( input->name().c_str() );
    if ( unlikely( output.psz_name == nullptr ) )
        return false;
    return true;
}

void Release( ml_show_t& show )
{
    free( show.psz_artwork_mrl );
    free( show.psz_name );
    free( show.psz_summary );
    free( show.psz_tvdb_id );
}

bool Convert( const medialibrary::IShow* input, ml_show_t& output )
{
    output.pf_release = static_cast<void(*)(ml_show_t*)>( &ReleaseRef );
    output.i_id = input->id();
    output.i_release_year = input->releaseDate();
    output.i_nb_episodes = input->nbEpisodes();
    output.i_nb_seasons = input->nbSeasons();
    if ( input->name().empty() == false )
    {
        output.psz_name = strdup( input->name().c_str() );
        if ( output.psz_name == nullptr )
            return false;
    }
    if ( input->artworkMrl().empty() == false )
    {
        output.psz_artwork_mrl = strdup( input->artworkMrl().c_str() );
        if ( unlikely( output.psz_artwork_mrl == nullptr ) )
            return false;
    }
    if ( input->tvdbId().empty() == false )
    {
        output.psz_tvdb_id = strdup( input->tvdbId().c_str() );
        if ( unlikely( output.psz_tvdb_id == nullptr ) )
            return false;
    }
    if ( input->shortSummary().empty() == false )
    {
        output.psz_summary = strdup( input->shortSummary().c_str() );
        if ( unlikely( output.psz_summary == nullptr ) )
            return false;
    }
    return true;
}

void Release( ml_label_t& label )
{
    free( label.psz_name );
}

bool Convert( const medialibrary::ILabel* input, ml_label_t& output )
{
    output.pf_release = static_cast<void(*)(ml_label_t*)>( &ReleaseRef );
    assert( input->name().empty() == false );
    output.psz_name = strdup( input->name().c_str() );
    if ( unlikely( output.psz_name == nullptr ) )
        return false;
    return true;
}
