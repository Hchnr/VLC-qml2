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
#include <vlc_media_library.h>
#include <vlc_modules.h>
#include <libvlc.h>

#include <assert.h>

void vlc_ml_entrypoints_release( ml_entrypoint_t* p_list, size_t i_nb_items )
{
    for ( size_t i = 0; i < i_nb_items; ++i )
    {
        free( p_list[i].psz_mrl );
    }
    free( p_list );
}

#undef vlc_ml_create
vlc_medialibrary_t* vlc_ml_create( vlc_object_t* p_obj )
{
    vlc_medialibrary_t *p_ml = vlc_custom_create( p_obj, sizeof( *p_ml ), "medialibrary" );
    if ( unlikely( p_ml == NULL ) )
        return NULL;
    p_ml->p_module = module_need( p_ml, "medialibrary", NULL, false );
    if ( p_ml->p_module == NULL )
    {
        vlc_object_release( p_ml );
        return NULL;
    }
    return p_ml;
}

void vlc_ml_release( vlc_medialibrary_t* p_ml )
{
    assert( p_ml != NULL );
    module_unneed( p_ml, p_ml->p_module );
    vlc_object_release( p_ml );
}
