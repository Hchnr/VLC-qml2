/*****************************************************************************
 * medialib.cpp: medialibrary module
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

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_media_library.h>
#include "medialibrary.h"

#include <medialibrary/IFolder.h>
#include <medialibrary/IMedia.h>
#include <medialibrary/IAlbumTrack.h>
#include <medialibrary/IAlbum.h>
#include <medialibrary/IArtist.h>
#include <medialibrary/IGenre.h>

class Logger : public medialibrary::ILogger
{
public:
    Logger( vlc_object_t* obj ) : m_obj( obj ) {}

private:
    virtual void Error( const std::string& msg ) override
    {
        msg_Err( m_obj, "%s", msg.c_str() );
    }
    virtual void Warning( const std::string& msg ) override
    {
        msg_Warn( m_obj, "%s", msg.c_str() );
    }
    virtual void Info( const std::string& msg ) override
    {
        msg_Dbg( m_obj, "%s", msg.c_str() );
    }
    virtual void Debug( const std::string& msg ) override
    {
        msg_Dbg( m_obj, "%s", msg.c_str() );
    }

private:
    vlc_object_t* m_obj;
};

void MediaLibrary::onMediaAdded( std::vector<medialibrary::MediaPtr> )
{
}

void MediaLibrary::onMediaUpdated( std::vector<medialibrary::MediaPtr> )
{
}

void MediaLibrary::onMediaDeleted( std::vector<int64_t> )
{
}

void MediaLibrary::onArtistsAdded( std::vector<medialibrary::ArtistPtr> )
{
}

void MediaLibrary::onArtistsModified( std::vector<medialibrary::ArtistPtr> )
{
}

void MediaLibrary::onArtistsDeleted( std::vector<int64_t> )
{
}

void MediaLibrary::onAlbumsAdded( std::vector<medialibrary::AlbumPtr> )
{
}

void MediaLibrary::onAlbumsModified( std::vector<medialibrary::AlbumPtr> )
{
}

void MediaLibrary::onAlbumsDeleted( std::vector<int64_t> )
{
}

void MediaLibrary::onTracksAdded( std::vector<medialibrary::AlbumTrackPtr> )
{
}

void MediaLibrary::onTracksDeleted( std::vector<int64_t> )
{
}

void MediaLibrary::onPlaylistsAdded( std::vector<medialibrary::PlaylistPtr> )
{
}

void MediaLibrary::onPlaylistsModified( std::vector<medialibrary::PlaylistPtr> )
{
}

void MediaLibrary::onPlaylistsDeleted( std::vector<int64_t> )
{
}

void MediaLibrary::onDiscoveryStarted( const std::string& )
{
}

void MediaLibrary::onDiscoveryProgress( const std::string& )
{
}

void MediaLibrary::onDiscoveryCompleted( const std::string& )
{
}

void MediaLibrary::onReloadStarted( const std::string& )
{
}

void MediaLibrary::onReloadCompleted( const std::string& )
{
}

void MediaLibrary::onEntryPointRemoved( const std::string&, bool )
{
}

void MediaLibrary::onEntryPointBanned( const std::string&, bool )
{
}

void MediaLibrary::onEntryPointUnbanned( const std::string&, bool )
{
}

void MediaLibrary::onParsingStatsUpdated( uint32_t )
{
}

void MediaLibrary::onBackgroundTasksIdleChanged( bool )
{
}

void MediaLibrary::onMediaThumbnailReady( medialibrary::MediaPtr, bool )
{
}

MediaLibrary::MediaLibrary( vlc_object_t* obj )
    : m_logger( new Logger( obj ) )
    , m_ml( NewMediaLibrary() )
{
    m_ml->setVerbosity( medialibrary::LogLevel::Info );
    m_ml->setLogger( m_logger.get() );
    auto userDir = wrapCPtr( config_GetUserDir( VLC_USERDATA_DIR ) );
    std::string mlDir = std::string{ userDir.get() } + "/ml/";

    //tmp:
    unlink( (mlDir + "ml.db").c_str() );

    auto initStatus = m_ml->initialize( mlDir + "ml.db", mlDir + "thumbnails/", this );
    switch ( initStatus )
    {
        case medialibrary::InitializeResult::AlreadyInitialized:
            throw std::runtime_error( "Unexpected double medialibrary intialization" );
        case medialibrary::InitializeResult::Failed:
            throw std::runtime_error( "Medialibrary failed to initialize" );
        case medialibrary::InitializeResult::DbReset:
            msg_Info( obj, "FIXME: Handle database reset" );
            break;
        case medialibrary::InitializeResult::Success:
            msg_Dbg( obj, "MediaLibrary successfully initialized" );
            break;
    }

    m_ml->addParserService( std::make_shared<MetadataExtractor>( obj ) );
    auto res = m_ml->start();
    if ( res == false )
        throw std::runtime_error( "Failed to start medialibrary" );
    m_ml->discover( "file:///home/chouquette/dev/medialibrary/test/samples/samples/playlist/same_folder/" );
    m_ml->reload();
}

int MediaLibrary::Control( int query, va_list args )
{
    switch ( query )
    {
        case ML_ADD_FOLDER:
        case ML_REMOVE_FOLDER:
        case ML_BAN_FOLDER:
        case ML_UNBAN_FOLDER:
        {
            const char* mrl = va_arg( args, const char* );
            switch( query )
            {
                case ML_ADD_FOLDER:
                    m_ml->discover( mrl );
                    break;
                case ML_REMOVE_FOLDER:
                    m_ml->removeEntryPoint( mrl );
                    break;
                case ML_BAN_FOLDER:
                    m_ml->banFolder( mrl );
                    break;
                case ML_UNBAN_FOLDER:
                    m_ml->unbanFolder( mrl );
                    break;
            }
            break;
        }
        case ML_LIST_FOLDERS:
        {
            auto entryPoints = m_ml->entryPoints()->all();
            auto nbItem = entryPoints.size();
            auto list = wrapCArray( reinterpret_cast<ml_entrypoint_t*>(
                    calloc( entryPoints.size(), sizeof( ml_entrypoint_t ) ) ),
                    [nbItem]( ml_entrypoint_t* ptr ) {
                        vlc_ml_entrypoints_release( ptr, nbItem );
                    });
            if ( unlikely( list == nullptr ) )
                return VLC_ENOMEM;
            for ( auto i = 0u; i < entryPoints.size(); ++i )
            {
                const auto ep = entryPoints[i].get();
                if ( ep->isPresent() == true )
                {
                    list[i].psz_mrl = strdup( ep->mrl().c_str() );
                    if ( unlikely( list[i].psz_mrl == nullptr ) )
                        return VLC_ENOMEM;
                    list[i].b_present = true;
                }
                else
                {
                    list[i].psz_mrl = nullptr;
                    list[i].b_present = false;
                }
                list[i].b_banned = ep->isBanned();
            }
            *(va_arg( args, ml_entrypoint_t**) ) = list.release();
            *(va_arg( args, size_t*) ) = entryPoints.size();
            break;
        }
        case ML_PAUSE_BACKGROUND:
            m_ml->pauseBackgroundOperations();
            break;
        case ML_RESUME_BACKGROUND:
            m_ml->resumeBackgroundOperations();
            break;
        case ML_CLEAR_HISTORY:
            m_ml->clearHistory();
            break;
        case ML_MEDIA_INCREASE_PLAY_COUNT:
        {
            auto mediaId = va_arg( args, int64_t );
            auto m = m_ml->media( mediaId );
            if ( m == nullptr )
                return VLC_EGENERIC;
            if ( m->increasePlayCount() == false )
                return VLC_EGENERIC;
            break;
        }
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}

int MediaLibrary::List( int listQuery, const ml_query_params_t* params, va_list args )
{
    medialibrary::QueryParameters p{};
    medialibrary::QueryParameters* paramsPtr = nullptr;
    uint32_t nbItems = 0;
    uint32_t offset = 0;
    if ( params )
    {
        p.desc = params->b_desc;
        switch ( params->i_sort )
        {
            case ML_SORTING_DEFAULT:
                p.sort = medialibrary::SortingCriteria::Default;
                break;
            case ML_SORTING_ALPHA:
                p.sort = medialibrary::SortingCriteria::Alpha;
                break;
            case ML_SORTING_DURATION:
                p.sort = medialibrary::SortingCriteria::Duration;
                break;
            case ML_SORTING_INSERTIONDATE:
                p.sort = medialibrary::SortingCriteria::InsertionDate;
                break;
            case ML_SORTING_LASTMODIFICATIONDATE:
                p.sort = medialibrary::SortingCriteria::LastModificationDate;
                break;
            case ML_SORTING_RELEASEDATE:
                p.sort = medialibrary::SortingCriteria::ReleaseDate;
                break;
            case ML_SORTING_FILESIZE:
                p.sort = medialibrary::SortingCriteria::FileSize;
                break;
            case ML_SORTING_ARTIST:
                p.sort = medialibrary::SortingCriteria::Artist;
                break;
            case ML_SORTING_PLAYCOUNT:
                p.sort = medialibrary::SortingCriteria::PlayCount;
                break;
            case ML_SORTING_ALBUM:
                p.sort = medialibrary::SortingCriteria::Album;
                break;
            case ML_SORTING_FILENAME:
                p.sort = medialibrary::SortingCriteria::Filename;
                break;
            case ML_SORTING_TRACKNUMBER:
                p.sort = medialibrary::SortingCriteria::TrackNumber;
                break;
        }
        nbItems = params->i_nbResults;
        offset = params->i_offset;
        paramsPtr = &p;
    }
    switch( listQuery )
    {
        case ML_LIST_ALBUM_TRACKS:
        case ML_COUNT_ALBUM_TRACKS:
        {
            auto album = m_ml->album( va_arg( args, int64_t ) );
            if ( album == nullptr )
                return VLC_EGENERIC;
            auto query = album->tracks( paramsPtr );
            switch ( listQuery )
            {
                case ML_LIST_ALBUM_TRACKS:
                    *va_arg( args, ml_media_list_t**) = ml_convert_list<ml_media_list_t>(
                                query->items( nbItems, offset ) );
                    break;
                case ML_COUNT_ALBUM_TRACKS:
                    *va_arg( args, size_t* ) = query->count();
                    break;
                default:
                    vlc_assert_unreachable();
            }
            break;
        }
        case ML_LIST_ARTIST_ALBUMS:
        case ML_COUNT_ARTIST_ALBUMS:
        case ML_LIST_ARTIST_TRACKS:
        case ML_COUNT_ARTIST_TRACKS:
        {
            auto artist = m_ml->artist( va_arg( args, int64_t ) );
            if ( artist == nullptr )
                return VLC_EGENERIC;
            switch( listQuery )
            {
                case ML_LIST_ARTIST_ALBUMS:
                case ML_COUNT_ARTIST_ALBUMS:
                {
                    auto query = artist->albums( paramsPtr );
                    switch ( listQuery )
                    {
                        case ML_LIST_ARTIST_ALBUMS:
                            *va_arg( args, ml_album_list_t**) = ml_convert_list<ml_album_list_t>(
                                        query->items( nbItems, offset ) );
                            break;
                        case ML_COUNT_ARTIST_ALBUMS:
                            *va_arg( args, size_t* ) = query->count();
                            break;
                        default:
                            vlc_assert_unreachable();
                    }
                    break;
                }
                case ML_LIST_ARTIST_TRACKS:
                case ML_COUNT_ARTIST_TRACKS:
                {
                    auto query = artist->media( paramsPtr );
                    switch ( listQuery )
                    {
                        case ML_LIST_ARTIST_TRACKS:
                            *va_arg( args, ml_media_list_t**) = ml_convert_list<ml_media_list_t>(
                                        query->items( nbItems, offset ) );
                            break;
                        case ML_COUNT_ARTIST_TRACKS:
                            *va_arg( args, size_t* ) = query->count();
                            break;
                        default:
                            vlc_assert_unreachable();
                    }
                    break;
                }
                default:
                    vlc_assert_unreachable();
            }
            break;
        }
        case ML_LIST_VIDEOS:
        {
            auto query = m_ml->videoFiles( paramsPtr );
            auto res = ml_convert_list<ml_media_list_t>( query->items( nbItems, offset ) );
            *va_arg( args, ml_media_list_t**) = res;
            break;
        }
        case ML_COUNT_VIDEOS:
        {
            auto query = m_ml->videoFiles( paramsPtr );
            *va_arg( args, size_t* ) = query->count();
            break;
        }
        case ML_LIST_AUDIOS:
        {
            auto query = m_ml->audioFiles( paramsPtr );
            auto res = ml_convert_list<ml_media_list_t>( query->items( nbItems, offset ) );
            *va_arg( args, ml_media_list_t**) = res;
            break;
        }
        case ML_COUNT_AUDIOS:
        {
            auto query = m_ml->audioFiles( paramsPtr );
            *va_arg( args, size_t* ) = query->count();
            break;
        }
        case ML_LIST_ALBUMS:
        {
            auto query = m_ml->albums( paramsPtr );
            auto res = ml_convert_list<ml_album_list_t>( query->items( nbItems, offset ) );
            *va_arg( args, ml_album_list_t**) = res;
            break;
        }
        case ML_COUNT_ALBUMS:
        {
            auto query = m_ml->albums( paramsPtr );
            *va_arg( args, size_t* ) = query->count();
            break;
        }
        case ML_LIST_GENRES:
        {
            auto query = m_ml->genres( paramsPtr );
            auto res = ml_convert_list<ml_genre_list_t>( query->items( nbItems, offset ) );
            *va_arg( args, ml_genre_list_t**) = res;
            break;
        }
        case ML_COUNT_GENRES:
        {
            auto query = m_ml->genres( paramsPtr );
            *va_arg( args, size_t* ) = query->count();
            break;
        }
        case ML_LIST_ARTISTS:
        {
            auto query = m_ml->artists( paramsPtr );
            auto res = ml_convert_list<ml_artist_list_t>( query->items( nbItems, offset ) );
            *va_arg( args, ml_artist_list_t**) = res;
            break;
        }
        case ML_COUNT_ARTISTS:
        {
            auto query = m_ml->artists( paramsPtr );
            *va_arg( args, size_t* ) = query->count();
            break;
        }
        case ML_LIST_GENRE_ARTISTS:
        case ML_COUNT_GENRE_ARTISTS:
        case ML_LIST_GENRE_TRACKS:
        case ML_COUNT_GENRE_TRACKS:
        case ML_LIST_GENRE_ALBUMS:
        case ML_COUNT_GENRE_ALBUMS:
        {
            auto genre = m_ml->genre( va_arg( args, int64_t ) );
            if ( genre == nullptr )
                return VLC_EGENERIC;
            switch( listQuery )
            {
                case ML_LIST_GENRE_ARTISTS:
                case ML_COUNT_GENRE_ARTISTS:
                {
                    auto query = genre->artists( paramsPtr );
                    switch ( listQuery )
                    {
                        case ML_LIST_GENRE_ARTISTS:
                            *va_arg( args, ml_artist_list_t**) = ml_convert_list<ml_artist_list_t>(
                                            query->items( nbItems, offset ) );
                            break;
                        case ML_COUNT_GENRE_ARTISTS:
                            *va_arg( args, size_t* ) = query->count();
                            break;
                        default:
                            vlc_assert_unreachable();
                    }
                    break;
                }
                case ML_LIST_GENRE_TRACKS:
                case ML_COUNT_GENRE_TRACKS:
                {
                    auto query = genre->tracks( paramsPtr );
                    switch ( listQuery )
                    {
                        case ML_LIST_GENRE_TRACKS:
                            *va_arg( args, ml_media_list_t**) = ml_convert_list<ml_media_list_t>( query->items( nbItems, offset ) );
                            break;
                        case ML_COUNT_GENRE_TRACKS:
                            *va_arg( args, size_t*) = query->count();
                            break;
                        default:
                            vlc_assert_unreachable();
                    }
                    break;
                }
                case ML_LIST_GENRE_ALBUMS:
                case ML_COUNT_GENRE_ALBUMS:
                {
                    auto query = genre->albums( paramsPtr );
                    switch ( listQuery )
                    {
                        case ML_LIST_GENRE_ALBUMS:
                            *va_arg( args, ml_album_list_t**) = ml_convert_list<ml_album_list_t>(
                                        query->items( nbItems, offset ) );
                            break;
                        case ML_COUNT_GENRE_ALBUMS:
                            *va_arg( args, size_t* ) = query->count();
                            break;
                        default:
                            vlc_assert_unreachable();
                    }
                    break;
                }
                default:
                    vlc_assert_unreachable();
            }
        }
        case ML_LIST_MEDIA_LABELS:
        case ML_COUNT_MEDIA_LABELS:
        {
            auto media = m_ml->media( va_arg( args, int64_t ) );
            if ( media == nullptr )
                return VLC_EGENERIC;
            auto query = media->labels();
            switch ( listQuery )
            {
                case ML_LIST_MEDIA_LABELS:
                    *va_arg( args, ml_label_list_t**) = ml_convert_list<ml_label_list_t>(
                                query->items( nbItems, offset ) );
                    break;
                case ML_COUNT_MEDIA_LABELS:
                    *va_arg( args, size_t* ) = query->count();
                    break;
                default:
                    vlc_assert_unreachable();
            }
            break;
        }
    }
    return VLC_SUCCESS;
}

void* MediaLibrary::Get( int query, int64_t id )
{
    switch ( query )
    {
        case ML_GET_MEDIA:
        {
            auto media = m_ml->media( id );
            return CreateAndConvert<ml_media_t>( media.get() );
        }
        case ML_GET_ALBUM:
        {
            auto album = m_ml->album( id );
            return CreateAndConvert<ml_album_t>( album.get() );
        }
        case ML_GET_ARTIST:
        {
            auto artist = m_ml->artist( id );
            return CreateAndConvert<ml_artist_t>( artist.get() );
        }
        case ML_GET_GENRE:
        {
            auto genre = m_ml->genre( id );
            return CreateAndConvert<ml_genre_t>( genre.get() );
        }
        case ML_GET_SHOW:
        {
            auto show = m_ml->show( id );
            return CreateAndConvert<ml_show_t>( show.get() );
        }
        default:
            vlc_assert_unreachable();

    }
    return nullptr;
}

static void* Get( vlc_medialibrary_t* module, int query, int64_t id )
{
    auto ml = reinterpret_cast<MediaLibrary*>( module->p_sys );
    return ml->Get( query, id );
}

static int List( vlc_medialibrary_t* module, int query,
                   const ml_query_params_t* params, ... )
{
    va_list args;
    va_start( args, params );
    auto ml = reinterpret_cast<MediaLibrary*>( module->p_sys );
    auto res = ml->List( query, params, args );
    va_end( args );
    return res;
}

static int Control( vlc_medialibrary_t* module, int query, ... )
{
    va_list args;
    va_start( args, query );
    auto ml = reinterpret_cast<MediaLibrary*>( module->p_sys );
    int res = ml->Control( query, args );
    va_end( args );
    return res;
}

static int Open( vlc_object_t* obj )
{
    vlc_medialibrary_t* p_module = reinterpret_cast<vlc_medialibrary_t*>( obj );

    try
    {
        p_module->p_sys = new MediaLibrary( obj );
    }
    catch ( const std::exception& ex )
    {
        msg_Err( obj, "Failed to instantiate/initialize medialibrary: %s", ex.what() );
        return VLC_EGENERIC;
    }
    p_module->pf_control = Control;
    p_module->pf_get = Get;
    p_module->pf_list = List;
    return VLC_SUCCESS;
}

static void Close( vlc_medialibrary_t* module )
{
    MediaLibrary* p_ml = reinterpret_cast<MediaLibrary*>( module->p_sys );
    delete p_ml;
}

vlc_module_begin()
    set_shortname(N_("media library"))
    set_description(N_( "Organize your media" ))
    set_category(CAT_ADVANCED)
    set_subcategory(SUBCAT_ADVANCED_MISC)
    set_capability("medialibrary", 100)
    set_callbacks(Open, Close)
vlc_module_end()
