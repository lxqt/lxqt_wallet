/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lxqt_secret_service.h"

lxqt::Wallet::secretService::secretService()
{

}

lxqt::Wallet::secretService::~secretService()
{
	;
}

bool lxqt::Wallet::secretService::addKey( const QString& key,const QByteArray& value )
{
	lxqt_secret_service_password_store_sync( key.toAscii().constBegin(),value.constData(),
						 m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
	return true ;
}

bool lxqt::Wallet::secretService::open( const QString& walletName,const QString& applicationName,const QString& password )
{
	m_walletName        = walletName ;
	m_applicationName   = applicationName ;
	m_password          = password ;
	connect( this,SIGNAL( walletIsOpen( bool ) ),m_interfaceObject,SLOT( walletIsOpen( bool ) ) ) ;
	this->walletOpened( true ) ;
	return false ;
}

void lxqt::Wallet::secretService::walletOpened( bool opened )
{
	emit walletIsOpen( opened ) ;
}

QByteArray lxqt::Wallet::secretService::readValue( const QString& key )
{
	QByteArray r ;
	char * e = lxqt_secret_service_get_value( key.toAscii().constData(),
						  m_walletName.toAscii().constEnd(),m_applicationName.toAscii().constData() ) ;
	if( e ){
		r = QByteArray( e ) ;
		free( e ) ;
	}
	return r ;
}

QVector<lxqt::Wallet::walletKeyValues> lxqt::Wallet::secretService::readAllKeyValues( void )
{
	QVector<lxqt::Wallet::walletKeyValues> p ;

	return p ;
}

QStringList lxqt::Wallet::secretService::readAllKeys( void )
{
	return QStringList() ;
}

void lxqt::Wallet::secretService::deleteKey( const QString& key )
{
	lxqt_secret_service_clear_sync( key.toAscii().constData(),
					m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
}

int lxqt::Wallet::secretService::walletSize( void )
{
	QStringList l ;
	return l.size() ;
}

void lxqt::Wallet::secretService::closeWallet( bool b )
{
	Q_UNUSED( b ) ;
}

lxqt::Wallet::walletBackEnd lxqt::Wallet::secretService::backEnd( void )
{
	return lxqt::Wallet::secretServiceBackEnd ;
}

bool lxqt::Wallet::secretService::walletIsOpened( void )
{
	return true ;
}

void lxqt::Wallet::secretService::setInterfaceObject( QObject * interfaceObject )
{
	m_interfaceObject = interfaceObject ;
}

QObject * lxqt::Wallet::secretService::qObject( void )
{
	return static_cast< QObject * >( this ) ;
}

QString lxqt::Wallet::secretService::storagePath()
{
	return QString() ;
}

void lxqt::Wallet::secretService::changeWalletPassWord( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( applicationName ) ;
	Q_UNUSED( walletName ) ;
}

QStringList lxqt::Wallet::secretService::managedWalletList()
{
	return QStringList() ;
}

