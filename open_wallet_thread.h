/***********************************************************************************
 *   Copyright 2013 by mhogomchungu <mhogomchungu@gmail.com>		           *
 *                                                                                 *
 *                                                                                 *
 *   This library is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public                    *
 *   License as published by the Free Software Foundation; either                  *
 *   version 2.1 of the License, or (at your option) any later version.            *
 *                                                                                 *
 *   This library is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 *   Lesser General Public License for more details.                               *
 *                                                                                 *
 *   You should have received a copy of the GNU Lesser General Public              *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 ***********************************************************************************/

#ifndef OPEN_WALLET_THREAD_H
#define OPEN_WALLET_THREAD_H

#include <QRunnable>
#include <QThreadPool>
#include <QObject>
#include <QString>
#include <QStringList>

#include "lxqtwallet.h"

class openWalletThread : public QObject,public QRunnable
{
	Q_OBJECT
public:
	openWalletThread( lxqt_wallet_t * wallet,QString password,QString walletName,QString applicationName ) ;
	void start( void ) ;
signals:
	void walletOpened( bool ) ;
private:
	void run( void ) ;
	lxqt_wallet_t * m_wallet ;
	QString m_password ;
	QString m_walletName ;
	QString m_applicationName ;
};

#endif // OPEN_WALLET_THREAD_H
