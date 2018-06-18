/*****************************************************************************
 * vlc_media_library.h: SQL-based media library
 *****************************************************************************
 * Copyright (C) 2008-2010 the VideoLAN Team and AUTHORS
 * $Id$
 *
 * Authors: Antoine Lejeune <phytos@videolan.org>
 *          Jean-Philippe André <jpeg@videolan.org>
 *          Rémi Duraffort <ivoire@videolan.org>
 *          Adrien Maglo <magsoft@videolan.org>
 *          Srikanth Raju <srikiraju at gmail dot com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef VLC_MEDIA_LIBRARY_H
# define VLC_MEDIA_LIBRARY_H

//FIXME: Remove this, it's not needed but it keeps clang-model happy
#include <vlc_common.h>

# ifdef __cplusplus
extern "C" {
# endif

typedef enum ml_media_type_t
{
    ML_MEDIA_TYPE_UNKNOWN,
    ML_MEDIA_TYPE_VIDEO,
    ML_MEDIA_TYPE_AUDIO,
    ML_MEDIA_TYPE_EXTERNAL,
} ml_media_type_t;

typedef enum ml_media_subtype_t
{
    ML_MEDIA_SUBTYPE_UNKNOWN,
    ML_MEDIA_SUBTYPE_SHOW_EPISODE,
    ML_MEDIA_SUBTYPE_MOVIE,
    ML_MEDIA_SUBTYPE_ALBUMTRACK,
} ml_media_subtype_t;

typedef enum ml_file_type_t
{
    ML_FILE_TYPE_UNKNOWN,
    ML_FILE_TYPE_MAIN,
    ML_FILE_TYPE_PART,
    ML_FILE_TYPE_SOUNDTRACK,
    ML_FILE_TYPE_SUBTITLE,
    ML_FILE_TYPE_PLAYLIST,
} ml_file_type_t;

typedef struct ml_movie_t
{
    char* psz_title;
    char* psz_summary;
    char* psz_artwork_mrl;
    char* psz_imdb_id;
} ml_movie_t;

typedef struct ml_show_episode_t
{
    char* psz_artwork_mrl;
    char* psz_name;
    char* psz_summary;
    char* psz_tvdb_id;
    uint32_t i_episode_nb;
    uint32_t i_season_number;
} ml_show_episode_t;

typedef struct ml_show_t
{
    void (*pf_release)( struct ml_show_t* );

    int64_t i_id;
    char* psz_name;
    char* psz_summary;
    char* psz_artwork_mrl;
    char* psz_tvdb_id;
    unsigned int i_release_year;
    uint32_t i_nb_episodes;
    uint32_t i_nb_seasons;
} ml_show_t;

typedef struct ml_album_track_t
{
    int64_t i_artist_id;
    int64_t i_album_id;
    int64_t i_genre_id;

    int i_track_nb;
    int i_disc_nb;
} ml_album_track_t;

typedef struct ml_label_t
{
    void (*pf_release)( struct ml_label_t* );
    int64_t i_id;
    char* psz_name;
} ml_label_t;

typedef struct ml_label_list_t
{
    void (*pf_release)( struct ml_label_list_t* );

    size_t i_nb_items;
    ml_label_t* p_items;
} ml_label_list_t;

typedef struct ml_file_t
{
    void (*pf_release)( struct ml_file_t* );

    char* psz_mrl;
    ml_file_type_t i_type;
    bool b_external;
} ml_file_t;

typedef struct ml_file_list_t
{
    void (*pf_release)( struct ml_file_list_t* );

    size_t i_nb_items;
    ml_file_t* p_items;
} ml_file_list_t;

typedef struct ml_media_t
{
    void (*pf_release)( struct ml_media_t* );

    int64_t i_id;

    ml_media_type_t i_type;
    ml_media_subtype_t i_subtype;

    ml_file_list_t* p_files;

    int32_t i_year;
    /* Duration in milliseconds */
    int64_t i_duration;
    uint32_t i_playcount;
    char* psz_title;

    char* psz_artwork_mrl;
    bool b_is_favorite;

    union
    {
        ml_show_episode_t show_episode;
        ml_movie_t movie;
        ml_album_track_t album_track;
    };
} ml_media_t;


typedef struct ml_artist_t {
    void (*pf_release)( struct ml_artist_t* );

    int64_t i_id;
    char* psz_name;
    char* psz_shortbio;
    char* psz_artwork_mrl;
    char* psz_mb_id;

    unsigned int i_nb_album;
    unsigned int i_nb_tracks;
} ml_artist_t;

typedef struct ml_artist_list_t
{
    void (*pf_release)( struct ml_artist_list_t* );

    size_t i_nb_items;
    ml_artist_t* p_items;
} ml_artist_list_t;

typedef struct ml_album_t {
    void (*pf_release)( struct ml_album_t* );

    int64_t i_id;
    char* psz_title;
    char* psz_summary;
    char* psz_artwork_mrl;
    char* psz_artist;
    int64_t i_artist_id;

    ml_artist_list_t* p_featuring;

    size_t i_nb_tracks;
    unsigned int i_duration;
    unsigned int i_year;
} ml_album_t;

typedef struct ml_genre_t
{
    void (*pf_release)( struct ml_genre_t* );

    int64_t i_id;
    char* psz_name;
    size_t i_nb_tracks;
} ml_genre_t;

typedef struct ml_media_list_t
{
    void (*pf_release)( struct ml_media_list_t* );

    size_t i_nb_items;
    ml_media_t* p_items;
} ml_media_list_t;

typedef struct ml_album_list_t
{
    void (*pf_release)( struct ml_album_list_t* );

    size_t i_nb_items;
    ml_album_t* p_items;
} ml_album_list_t;

typedef struct ml_show_list_t
{
    void (*pf_release)( struct ml_show_list_t* );

    size_t i_nb_items;
    ml_show_t* p_items;
} ml_show_list_t;

typedef struct ml_genre_list_t {
    void (*pf_release)( struct ml_genre_list_t* );

    size_t i_nb_items;
    ml_genre_t* p_items;
} ml_genre_list_t;

typedef enum ml_sorting_criteria_t
{
    /*
     * Default depends on the entity type:
     * - By track number (and disc number) for album tracks
     * - Alphabetical order for others
     */
    ML_SORTING_DEFAULT,
    ML_SORTING_ALPHA,
    ML_SORTING_DURATION,
    ML_SORTING_INSERTIONDATE,
    ML_SORTING_LASTMODIFICATIONDATE,
    ML_SORTING_RELEASEDATE,
    ML_SORTING_FILESIZE,
    ML_SORTING_ARTIST,
    ML_SORTING_PLAYCOUNT,
    ML_SORTING_ALBUM,
    ML_SORTING_FILENAME,
    ML_SORTING_TRACKNUMBER,
} ml_sorting_criteria_t;

typedef struct ml_query_params_t ml_query_params_t;
struct ml_query_params_t
{
    const char* psz_pattern;
    ml_sorting_criteria_t i_sort;
    uint32_t i_nbResults;
    uint32_t i_offset;
    bool b_desc;
};

typedef struct vlc_medialibrary_t vlc_medialibrary_t;
struct vlc_medialibrary_t
{
    struct vlc_common_members obj;
    void* p_sys;

    int (*pf_control)( vlc_medialibrary_t* p_ml, int i_query, ... );
    /**
     * List some entities from the medialibrary.
     *
     * \param p_ml The medialibrary module instance.
     * \param i_query The type search to be performed. \see ml_list enumeration
     * \param p_params A pointer to a ml_query_params_t structure, or NULL for
     * the default parameters (alphabetical ascending sort, no pagination)
     *
     * \return VLC_SUCCESS or an error code
     *
     * Refer to the individual list of ml_list requests for the additional
     * per-query input/ouput parameters values & types
     */
    int (*pf_list)( vlc_medialibrary_t* p_ml, int i_query,
                    const ml_query_params_t* p_params, ... );

    /**
     * Get a specific entity by its id.
     *
     * \return The required entity, or a NULL pointer if couldn't be found.
     *
     * Refer to the list of queries for the specific return type
     */
    void* (*pf_get)( vlc_medialibrary_t* p_ml, int i_query, int64_t i_id );
};

typedef struct ml_entrypoint_t ml_entrypoint_t;
struct ml_entrypoint_t
{
    char* psz_mrl; /**< This entrypoint's MRL. Will be NULL if b_present is false */
    bool b_present; /**< The presence state for this entrypoint. */
    bool b_banned; /**< Will be true if the user required this entrypoint to be excluded */
};

enum ml_control
{
    /* Adds a folder to discover through the medialibrary */
    ML_ADD_FOLDER,              /**< arg1: mrl (const char*)  res: can't fail */
    ML_REMOVE_FOLDER,           /**< arg1: mrl (const char*)  res: can't fail */
    ML_BAN_FOLDER,              /**< arg1: mrl (const char*)  res: can't fail */
    ML_UNBAN_FOLDER,            /**< arg1: mrl (const char*)  res: can't fail */
    ML_LIST_FOLDERS,            /**< arg1: entrypoints (ml_entrypoint_t**); arg2: nb results(size_t*), res: can fail */

    /* Pause/resume background operations, such as media discovery & media analysis */
    ML_PAUSE_BACKGROUND,        /**< no args; can't fail */
    ML_RESUME_BACKGROUND,       /**< no args; can't fail */

    /* Misc operations */
    ML_CLEAR_HISTORY,           /**< no args; can't fail */

    /* Media management */
    ML_MEDIA_INCREASE_PLAY_COUNT,       /**< arg1: media id; can fail */
};


static inline void vlc_ml_add_folder( vlc_medialibrary_t* p_ml, const char* psz_folder )
{
    p_ml->pf_control( p_ml, ML_ADD_FOLDER, psz_folder );
}

static inline void vlc_ml_remove_folder( vlc_medialibrary_t* p_ml, const char* psz_folder )
{
    p_ml->pf_control( p_ml, ML_REMOVE_FOLDER, psz_folder );
}

static inline void vlc_ml_ban_folder( vlc_medialibrary_t* p_ml, const char* psz_folder )
{
    p_ml->pf_control( p_ml, ML_BAN_FOLDER, psz_folder );
}

static inline void vlc_ml_unban_folder( vlc_medialibrary_t* p_ml, const char* psz_folder )
{
    p_ml->pf_control( p_ml, ML_UNBAN_FOLDER, psz_folder );
}

static inline int vlc_ml_list_folder( vlc_medialibrary_t* p_ml,
                                      ml_entrypoint_t** pp_entrypoints, size_t* p_nb_items )
{
    return p_ml->pf_control( p_ml, ML_LIST_FOLDERS, pp_entrypoints, p_nb_items );
}

static inline void vlc_ml_pause_background( vlc_medialibrary_t* p_ml )
{
    p_ml->pf_control( p_ml, ML_PAUSE_BACKGROUND );
}

static inline void vlc_ml_resume_background( vlc_medialibrary_t* p_ml )
{
    p_ml->pf_control( p_ml, ML_RESUME_BACKGROUND );
}

static inline void vlc_ml_clear_history( vlc_medialibrary_t* p_ml )
{
    p_ml->pf_control( p_ml, ML_CLEAR_HISTORY );
}

static inline int vlc_ml_increase_playcount( vlc_medialibrary_t* p_ml, int64_t i_media_id )
{
    return p_ml->pf_control( p_ml, ML_MEDIA_INCREASE_PLAY_COUNT, i_media_id );
}

enum ml_get_queries
{
    ML_GET_MEDIA,           /**< arg1: Media  ID; ret: ml_media_t*  */
    ML_GET_ALBUM,           /**< arg1: Album  ID; ret: ml_album_t*  */
    ML_GET_ARTIST,          /**< arg1: Artist ID; ret: ml_artist_t* */
    ML_GET_GENRE,           /**< arg1: Genre  ID; ret: ml_genre_t*  */
    ML_GET_SHOW,            /**< arg1: Show   ID; ret: ml_show_t*   */
};

static inline ml_media_t* ml_get_media( vlc_medialibrary_t* p_ml, int64_t i_media_id )
{
    return (ml_media_t*)p_ml->pf_get( p_ml, ML_GET_MEDIA, i_media_id );
}

static inline ml_album_t* ml_get_album( vlc_medialibrary_t* p_ml, int64_t i_album_id )
{
    return (ml_album_t*)p_ml->pf_get( p_ml, ML_GET_ALBUM, i_album_id );
}

static inline ml_artist_t* ml_get_artist( vlc_medialibrary_t* p_ml, int64_t i_artist_id )
{
    return (ml_artist_t*)p_ml->pf_get( p_ml, ML_GET_ARTIST, i_artist_id );
}

static inline ml_genre_t* ml_get_genre( vlc_medialibrary_t* p_ml, int64_t i_genre_id )
{
    return (ml_genre_t*)p_ml->pf_get( p_ml, ML_GET_GENRE, i_genre_id );
}

static inline ml_show_t* ml_get_show( vlc_medialibrary_t* p_ml, int64_t i_show_id )
{
    return (ml_show_t*)p_ml->pf_get( p_ml, ML_GET_SHOW, i_show_id );
}

enum ml_list_queries
{
    /* General listing: */

    ML_LIST_VIDEOS,           /**< arg1 (out): ml_media_list_t**    */
    ML_COUNT_VIDEOS,          /**< arg1 (out): size_t*              */
    ML_LIST_AUDIOS,           /**< arg1 (out): ml_media_list_t**    */
    ML_COUNT_AUDIOS,          /**< arg1 (out): size_t*              */
    ML_LIST_ALBUMS,           /**< arg1 (out): ml_album_list_t**    */
    ML_COUNT_ALBUMS,          /**< arg1 (out): size_t*              */
    ML_LIST_GENRES,           /**< arg1 (out): ml_genre_list_t**    */
    ML_COUNT_GENRES,          /**< arg1 (out): size_t*              */
    ML_LIST_ARTISTS,          /**< arg1 (out): ml_genre_list_t**    */
    ML_COUNT_ARTISTS,         /**< arg1 (out): size_t*              */

    /* Album specific listings */
    ML_LIST_ALBUM_TRACKS,     /**< arg1: The album id. arg2 (out): ml_media_list_t**  */
    ML_COUNT_ALBUM_TRACKS,    /**< arg1: The album id. arg2 (out): size_t**  */

    /* Artist specific listings */
    ML_LIST_ARTIST_ALBUMS,  /**< arg1: The artist id. arg2(out): ml_album_list_t**    */
    ML_COUNT_ARTIST_ALBUMS, /**< arg1: The artist id. arg2(out): size_t*              */
    ML_LIST_ARTIST_TRACKS,  /**< arg1: The artist id. arg2(out): ml_media_list_t**    */
    ML_COUNT_ARTIST_TRACKS, /**< arg1: The artist id. arg2(out): size_t*              */

    /* Genre specific listings */
    ML_LIST_GENRE_ARTISTS,    /**< arg1: genre id;  arg2 (out): ml_artist_list_t**  */
    ML_COUNT_GENRE_ARTISTS,   /**< arg1: genre id;  arg2 (out): size_t*             */
    ML_LIST_GENRE_TRACKS,     /**< arg1: genre id;  arg2 (out): ml_media_list_t**   */
    ML_COUNT_GENRE_TRACKS,    /**< arg1: genre id;  arg2 (out): size_t*             */
    ML_LIST_GENRE_ALBUMS,     /**< arg1: genre id;  arg2 (out): ml_album_list_t**   */
    ML_COUNT_GENRE_ALBUMS,    /**< arg1: genre id;  arg2 (out): size_t*             */

    /* Media specific listings */
    ML_LIST_MEDIA_LABELS,     /**< arg1: media id;  arg2 (out) ml_label_list_t**    */
    ML_COUNT_MEDIA_LABELS,    /**< arg1: media id;  arg2 (out) size_t*              */
};

static inline ml_media_list_t* ml_list_album_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_album_id )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_ALBUM_TRACKS, params, i_album_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_album_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_album_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_ALBUM_TRACKS, params, i_album_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_album_list_t* ml_list_artist_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_artist_id )
{
    ml_album_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_ARTIST_ALBUMS, params, i_artist_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_artist_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_artist_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_ARTIST_ALBUMS, params, i_artist_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_media_list_t* ml_list_artist_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_artist_id )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_ARTIST_TRACKS, params, i_artist_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_artist_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_artist_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_ARTIST_TRACKS, params, i_artist_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_media_list_t* ml_list_video_media( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_VIDEOS, params, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_video_media( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_VIDEOS, params, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_media_list_t* ml_list_audio_media( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_AUDIOS, params, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_audio_media( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_AUDIOS, params, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_album_list_t* ml_list_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    ml_album_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_ALBUMS, params, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_ALBUMS, params, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_genre_list_t* ml_list_genres( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    ml_genre_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_GENRES, params, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_genres( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_GENRES, params, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_artist_list_t* ml_list_artists( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    ml_artist_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_ARTISTS, params, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_artists( vlc_medialibrary_t* p_ml, ml_query_params_t* params )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_ARTISTS, params, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_media_list_t* ml_list_genre_artists( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_GENRE_ARTISTS, params, i_genre_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_genre_artists( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_GENRE_ARTISTS, params, i_genre_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_media_list_t* ml_list_genre_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    ml_media_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_GENRE_TRACKS, params, i_genre_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_genre_tracks( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_GENRE_TRACKS, params, i_genre_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_album_list_t* ml_list_genre_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    ml_album_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_GENRE_ALBUMS, params, i_genre_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_genre_albums( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_genre_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_COUNT_GENRE_ALBUMS, params, i_genre_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

static inline ml_label_list_t* ml_list_media_labels( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_media_id )
{
    ml_label_list_t* res;
    if ( p_ml->pf_list( p_ml, ML_LIST_MEDIA_LABELS, params, i_media_id, &res ) != VLC_SUCCESS )
        return NULL;
    return res;
}

static inline size_t ml_count_media_labels( vlc_medialibrary_t* p_ml, ml_query_params_t* params, int64_t i_media_id )
{
    size_t count;
    if ( p_ml->pf_list( p_ml, ML_LIST_MEDIA_LABELS, params, i_media_id, &count ) != VLC_SUCCESS )
        return 0;
    return count;
}

#define vlc_ml_release( obj ) (obj)->pf_release( ( obj ) )

VLC_API void vlc_ml_entrypoints_release( ml_entrypoint_t* p_list, size_t i_nb_items );

/*****************************************************************************
 * ML Enums
 *****************************************************************************/

#define ML_PERSON_ARTIST        "Artist"
#define ML_PERSON_ALBUM_ARTIST  "Album Artist"
#define ML_PERSON_ENCODER       "Encoder"
#define ML_PERSON_PUBLISHER     "Publisher"


/** List of Query select types.
 * In a query array or variable argument list, each select type is followed
 * by an argument (X) of variable type (char* or int, @see ml_element_t).
 * These types can be used either in the query list or in the result array.
 * Some types are reserved for the result array:
 */
typedef enum
{
    ML_ALBUM = 1,              /**< Album Title */
    ML_ALBUM_ID,               /**< Album ID */
    ML_ALBUM_COVER,            /**< Album Cover art url */
    /* FIXME: Remove ML_ARTIST */
    ML_ARTIST,                 /**< Artist, interpreted as ML_PEOPLE
                                    && ML_PEOPLE_ROLE = ML_PERSON_ARTIST */
    ML_ARTIST_ID,              /**< Artist ID, interpreted as ML_PEOPLE_ID
                                    && ML_PEOPLE_ROLE = ML_PERSON_ARTIST */
    ML_COMMENT,                /**< Comment about media */
    ML_COUNT_MEDIA,            /**< Number of medias */
    ML_COUNT_ALBUM,            /**< Number of albums */
    ML_COUNT_PEOPLE,           /**< Number of people */
    ML_COVER,                  /**< Cover art url */
    ML_DURATION,               /**< Duration in ms */
    ML_DISC_NUMBER,            /**< Disc number of the track */
    ML_EXTRA,                  /**< Extra/comment (string) on the media */
    ML_FIRST_PLAYED,           /**< First time media was played */
    ML_FILESIZE,               /**< Size of the media file */
    ML_GENRE,                  /**< Genre of the media (if any) */
    ML_ID,                     /**< Media ID */
    ML_IMPORT_TIME,            /**< Date when media was imported */
    ML_LANGUAGE,               /**< Language */
    ML_LAST_PLAYED,            /**< Last play UNIX timestamp */
    ML_LAST_SKIPPED,           /**< Time when media was last skipped */
    ML_ORIGINAL_TITLE,         /**< Media original title (if any) */
    ML_PEOPLE,                 /**< Any People associated with this media */
    ML_PEOPLE_ID,              /**< Id of a person */
    ML_PEOPLE_ROLE,            /**< Person role */
    ML_PLAYED_COUNT,           /**< Media play count */
    ML_PREVIEW,                /**< Url of the video preview */
    ML_SKIPPED_COUNT,          /**< Number of times skipped */
    ML_SCORE,                  /**< Computed media score */
    ML_TITLE,                  /**< Media title */
    ML_TRACK_NUMBER,           /**< Media track number (if any) */
    ML_TYPE,                   /**< Media type. @see ml_type_e */
    ML_URI,                    /**< Media full URI. */
    ML_VOTE,                   /**< Media user vote value */
    ML_YEAR,                   /**< Media publishing year */
    ML_DIRECTORY,              /**< Monitored directory */
    ML_MEDIA,                  /**< Full media descriptor. @see ml_media_t */
    ML_MEDIA_SPARSE,           /**< Sparse media. @see ml_media_t */
    ML_MEDIA_EXTRA,            /**< Sparse + Extra = Full media */

    /* Some special elements */
    ML_LIMIT     = -1,         /**< Limit a query to X results */
    ML_SORT_DESC = -2,         /**< Sort a query descending on argument X */
    ML_SORT_ASC  = -3,         /**< Sort a query ascending on argument X */
    ML_DISTINCT  = -4,         /**< Add DISTINCT to SELECT statements. */
    ML_END       = -42         /**< End of argument list */
} ml_select_e;

/** Media types (audio, video, etc...) */
typedef enum
{
    ML_UNKNOWN   = 0,       /**< Unknown media type */
    ML_AUDIO     = 1 << 0,  /**< Audio only media */
    ML_VIDEO     = 1 << 1,  /**< Video media. May contain audio channels */
    ML_STREAM    = 1 << 2,  /**< Streamed media = not a local file */
    ML_NODE      = 1 << 3,  /**< Nodes like simple nodes, directories, playlists, etc */
    ML_REMOVABLE = 1 << 4,  /**< Removable media: CD/DVD/Card/... */
} ml_type_e;

/** Query result item/list type: integers, strings, medias, timestamps */
typedef enum {
    ML_TYPE_INT,        /**< Object is an int */
    ML_TYPE_PSZ,        /**< A string char* */
    ML_TYPE_TIME,       /**< A timestamp mtime_t */
    ML_TYPE_MEDIA,      /**< A pointer to a media ml_media_t* */
} ml_result_type_e;

#ifdef __cplusplus
}
#endif /* C++ */

#endif /* VLC_MEDIA_LIBRARY_H */
