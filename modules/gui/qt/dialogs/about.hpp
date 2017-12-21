/*****************************************************************************
 * about.hpp : About dialog
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

#ifndef QVLC_ABOUT_DIALOG_H_
#define QVLC_ABOUT_DIALOG_H_ 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "qt.hpp"

#include "util/qvlcframe.hpp"
#include "util/singleton.hpp"

#include <QQuickView>

class QEvent;

class AboutDialog : public QObject, public Singleton<AboutDialog>
{
    Q_OBJECT
private:
    AboutDialog( intf_thread_t * );
    ~AboutDialog();
    QQuickView view;

public slots:
    friend class    Singleton<AboutDialog>;

public :
    void show();
    Q_INVOKABLE void hide();

/*protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent ( QShowEvent * ) Q_DECL_OVERRIDE;*/

/*private:
    bool b_advanced;*/

/*private slots:
    void showLicense();
    void showAuthors();
    void showCredit();*/
};

#ifdef UPDATE_CHECK

class UpdateDialog : public QVLCFrame, public Singleton<UpdateDialog>
{
    Q_OBJECT
public:
    static const QEvent::Type UDOkEvent;
    static const QEvent::Type UDErrorEvent;
    void updateNotify( bool );

private:
    UpdateDialog( intf_thread_t * );
    virtual ~UpdateDialog();

    Ui::updateWidget ui;
    update_t *p_update;
    void customEvent( QEvent * );
    bool b_checked;

private slots:
    void close() Q_DECL_OVERRIDE { toggleVisible(); }

    void UpdateOrDownload();

    friend class    Singleton<UpdateDialog>;
};
#endif

#endif
