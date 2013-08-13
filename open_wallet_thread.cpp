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

#include "open_wallet_thread.h"

openWalletThread::openWalletThread( lxqt_wallet_t * wallet,QString password,QString walletName,QString applicationName )
{
	m_wallet          = wallet ;
	m_password        = password ;
	m_walletName      = walletName ;
	m_applicationName = applicationName ;
}

void openWalletThread::start()
{
	QThreadPool::globalInstance()->start( this ) ;
}

void openWalletThread::run()
{
	lxqt_wallet_error r = lxqt_wallet_open( m_wallet,m_password.toAscii().constData(),m_password.size(),
				     m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
	emit walletOpened( r == lxqt_wallet_no_error ) ;
}
