/*****************************************************************************
 * about.cpp : About dialog
 ****************************************************************************
 * Copyright (C) 2007 the VideoLAN team
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qt.hpp"
#include "dialogs/about.hpp"
#include "util/qt_dirs.hpp"

#include <vlc_about.h>
#include <vlc_intf_strings.h>

#ifdef UPDATE_CHECK
# include <vlc_update.h>
#endif

#include <QQuickView>

#include <QQmlComponent> // for accessing QML from C++
#include <QQmlContext>   // for accessing C++ from QML

#include <assert.h>

AboutDialog::AboutDialog( intf_thread_t *_p_intf)
{
    /* Add rootContext variables */
    view.rootContext()->setContextProperty( "versionText", qfu( " " VERSION_MESSAGE ) );
    view.rootContext()->setContextProperty( "buildinfoText", qfu( VLC_CompileBy() )+"@"+qfu( VLC_CompileHost() )+" "+__DATE__+" "+__TIME__ );
    view.rootContext()->setContextProperty( "mainBlablaText",
            "<html><head/><body>" + qtr( "<p>VLC media player is a free and open source media player, encoder, and streamer made by the volunteers of the "
                                         "<a href=\"http://www.videolan.org/\"><span style=\" text-decoration: underline; color:#0057ae;\">VideoLAN</span></a> "
                                         "community.</p><p>VLC uses its internal codecs, works on essentially every popular platform, and can read almost all "
                                         "files, CDs, DVDs, network streams, capture cards and other media formats!</p><p><a href=\"http://www.videolan.org/contribute/\">"
                                         "<span style=\" text-decoration: underline; color:#0057ae;\">Help and join us!</span></a>" ) + "</p></body> </html>" );
    view.rootContext()->setContextProperty( "authorsText", "<html><head/><body><p><span style=\" text-decoration: underline; color:#0057ae;\">"+
                                            qtr( "Authors" )+"</span></p></body></html>" );
    view.rootContext()->setContextProperty( "licenseText", "<html><head/><body><p><span style=\" text-decoration: underline; color:#0057ae;\">"+
                                            qtr( "License" )+"</span></p></body></html>" );
    view.rootContext()->setContextProperty( "creditsText", "<html><head/><body><p><span style=\" text-decoration: underline; color:#0057ae;\">"+
                                            qtr( "Credits" )+"</span></p></body></html>" );
    view.rootContext()->setContextProperty( "authorsBlablaText", qfu( psz_authors ) );
    view.rootContext()->setContextProperty( "licenseBlablaText", qfu( psz_license ) );
    view.rootContext()->setContextProperty( "creditsBlablaText", qfu( psz_thanks ) );
    view.rootContext()->setContextProperty( "titleText", "<html><head/><body><p><span style=\" font-size:26pt; color:#353535;\"> " + qtr( "VLC media player" ) + " </span></p></body></html>");
#if 0
    if( QDate::currentDate().dayOfYear() >= QT_XMAS_JOKE_DAY && var_InheritBool( p_intf, "qt-icon-change" ) )
        view.rootContext()->setContextProperty( "iconVLC", ":/logo/vlc128-xmas.png" );
    else
        view.rootContext()->setContextProperty( "iconVLC", ":/logo/vlc128.png" );
#endif

    /* Setup of the window */
    view.setResizeMode( QQuickView::SizeRootObjectToView );
    view.setTitle( qtr( "About" ) );
    view.setSource( QUrl::fromLocalFile(":/qml/about") );

}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::show()
{
    view.show();
}

void AboutDialog::hide()
{
    view.hide();
}

#ifdef UPDATE_CHECK

/*****************************************************************************
 * UpdateDialog
 *****************************************************************************/
/* callback to get information from the core */
static void UpdateCallback( void *data, bool b_ret )
{
    UpdateDialog* UDialog = (UpdateDialog *)data;
    QEvent* event;

    if( b_ret )
        event = new QEvent( UpdateDialog::UDOkEvent );
    else
        event = new QEvent( UpdateDialog::UDErrorEvent );

    QApplication::postEvent( UDialog, event );
}

const QEvent::Type UpdateDialog::UDOkEvent =
        (QEvent::Type)QEvent::registerEventType();
const QEvent::Type UpdateDialog::UDErrorEvent =
        (QEvent::Type)QEvent::registerEventType();

UpdateDialog::UpdateDialog( intf_thread_t *_p_intf ) : QVLCFrame( _p_intf )
{
    /* build Ui */
    ui.setupUi( this );
    ui.updateDialogButtonBox->addButton( new QPushButton( qtr("&Close"), this ),
                                         QDialogButtonBox::RejectRole );
    QPushButton *recheckButton = new QPushButton( qtr("&Recheck version"), this );
    ui.updateDialogButtonBox->addButton( recheckButton, QDialogButtonBox::ActionRole );

    ui.updateNotifyButtonBox->addButton( new QPushButton( qtr("&Yes"), this ),
                                         QDialogButtonBox::AcceptRole );
    ui.updateNotifyButtonBox->addButton( new QPushButton( qtr("&No"), this ),
                                         QDialogButtonBox::RejectRole );

    setWindowTitle( qtr( "VLC media player updates" ) );
    setWindowRole( "vlc-update" );

    BUTTONACT( recheckButton, UpdateOrDownload() );
    CONNECT( ui.updateDialogButtonBox, rejected(), this, close() );

    CONNECT( ui.updateNotifyButtonBox, accepted(), this, UpdateOrDownload() );
    CONNECT( ui.updateNotifyButtonBox, rejected(), this, close() );

    /* Create the update structure */
    p_update = update_New( p_intf );
    b_checked = false;

    setMinimumSize( 300, 300 );
    setMaximumSize( 500, 300 );

    restoreWidgetPosition( "Update", maximumSize() );

    /* Check for updates */
    UpdateOrDownload();
}

UpdateDialog::~UpdateDialog()
{
    update_Delete( p_update );
    saveWidgetPosition( "Update" );
}

/* Check for updates */
void UpdateDialog::UpdateOrDownload()
{
    if( !b_checked )
    {
        ui.stackedWidget->setCurrentWidget( ui.updateRequestPage );
        update_Check( p_update, UpdateCallback, this );
    }
    else
    {
        QString dest_dir = QDir::tempPath();
        if( !dest_dir.isEmpty() )
        {
            dest_dir = toNativeSepNoSlash( dest_dir ) + DIR_SEP;
            msg_Dbg( p_intf, "Downloading to folder: %s", qtu( dest_dir ) );
            toggleVisible();
            update_Download( p_update, qtu( dest_dir ) );
            /* FIXME: We should trigger a change to another dialog here ! */
        }
    }
}

/* Handle the events */
void UpdateDialog::customEvent( QEvent *event )
{
    if( event->type() == UDOkEvent )
        updateNotify( true );
    else
        updateNotify( false );
}

* Notify the end of the update_Check */
void UpdateDialog::updateNotify( bool b_result )
{
    /* The update finish without errors */
    if( b_result )
    {
        if( update_NeedUpgrade( p_update ) )
        {
            ui.stackedWidget->setCurrentWidget( ui.updateNotifyPage );
            update_release_t *p_release = update_GetRelease( p_update );
            assert( p_release );
            b_checked = true;
            QString message = QString(
                    qtr( "A new version of VLC (%1.%2.%3%4) is available." ) )
                .arg( QString::number( p_release->i_major ) )
                .arg( QString::number( p_release->i_minor ) )
                .arg( QString::number( p_release->i_revision ) )
                .arg( p_release->i_extra == 0 ? "" : "." + QString::number( p_release->i_extra ) );

            ui.updateNotifyLabel->setText( message );
            message = qfu( p_release->psz_desc ).replace( "\n", "<br/>" );

            /* Try to highlight releases featuring security changes */
            int i_index = message.indexOf( "security", Qt::CaseInsensitive );
            if ( i_index >= 0 )
            {
                message.insert( i_index + 8, "</font>" );
                message.insert( i_index, "<font style=\"color:red\">" );
            }
            ui.updateNotifyTextEdit->setHtml( message );

            /* Force the dialog to be shown */
            this->show();
        }
        else
        {
            ui.stackedWidget->setCurrentWidget( ui.updateDialogPage );
            ui.updateDialogLabel->setText(
                    qtr( "You have the latest version of VLC media player." ) );
        }
    }
    else
    {
        ui.stackedWidget->setCurrentWidget( ui.updateDialogPage );
        ui.updateDialogLabel->setText(
                    qtr( "An error occurred while checking for updates..." ) );
    }
}

#endif
