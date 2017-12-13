#pragma once

#include <medialibrary/IMediaLibrary.h>
#include <components/mediacenter/mcmedialib.hpp>
namespace medialibrary
{

class ExCallback : public IMediaLibraryCb
{
public:
    ~ExCallback() = default;
    /**
     * @brief onFileAdded Will be called when some media get added.
     * Depending if the media is being restored or was just discovered,
     * the media type might be a best effort guess. If the media was freshly
     * discovered, it is extremely likely that no metadata will be
     * available yet.
     * The number of media is undefined, but is guaranteed to be at least 1.
     */
    virtual void onMediaAdded( std::vector<MediaPtr> media ) override
    { }
    /**
     * @brief onFileUpdated Will be called when a file metadata gets updated.
     */
    virtual void onMediaUpdated( std::vector<MediaPtr> media ) override
    { }

    virtual void onMediaDeleted( std::vector<int64_t> mediaIds ) override
    { }

    virtual void onArtistsAdded( std::vector<ArtistPtr> artists ) override
    { }
    virtual void onArtistsModified( std::vector<ArtistPtr> artists ) override
    { }
    virtual void onArtistsDeleted( std::vector<int64_t> artistsIds ) override
    { }

    virtual void onAlbumsAdded( std::vector<AlbumPtr> albums ) override
    { }
    virtual void onAlbumsModified( std::vector<AlbumPtr> albums ) override
    { }
    virtual void onAlbumsDeleted( std::vector<int64_t> albumsIds ) override
    { }
    /**
     * @brief onTrackAdded Called when a media gets detected as an album track
     * and after it has been added to the album representation
     */
    virtual void onTracksAdded( std::vector<AlbumTrackPtr> tracks ) override
    { }
    // Tracks are never modified after their creation, so there is no tracksModified event
    virtual void onTracksDeleted( std::vector<int64_t> trackIds ) override
    { }

    virtual void onPlaylistsAdded( std::vector<PlaylistPtr> playlists ) override
    { }
    virtual void onPlaylistsModified( std::vector<PlaylistPtr> playlists ) override
    { }
    virtual void onPlaylistsDeleted( std::vector<int64_t> playlistIds ) override
    { }

    /**
     * @brief onDiscoveryStarted This callback will be invoked when a folder queued for discovery
     * (by calling IMediaLibrary::discover()) gets processed.
     * @param entryPoint The entrypoint being discovered
     * This callback will be invoked once per endpoint.
     */
    virtual void onDiscoveryStarted( const std::string& entryPoint ) override
    { }
    /**
     * @brief onDiscoveryProgress This callback will be invoked each time the discoverer enters a new
     * entrypoint. Typically, everytime it enters a new folder.
     * @param entryPoint The entrypoint being discovered
     * This callback can be invoked multiple times even though a single entry point was asked to be
     * discovered. ie. In the case of a file system discovery, discovering a folder would make this
     * callback being invoked for all subfolders
     */
    virtual void onDiscoveryProgress( const std::string& entryPoint ) override
    { }
    /**
     * @brief onDiscoveryCompleted Will be invoked when the discovery of a specified entrypoint has
     * completed.
     * ie. in the case of a filesystem discovery, once the folder, and all its files and subfolders
     * have been discovered.
     * This will also be invoked with an empty entryPoint when the initial reload of the medialibrary
     * has completed.
     */
    virtual void onDiscoveryCompleted( const std::string& entryPoint ) override
    { }
    /**
     * @brief onReloadStarted will be invoked when a reload operation begins.
     * @param entryPoint Will be an empty string is the reload is a global reload, or the specific
     * entry point that gets reloaded
     */
    virtual void onReloadStarted( const std::string& entryPoint ) override
    { }
    /**
     * @brief onReloadCompleted will be invoked when a reload operation gets completed.
     * @param entryPoint Will be an empty string is the reload was a global reload, or the specific
     * entry point that has been reloaded
     */
    virtual void onReloadCompleted( const std::string& entryPoint ) override
    { }
    /**
     * @brief onEntryPointRemoved will be invoked when an entrypoint removal request gets processsed
     * by the appropriate worker thread.
     * @param entryPoint The entry point which removal was required
     * @param success A boolean representing the operation's success
     */
    virtual void onEntryPointRemoved( const std::string& entryPoint, bool success ) override
    { }
    /**
     * @brief onEntryPointBanned will be called when an entrypoint ban request is done being processed.
     * @param entryPoint The banned entrypoint
     * @param success A boolean representing the operation's success
     */
    virtual void onEntryPointBanned( const std::string& entryPoint, bool success ) override
    { }
    /**
     * @brief onEntryPointUnbanned will be called when an entrypoint unban request is done being processed.
     * @param entryPoint The unbanned entrypoint
     * @param success A boolean representing the operation's success
     */
    virtual void onEntryPointUnbanned( const std::string& entryPoint, bool success ) override
    { }
    /**
     * @brief onParsingStatsUpdated Called when the parser statistics are updated
     *
     * There is no waranty about how often this will be called.
     * @param percent The progress percentage [0,100]
     *
     */
    virtual void onParsingStatsUpdated( uint32_t percent) override
    { }
    /**
     * @brief onBackgroundTasksIdleChanged Called when background tasks idle state change
     * @param isIdle true when all background tasks are idle, false otherwise
     */
    virtual void onBackgroundTasksIdleChanged( bool isIdle ) override
    { }
};

}
