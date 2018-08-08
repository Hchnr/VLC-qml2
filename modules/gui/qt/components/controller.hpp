/*****************************************************************************
 * controller.hpp : Controller for the main interface
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

#ifndef QVLC_CONTROLLER_H_
#define QVLC_CONTROLLER_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "actions_manager.hpp"
#include "qt.hpp"

#include <QFrame>
#include <QString>
#include <QSizeGrip>

#define MAIN_TB1_DEFAULT "64;39;64;38;65"
#define MAIN_TB2_DEFAULT "0-2;64;3;1;4;64;7;9;64;10;20;19;64-4;37;65;35-4"
#define ADV_TB_DEFAULT "12;11;13;14"
#define INPT_TB_DEFAULT "43;33-4;44"
#define FSC_TB_DEFAULT "0-2;64;3;1;4;64;37;64;38;64;8;65;25;35-4;34"

#define I_PLAY_TOOLTIP N_("Play\nIf the playlist is empty, open a medium")

class QBoxLayout;
class QHBoxLayout;

class QAbstractButton;
class QToolButton;

class WidgetListing;
class VideoWidget;

class QSignalMapper;
class QTimer;

typedef enum buttonType_e
{
    PLAY_BUTTON,
    STOP_BUTTON,
    OPEN_BUTTON,
    PREV_SLOW_BUTTON,
    NEXT_FAST_BUTTON,
    SLOWER_BUTTON,
    FASTER_BUTTON,
    FULLSCREEN_BUTTON,
    DEFULLSCREEN_BUTTON,
    EXTENDED_BUTTON,
    PLAYLIST_BUTTON,
    SNAPSHOT_BUTTON,
    RECORD_BUTTON,
    ATOB_BUTTON,
    FRAME_BUTTON,
    REVERSE_BUTTON,
    SKIP_BACK_BUTTON,
    SKIP_FW_BUTTON,
    QUIT_BUTTON,
    RANDOM_BUTTON,
    LOOP_BUTTON,
    INFO_BUTTON,
    PREVIOUS_BUTTON,
    NEXT_BUTTON,
    OPEN_SUB_BUTTON,
    FULLWIDTH_BUTTON,
    BUTTON_MAX,

    SPLITTER = 0x20,
    INPUT_SLIDER,
    TIME_LABEL,
    VOLUME,
    VOLUME_SPECIAL,
    MENU_BUTTONS,
    TELETEXT_BUTTONS,
    ADVANCED_CONTROLLER,
    PLAYBACK_BUTTONS,
    ASPECT_RATIO_COMBOBOX,
    SPEED_LABEL,
    TIME_LABEL_ELAPSED,
    TIME_LABEL_REMAINING,
    SPECIAL_MAX,

    WIDGET_SPACER = 0x40,
    WIDGET_SPACER_EXTEND,
    WIDGET_MAX,
} buttonType_e;


static const char* const nameL[BUTTON_MAX] = { N_("Play"), N_("Stop"), N_("Open"),
    N_("Previous / Backward"), N_("Next / Forward"), N_("Slower"), N_("Faster"), N_("Fullscreen"),
    N_("De-Fullscreen"), N_("Extended panel"), N_("Playlist"), N_("Snapshot"),
    N_("Record"), N_("A→B Loop"), N_("Frame By Frame"), N_("Trickplay Reverse"),
    N_("Step backward" ), N_("Step forward"), N_("Quit"), N_("Random"),
    N_("Loop / Repeat"), N_("Information"), N_("Previous"), N_("Next"),
    N_("Open subtitles"), N_("Dock fullscreen controller")
};
static const char* const tooltipL[BUTTON_MAX] = { I_PLAY_TOOLTIP,
    N_("Stop playback"), N_("Open a medium"),
    N_("Previous media in the playlist, skip backward when held"),
    N_("Next media in the playlist, skip forward when held"), N_("Slower"), N_("Faster"),
    N_("Toggle the video in fullscreen"), N_("Toggle the video out fullscreen"),
    N_("Show extended settings" ), N_( "Toggle playlist" ),
    N_( "Take a snapshot" ), N_( "Record" ),
    N_( "Loop from point A to point B continuously." ), N_("Frame by frame"),
    N_("Reverse"), N_("Step backward"), N_("Step forward"), N_("Quit"),
    N_("Random"), N_("Change the loop and repeat modes"), N_("Information"),
    N_("Previous media in the playlist"), N_("Next media in the playlist"),
    N_("Open subtitle file"),
    N_("Dock/undock fullscreen controller to/from bottom of screen"),
};
static const QString iconL[BUTTON_MAX] ={ ":/toolbar/play_b.svg", ":/toolbar/stop_b.svg",
    ":/toolbar/eject.svg", ":/toolbar/previous_b.svg", ":/toolbar/next_b.svg",
    ":/toolbar/slower.svg", ":/toolbar/faster.svg", ":/toolbar/fullscreen.svg",
    ":/toolbar/defullscreen.svg", ":/toolbar/extended.svg", ":/toolbar/playlist.svg",
    ":/toolbar/snapshot.svg", ":/toolbar/record.svg", ":/toolbar/atob_nob.svg",
    ":/toolbar/frame.svg", ":/toolbar/reverse.svg", ":/toolbar/skip_back.svg",
    ":/toolbar/skip_fw.svg", ":/toolbar/clear.svg", ":/buttons/playlist/shuffle_on.svg",
    ":/buttons/playlist/repeat_all.svg", ":/menu/info.svg",
    ":/toolbar/previous_b.svg", ":/toolbar/next_b.svg", ":/toolbar/eject.svg", ":/toolbar/space.svg"
};

enum
{
   WIDGET_NORMAL = 0x0,
   WIDGET_FLAT   = 0x1,
   WIDGET_BIG    = 0x2,
   WIDGET_SHINY  = 0x4,
};

class AdvControlsWidget;
class AbstractController : public QFrame
{
    friend class WidgetListing; /* For ToolBar Edition HACKS */

    Q_OBJECT
public:
    AbstractController( intf_thread_t  *_p_i, QWidget *_parent = 0 );

protected:
    intf_thread_t       *p_intf;

    QSignalMapper       *toolbarActionsMapper;
    QBoxLayout          *controlLayout;
    /* Change to BoxLayout if both dir are needed */

    AdvControlsWidget   *advControls;

    void parseAndCreate( const QString& config, QBoxLayout *controlLayout );

    virtual void createAndAddWidget( QBoxLayout *controlLayout, int i_index,
                                     buttonType_e i_type, int i_option );

    QWidget *createWidget( buttonType_e, int options = WIDGET_NORMAL );
private:
    static void setupButton( QAbstractButton * );
    QFrame *discFrame();
    QFrame *telexFrame();
    void applyAttributes( QToolButton *, bool b_flat, bool b_big );

    QHBoxLayout         *buttonGroupLayout;
protected slots:
    virtual void setStatus( int );

signals:
    void inputExists( bool ); /// This might be useful in the IM ?
    void inputPlaying( bool ); /// This might be useful in the IM ?
    void inputIsRecordable( bool ); /// same ?
    void inputIsTrickPlayable( bool ); /// same ?
};

/* Advanced Button Bar */
class AdvControlsWidget : public AbstractController
{
    Q_OBJECT
public:
    AdvControlsWidget( intf_thread_t *, QWidget *_parent = 0 );
};

/* Slider Bar */
class InputControlsWidget : public AbstractController
{
    Q_OBJECT
public:
    InputControlsWidget( intf_thread_t * , QWidget *_parent = 0 );
};

/* Button Bar */
class ControlsWidget : public AbstractController
{
    Q_OBJECT
public:
    /* p_intf, advanced control visible or not, blingbling or not */
    ControlsWidget( intf_thread_t *_p_i, bool b_advControls,
                    QWidget *_parent = 0 );

    void setGripVisible( bool b_visible )
    { grip->setVisible( b_visible ); }

protected:
    friend class MainInterface;

    bool b_advancedVisible;

private:
    QSizeGrip *grip;

protected slots:
    void toggleAdvanced();

signals:
    void advancedControlsToggled( bool );
};

/* Default value of opacity for FS controller */
#define DEFAULT_OPACITY 0.70

/* Used to restore the minimum width after a full-width switch */
#define FSC_WIDTH 800

/***********************************
 * Fullscreen controller
 ***********************************/
class FullscreenModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isVisiable READ getIsVisiable WRITE setIsVisiable NOTIFY isVisiableChanged)
    Q_PROPERTY(bool isFullscreen READ getIsFullscreen WRITE setIsFullscreen NOTIFY isFullscreenChanged)
public:
    bool getIsVisiable() const { return b_visiable; }
    void setIsVisiable(bool b) { b_visiable = b; emit isVisiableChanged(); }
    bool getIsFullscreen() const { return b_fullscreen; }
    void setIsFullscreen(bool b) { b_fullscreen; emit isFullscreenChanged(); }

    FullscreenModel( intf_thread_t *, QWidget *_parent = 0, VideoWidget* vw = NULL );
    virtual ~FullscreenModel();

    /* Vout */
    void fullscreenChanged( vout_thread_t *, bool b_fs, int i_timeout );

    void planHideFSC();
    Q_INVOKABLE void enterEvent();
    Q_INVOKABLE void leaveEvent();
    Q_INVOKABLE void moveEvent();

private:
    static int FullscreenChanged( vlc_object_t *obj,
                    const char *, vlc_value_t, vlc_value_t new_val, void *data );

signals:
    void fullscreenChanged( bool );
    void keyPressed( QKeyEvent * );
    void isVisiableChanged();
    void isFullscreenChanged();

public slots:
    void setVoutList( vout_thread_t **, int );
    void hideFSC();

protected:
    friend class MainInterface;

private:
    QTimer *p_hideTimer;

    intf_thread_t *p_intf;
    VideoWidget* m_videoWidget;
    bool b_visiable;
    bool b_mouse_over;
    int i_screennumber;

    /* List of vouts currently tracked */
    QList<vout_thread_t *> vout;

    /* Shared variable between FSC and VLC (protected by a lock) */
    vlc_mutex_t lock;
    bool        b_fullscreen;
    int         i_hide_timeout;  /* FSC hiding timeout, same as mouse hiding timeout */

#ifdef QT5_HAS_WAYLAND
    bool b_hasWayland;
#endif
};

#endif
