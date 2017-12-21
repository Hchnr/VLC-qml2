/*****************************************************************************
 * help.cpp : Help dialog
 ****************************************************************************
 * Copyright (C) 2007 the VideoLAN team
 * $Id$
 *
 * Authors: Jean-Baptiste Kempf <jb (at) videolan.org>
 *          Rémi Duraffort <ivoire (at) via.ecp.fr>
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
#include "dialogs/help.hpp"
#include "util/qt_dirs.hpp"

#include <vlc_intf_strings.h>

#ifdef UPDATE_CHECK
# include <vlc_update.h>
#endif

#include <QQuickView>

#include <QQmlComponent> // for accessing QML from C++
#include <QQmlContext>   // for accessing C++ from QML

#include <assert.h>

HelpDialog::HelpDialog( intf_thread_t *_p_intf )
{
    view.rootContext()->setContextProperty( "helpText", qtr(I_LONGHELP) );
    view.rootContext()->setContextProperty( "dialog", this );
    view.setResizeMode( QQuickView::SizeRootObjectToView) ;
    view.setTitle( qtr( "Help" ) );
    view.setSource( QUrl::fromLocalFile(":/qml/help") );

}

HelpDialog::~HelpDialog()
{ }

void HelpDialog::show()
{
    view.show();
}

void HelpDialog::hide()
{
    view.hide();
}

