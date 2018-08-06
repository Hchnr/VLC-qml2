/*****************************************************************************
 * interface_widgets.hpp : Custom widgets for the main interface
 ****************************************************************************
 * Copyright (C) 2006-2008 the VideoLAN team
 * $Id$
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *          Rafaël Carré <funman@videolanorg>
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

#ifndef VLC_QT_INTERFACE_WIDGETS_HPP_
#define VLC_QT_INTERFACE_WIDGETS_HPP_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "main_interface.hpp" /* Interface integration */
#include "input_manager.hpp"  /* Speed control */

#include "components/controller.hpp"
#include "components/controller_widget.hpp"
#include "dialogs_provider.hpp"
#include "components/info_panels.hpp"
#include "util/input_slider.hpp"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QQmlContext>
#include <QLinkedList>

class QMenu;
class QSlider;
class QTimer;
class QWidgetAction;
class SpeedControlWidget;
struct vout_window_t;

/******************** Video Widget ****************/
class VideoWidget : public QFrame
{
    Q_OBJECT
public:
    VideoWidget( intf_thread_t *, QWidget* p_parent );
    virtual ~VideoWidget();

    bool request( struct vout_window_t * );
    void release( void );
    void sync( void );

protected:
    QPaintEngine *paintEngine() const Q_DECL_OVERRIDE
    {
        return NULL;
    }

    bool nativeEvent(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    QSize physicalSize() const;

private:
    int qtMouseButton2VLC( Qt::MouseButton );
    intf_thread_t *p_intf;
    vout_window_t *p_window;

    QWidget *stable;
    QLayout *layout;
    QTimer *cursorTimer;
    int cursorTimeout;

    void reportSize();
    void showCursor();

signals:
    void sizeChanged( int, int );

public slots:
    void setSize( unsigned int, unsigned int );

private slots:
    void hideCursor();
};

/******************** Background Widget ****************/
class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    BackgroundWidget( intf_thread_t * );
    void setExpandstoHeight( bool b_expand ) { b_expandPixmap = b_expand; }
    void setWithArt( bool b_withart_ ) { b_withart = b_withart_; };
private:
    intf_thread_t *p_intf;
    QString pixmapUrl;
    bool b_expandPixmap;
    bool b_withart;
    QPropertyAnimation *fadeAnimation;
    void contextMenuEvent( QContextMenuEvent *event ) Q_DECL_OVERRIDE;
protected:
    void paintEvent( QPaintEvent *e ) Q_DECL_OVERRIDE;
    void showEvent( QShowEvent * e ) Q_DECL_OVERRIDE;
    void updateDefaultArt( const QString& );
    static const int MARGIN = 5;
    QString defaultArt;
public slots:
    void toggle(){ isVisible() ? hide() : show(); }
    void updateArt( const QString& );
    void titleUpdated( const QString& );
};

class EasterEggBackgroundWidget : public BackgroundWidget
{
    Q_OBJECT

public:
    EasterEggBackgroundWidget( intf_thread_t * );
    virtual ~EasterEggBackgroundWidget();

public slots:
    void animate();

protected:
    void paintEvent( QPaintEvent *e ) Q_DECL_OVERRIDE;
    void showEvent( QShowEvent *e ) Q_DECL_OVERRIDE;
    void hideEvent( QHideEvent * ) Q_DECL_OVERRIDE;
    void resizeEvent( QResizeEvent * ) Q_DECL_OVERRIDE;

private slots:
    void spawnFlakes();
    void reset();

private:
    struct flake
    {
        QPoint point;
        bool b_fat;
    };
    QTimer *timer;
    QLinkedList<flake *> *flakes;
    int i_rate;
    int i_speed;
    bool b_enabled;
    static const int MAX_FLAKES = 1000;
};

class ClickableQLabel : public QLabel
{
    Q_OBJECT
public:
    void mouseDoubleClickEvent( QMouseEvent *event ) Q_DECL_OVERRIDE
    {
        Q_UNUSED( event );
        emit doubleClicked();
    }
signals:
    void doubleClicked();
};

class TimeLabel : public ClickableQLabel
{
    Q_OBJECT
public:
    enum Display
    {
        Elapsed,
        Remaining,
        Both
    };

    TimeLabel( intf_thread_t *_p_intf, TimeLabel::Display _displayType = TimeLabel::Both );
protected:
    void mousePressEvent( QMouseEvent *event ) Q_DECL_OVERRIDE
    {
        if( displayType == TimeLabel::Elapsed ) return;
        toggleTimeDisplay();
        event->accept();
    }
    void mouseDoubleClickEvent( QMouseEvent *event ) Q_DECL_OVERRIDE
    {
        if( displayType != TimeLabel::Both ) return;
        event->accept();
        toggleTimeDisplay();
        ClickableQLabel::mouseDoubleClickEvent( event );
    }
private:
    intf_thread_t *p_intf;
    bool b_remainingTime;
    float cachedPos;
    int64_t cachedTime;
    int cachedLength;
    TimeLabel::Display displayType;

    char psz_length[MSTRTIME_MAX_SIZE];
    char psz_time[MSTRTIME_MAX_SIZE];
    void toggleTimeDisplay();
    void refresh();
private slots:
    void setRemainingTime( bool );
    void setDisplayPosition( float pos, int64_t time, int length );
    void setDisplayPosition( float pos );
signals:
    void broadcastRemainingTime( bool );
    /* for QML time label */
    // void settimelabel( TimeLabel::Display dis, QString tm);
    void settimelabel( QString display, QString name);
};

/* model for toolbutton of qml-controlbar */
class ToolButtonModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString widgetName READ getWidgetName NOTIFY widgetNameChanged)
    Q_PROPERTY( int buttonAction READ getButtonAction NOTIFY buttonActionChanged)
    Q_PROPERTY(bool checkable READ getCheckable NOTIFY checkableChanged)
    Q_PROPERTY(bool checked READ getChecked NOTIFY checkedChanged)
    Q_PROPERTY(QString imgSrc READ getImgSrc NOTIFY imgSrcChanged)
    Q_PROPERTY(QString tip READ getTip NOTIFY tipChanged)

public:
    ToolButtonModel(QString, intf_thread_t*, AbstractController*, ActionsManager*);

    QString getWidgetName() { return m_widgetName; }
    int getButtonAction() { return m_buttonAction; }
    bool getCheckable() { return m_checkable; }
    bool getChecked() { return m_checked; }
    QString getImgSrc() { return m_imgSrc; }
    QString getTip() { return m_tip; }

private:
    static QMap<QString, int> actionsMap;
    static QMap<QString, int> initActionsMap();
    static QMap<QString, bool> checkedMap;
    static QMap<QString, bool> initCheckedMap();
    void initCheckedMapFromSettings();
    static QMap<QString, bool> checkableMap;
    static QMap<QString, bool> initCheckableMap();
    static QMap<QString, QString> tipMap;
    static QMap<QString, QString> initTipMap();
    static QMap<QString, QString> imgSrcMap;
    static QMap<QString, QString> initImgSrcMap();

    static bool         isCheckedMapInit;
    intf_thread_t      *p_intf;
    QString             m_widgetName;
    int                 m_buttonAction;
    bool                m_checked;
    bool                m_checkable;
    QString             m_imgSrc;
    QString             m_tip;
    AbstractController *m_abstractController;
    ActionsManager     *m_actionsManager;

signals:
    void widgetNameChanged();
    void buttonActionChanged();
    void checkedChanged();
    void checkableChanged();
    void imgSrcChanged();
    void tipChanged();

public slots:
    void updateButtonPlay( bool );
    void updateButtonLoop();
};

/* model for time label of qml-controlbar */
class TimeLabelModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString strElapsed READ strElapsed WRITE setStrElapsed NOTIFY strElapsedChanged)
    Q_PROPERTY(QString strRemaining READ strRemaining WRITE setStrRemaining NOTIFY strRemainingChanged)
    Q_PROPERTY(QString strBoth READ strBoth WRITE setStrBoth NOTIFY strBothChanged)
public:
    enum Display
    {
        Elapsed,
        Remaining,
        Both
    };
    Q_ENUMS(Display)
    static void declareQML() {
        qmlRegisterType<TimeLabelModel>("CommonParameter",1,0,"CommonParameter");
    }
    TimeLabelModel( intf_thread_t *_p_intf, TimeLabelModel::Display _displayType = TimeLabelModel::Both );
    TimeLabelModel() {}
    QString strElapsed() const { return _strElapsed; }
    void setStrElapsed(QString l) { _strElapsed = l; emit strElapsedChanged(); }
    QString strRemaining() const { return _strRemaining; }
    void setStrRemaining(QString t) { _strRemaining = t; emit strRemainingChanged(); }
    QString strBoth() const { return _strBoth; }
    void setStrBoth(QString t) { _strBoth = t; emit strBothChanged(); }

    Q_INVOKABLE void toggleTimeDisplay();

signals:
    void strElapsedChanged();
    void strRemainingChanged();
    void strBothChanged();

private:
    intf_thread_t *p_intf;
    bool b_remainingTime;
    float cachedPos;
    int64_t cachedTime;
    int cachedLength;
    TimeLabelModel::Display displayType;

    char psz_length[MSTRTIME_MAX_SIZE];
    char psz_time[MSTRTIME_MAX_SIZE];
    QString _strElapsed;
    QString _strRemaining;
    QString _strBoth;

private slots:
    void setRemainingTime( bool );
    void setDisplayPosition( float pos, int64_t time, int length );
    void setDisplayPosition( float pos );

signals:
    void broadcastRemainingTime( bool );

};

/* all models needed in QML context */
class ToolbarInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TimeLabelModel* LabelElapsed READ getLabelElapsed WRITE setLabelElapsed NOTIFY LabelElapsedChanged)
    Q_PROPERTY(TimeLabelModel* LabelRemaining READ getLabelRemaining WRITE setLabelRemaining NOTIFY LabelRemainingChanged)
    Q_PROPERTY(QList<QObject*> LeftList READ getLeftList WRITE setLeftList NOTIFY LeftListChanged)
    Q_PROPERTY(QList<QObject*> CenterList READ getCenterList WRITE setCenterList NOTIFY CenterListChanged)
    Q_PROPERTY(QList<QObject*> RightList READ getRightList WRITE setRightList NOTIFY RightListChanged)
    Q_PROPERTY(SoundWidgetModel* VolumeModel READ getVolumeModel WRITE setVolumeModel NOTIFY VolumeModelChanged)
    Q_PROPERTY(SeekSliderModel* SeekSlider READ getSeekSlider WRITE setSeekSlider NOTIFY SeekSliderChanged)

private:
    AbstractController* m_controller;

public:
    intf_thread_t*      p_intf;
    TimeLabelModel*     m_labelElapsed;
    TimeLabelModel*     m_labelRemaining;
    ActionsManager*     m_actionsManager;
    SoundWidgetModel*   m_volumeModel;
    SeekSliderModel*    m_seekSlider;
    QList<QObject*>     m_leftToolbarList;
    QList<QObject*>     m_centerToolbarList;
    QList<QObject*>     m_rightToolbarList;

    TimeLabelModel* getLabelElapsed() const { return m_labelElapsed; }
    TimeLabelModel* getLabelRemaining() const { return m_labelRemaining; }
    QList<QObject*> getLeftList() const { return m_leftToolbarList; }
    QList<QObject*> getCenterList() const { return m_centerToolbarList; }
    QList<QObject*> getRightList() const { return m_rightToolbarList; }
    SoundWidgetModel* getVolumeModel() const { return m_volumeModel; }
    SeekSliderModel*  getSeekSlider() const { return m_seekSlider; }

    void setLabelElapsed( TimeLabelModel* t) {}
    void setLabelRemaining( TimeLabelModel* t) {}
    void setLeftList( QList<QObject*> l ) {}
    void setCenterList( QList<QObject*> l ) {}
    void setRightList( QList<QObject*> l ) {}
    void setVolumeModel( SoundWidgetModel* m) {}
    void setSeekSlider( SeekSliderModel* m) {}

    ToolbarInformation(intf_thread_t* _p_intf);
    Q_INVOKABLE void doAction(int a) { m_actionsManager->doAction(a); }

signals:
    void LabelElapsedChanged();
    void LabelRemainingChanged();
    void LeftListChanged();
    void CenterListChanged();
    void RightListChanged();
    void VolumeModelChanged();
    void SeekSliderChanged();
    void playingStatusChanged();
};

class SpeedLabel : public QLabel
{
    Q_OBJECT
public:
    SpeedLabel( intf_thread_t *, QWidget * );
    virtual ~SpeedLabel();

protected:
    void mousePressEvent ( QMouseEvent * event ) Q_DECL_OVERRIDE
    {
        showSpeedMenu( event->pos() );
    }
private slots:
    void showSpeedMenu( QPoint );
    void setRate( float );
private:
    intf_thread_t *p_intf;
    QMenu *speedControlMenu;
    QString tooltipStringPattern;
    SpeedControlWidget *speedControl;
    QWidgetAction *widgetAction;
};

/******************** Speed Control Widgets ****************/
class SpeedControlWidget : public QFrame
{
    Q_OBJECT
public:
    SpeedControlWidget( intf_thread_t *, QWidget * );
    void updateControls( float );
private:
    intf_thread_t* p_intf;
    QSlider* speedSlider;
    QDoubleSpinBox* spinBox;
    int lastValue;

public slots:
    void activateOnState();

private slots:
    void updateRate( int );
    void updateSpinBoxRate( double );
    void resetRate();
};

class CoverArtLabel : public QLabel
{
    Q_OBJECT
public:
    CoverArtLabel( QWidget *parent, intf_thread_t * );
    void setItem( input_item_t * );
    virtual ~CoverArtLabel();

protected:
    void mouseDoubleClickEvent( QMouseEvent *event ) Q_DECL_OVERRIDE
    {
        if( ! p_item && qobject_cast<MetaPanel *>(this->window()) == NULL )
        {
            THEDP->mediaInfoDialog();
        }
        event->accept();
    }
private:
    intf_thread_t *p_intf;
    input_item_t *p_item;

public slots:
    void showArtUpdate( const QString& );
    void showArtUpdate( input_item_t * );
    void askForUpdate();
    void setArtFromFile();
    void clear();
};

#endif
