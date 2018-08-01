/*****************************************************************************
 * controller_widget.cpp : Controller Widget for the controllers
 ****************************************************************************
 * Copyright (C) 2006-2008 the VideoLAN team
 * $Id$
 *
 * Authors: Jean-Baptiste Kempf <jb@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "controller_widget.hpp"
#include "controller.hpp"

#include "input_manager.hpp"         /* Get notification of Volume Change */
#include "util/input_slider.hpp"     /* SoundSlider */
#include "util/imagehelper.hpp"

#include <math.h>

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QWidgetAction>
#include <QMouseEvent>

#define VOLUME_MAX 125

/**
 * SoundWidgetModel
 **/
SoundWidgetModel::SoundWidgetModel( intf_thread_t * _p_intf ) : p_intf(_p_intf)
{
    /* Set the volume from the config */
    _volume = playlist_VolumeGet( THEPL );
    libUpdateVolume( (_volume >= 0.f) ? _volume : 1.f );
    /* Sync mute status */
    if( playlist_MuteGet( THEPL ) > 0 )
        updateMuteStatus( true );

    /* Volume control connection */
    CONNECT( THEMIM, volumeChanged( float ), this, libUpdateVolume( float ) );
    CONNECT( THEMIM, soundMuteChanged( bool ), this, updateMuteStatus( bool ) );

    /*  it seems that, after binding a Q_PROPERTY to QML,
     *  value on QML side chagned will not call emit signal, so I call it manully
     *
    CONNECT( this, volumeChanged(), this, onVolumeChanged() );
    CONNECT( this, i_volumeChanged(), this, onI_volumeChanged() );
    */
}

/* libvlc changed value event slot */
void SoundWidgetModel::libUpdateVolume( float v )
{
    setI_volume(lroundf(v * 100.f));
    setVolume(v);
}

/* libvlc mute/unmute event slot */
void SoundWidgetModel::updateMuteStatus( bool mute )
{
    _b_is_muted = mute;
}

void SoundWidgetModel::onI_volumeChanged(long v)
{
    _i_volume = v;
    setVolume(v / 100.f);
    setMuted(false);
    playlist_VolumeSet(THEPL, _volume);
}

void SoundWidgetModel::onVolumeChanged(float v)
{
    _volume = v;
    setI_volume( lround(v * 100.f) );
    setMuted(false);
    playlist_VolumeSet(THEPL, _volume);
}

void SoundWidgetModel::setMuted( bool mute )
{
    _b_is_muted = mute;
    playlist_t *p_playlist = THEPL;
    playlist_MuteSet( p_playlist, mute );
}

/**
 * Play Button
 **/
void PlayButton::updateButtonIcons( bool b_playing )
{
    setIcon( b_playing ? QIcon( ":/toolbar/pause_b.svg" ) : QIcon( ":/toolbar/play_b.svg" ) );
    setToolTip( b_playing ? qtr( "Pause the playback" )
                          : qtr( I_PLAY_TOOLTIP ) );
}

void AtoB_Button::updateButtonIcons( bool timeA, bool timeB )
{
    if( !timeA && !timeB)
    {
        setIcon( QIcon( ":/toolbar/atob_nob.svg" ) );
        setToolTip( qtr( "Loop from point A to point B continuously\n"
                         "Click to set point A" ) );
    }
    else if( timeA && !timeB )
    {
        setIcon( QIcon( ":/toolbar/atob_noa.svg" ) );
        setToolTip( qtr( "Click to set point B" ) );
    }
    else if( timeA && timeB )
    {
        setIcon( QIcon( ":/toolbar/atob.svg" ) );
        setToolTip( qtr( "Stop the A to B loop" ) );
    }
}

void LoopButton::updateButtonIcons( int value )
{
    setChecked( value != NORMAL );
    setIcon( ( value == REPEAT_ONE ) ? QIcon( ":/buttons/playlist/repeat_one.svg" )
                                     : QIcon( ":/buttons/playlist/repeat_all.svg" ) );
}

void AspectRatioComboBox::updateRatios()
{
    /* Clear the list before updating */
    clear();
    vlc_value_t *val_list;
    char **text_list;
    size_t count;

    vout_thread_t* p_vout = THEMIM->getVout();

    /* Disable if there is no vout */
    if( p_vout == NULL )
    {
        addItem( qtr("Aspect Ratio") );
        setDisabled( true );
        return;
    }

    var_Change( p_vout, "aspect-ratio", VLC_VAR_GETCHOICES,
                &count, &val_list, &text_list );
    for( size_t i = 0; i < count; i++ )
    {
        addItem( qfu( text_list[i] ),
                 QString( val_list[i].psz_string ) );
        free(text_list[i]);
        free(val_list[i].psz_string);
    }
    setEnabled( true );
    free(text_list);
    free(val_list);
    vlc_object_release( p_vout );
}

void AspectRatioComboBox::updateAspectRatio( int x )
{
    vout_thread_t* p_vout = THEMIM->getVout();
    if( p_vout && x >= 0 )
    {
        var_SetString( p_vout, "aspect-ratio", qtu( itemData(x).toString() ) );
    }
    if( p_vout )
        vlc_object_release( p_vout );
}

