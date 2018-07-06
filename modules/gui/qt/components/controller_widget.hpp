/*****************************************************************************
 * controller_widget.hpp : Controller Widget for the controllers
 ****************************************************************************
 * Copyright (C) 2006-2008 the VideoLAN team
 * $Id$
 *
 * Authors: Jean-Baptiste Kempf <jb@videolan.org>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef VLC_QT_CONTROLLER_WIDGET_HPP_
#define VLC_QT_CONTROLLER_WIDGET_HPP_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qt.hpp"
#include "input_manager.hpp"
#include <vlc_vout.h>                       /* vout_thread_t for aspect ratio combobox */

#include <QWidget>
#include <QToolButton>
#include <QComboBox>

class QLabel;
class QFrame;
class QAbstractSlider;

/**
 * SPECIAL Widgets that are a bit more than just a ToolButton
 * and have an icon/behaviour that changes depending on the context:
 * - playButton
 * - A->B Button
 * - Teletext group buttons
 * - Sound Widget group
 **/
class PlayButton : public QToolButton
{
    Q_OBJECT
public slots:
    void updateButtonIcons( bool );
};

class LoopButton : public QToolButton
{
    Q_OBJECT
public slots:
    void updateButtonIcons( int );
};

class AtoB_Button : public QToolButton
{
    Q_OBJECT
private slots:
    void updateButtonIcons( bool, bool );
};

class AspectRatioComboBox : public QComboBox
{
    Q_OBJECT
public:
    AspectRatioComboBox( intf_thread_t* _p_intf ) : p_intf( _p_intf )
    {
        CONNECT( THEMIM->getIM(), voutChanged( bool ),
                 this, updateRatios() );
        CONNECT( this, currentIndexChanged( int ),
                 this, updateAspectRatio( int ) );
        updateRatios();
    }

public slots:
    void updateRatios();
    void updateAspectRatio( int );

private:
    intf_thread_t* p_intf;
};

class SoundWidget : public QWidget
{
    Q_OBJECT

public:
    SoundWidget( QWidget *parent, intf_thread_t  *_p_i, bool,
                 bool b_special = false );
    void setMuted( bool );

protected:
    bool eventFilter( QObject *obj, QEvent *e ) Q_DECL_OVERRIDE;

private:
    intf_thread_t       *p_intf;
    QLabel              *volMuteLabel;
    QAbstractSlider     *volumeSlider;
    QFrame              *volumeControlWidget;
    QMenu               *volumeMenu;

    bool                b_is_muted;
    bool                b_ignore_valuechanged;

protected slots:
    void userUpdateVolume( int );
    void libUpdateVolume( float );
    void updateMuteStatus( bool );
    void refreshLabels( void );
    void showVolumeMenu( QPoint pos );
    void valueChangedFilter( int );

signals:
    void valueReallyChanged( int );
};

class SoundWidgetModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(long i_volume READ i_volume WRITE setI_volume NOTIFY i_volumeChanged)

public:
    SoundWidgetModel(intf_thread_t  *_p_i);
    SoundWidgetModel() {}
    Q_INVOKABLE void setMuted( bool );

    float volume(){ return _volume; }
    void setVolume(float v) { _volume = v; emit volumeChanged(); }
    long i_volume(){ return _i_volume; }
    void setI_volume(long v) { _i_volume = v; emit i_volumeChanged(); }

signals:
    void volumeChanged();
    void i_volumeChanged();

public slots:
    Q_INVOKABLE void onVolumeChanged(float v);
    Q_INVOKABLE void onI_volumeChanged(long v);

private:
    bool                _b_is_muted;
    bool                _b_ignore_valuechanged;
    float               _volume;
    long                _i_volume;
    intf_thread_t*      p_intf;
protected slots:
    void libUpdateVolume( float );
    void updateMuteStatus( bool );
};

#endif
