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

#include "open_wallet_thread.h"
/*
 * adding libsecret header file together with C++ header files doesnt seem to work.
 * as a workaround,a static library that interfaces with libsecret is used and a "pure" C interface of the
 * static library is then used in C++
 */
extern "C" {
char * lxqt_secret_service_get_value( const char * key,const void * ) ;
int lxqt_secret_service_password_store_sync( const char * key,const char * value,const void *,const void * ) ;
int lxqt_secret_service_clear_sync( const char * key,const void *,const void * ) ;
char ** lxqt_secret_get_all_keys( const void *,const void *,int * count ) ;
int lxqt_secret_service_wallet_size( const void * ) ;
int lxqt_secret_service_wallet_is_open( const void * ) ;
void * lxqt_secret_service_create_schema( const char * schemaName ) ;
void * lxqt_secret_service_create_schema_1( const char * schemaName ) ;
}

lxqt::Wallet::secretService::secretService()
{
}

lxqt::Wallet::secretService::~secretService()
{
	free( m_schema ) ;
	free( m_schema_1 ) ;
}

bool lxqt::Wallet::secretService::addKey( const QString& key,const QByteArray& value )
{
	if( key.isEmpty() ){
		return false ;
	}else{
		lxqt_secret_service_password_store_sync( key.toAscii().constBegin(),value.constData(),m_schema,m_schema_1 ) ;
		return true ;
	}
}

bool lxqt::Wallet::secretService::open( const QString& walletName,const QString& applicationName,const QString& password )
{
	m_password  = password ;

	if( applicationName.isEmpty() ){

		m_byteArrayWalletName      = walletName.toAscii() ;
		m_byteArrayApplicationName = walletName.toAscii() ;

		m_walletName        = m_byteArrayWalletName.constData() ;
		m_applicationName   = m_byteArrayApplicationName.constData() ;

		m_byteArraySchemaName = QString( "lxqt.Wallet.%1.%2" ).arg( walletName ).arg( walletName ).toAscii() ;
	}else{
		m_byteArrayWalletName      = walletName.toAscii() ;
		m_byteArrayApplicationName = applicationName.toAscii() ;

		m_walletName        = m_byteArrayWalletName.constData() ;
		m_applicationName   = m_byteArrayApplicationName.constData() ;

		m_byteArraySchemaName = QString( "lxqt.Wallet.%1.%2" ).arg( walletName ).arg( applicationName ).toAscii() ;
	}


	m_schema   = lxqt_secret_service_create_schema( m_byteArraySchemaName.constData() ) ;
	m_schema_1 = lxqt_secret_service_create_schema_1( m_byteArraySchemaName.constData() ) ;

	connect( this,SIGNAL( walletIsOpen( bool ) ),m_interfaceObject,SLOT( walletIsOpen( bool ) ) ) ;

	openWalletThread * t = new openWalletThread( lxqt_secret_service_wallet_is_open,m_schema ) ;

	if( t ){
		connect( t,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;
		t->start( openWalletThread::openSecretService ) ;
	}else{
		this->walletOpened( false ) ;
	}

	return false ;
}

void lxqt::Wallet::secretService::walletOpened( bool opened )
{
	emit walletIsOpen( opened ) ;
}

QByteArray lxqt::Wallet::secretService::readValue( const QString& key )
{
	QByteArray r ;
	char * e = lxqt_secret_service_get_value( key.toAscii().constData(),m_schema ) ;
	if( e ){
		r = QByteArray( e ) ;
		free( e ) ;
	}
	return r ;
}

QVector<lxqt::Wallet::walletKeyValues> lxqt::Wallet::secretService::readAllKeyValues( void )
{
	QVector<lxqt::Wallet::walletKeyValues> p ;
	lxqt::Wallet::walletKeyValues q ;
	QStringList l = this->readAllKeys() ;
	int k = l.size() ;
	for( int i = 0 ; i < k ; i++ ){
		q.key = l.at( i ) ;
		q.value = this->readValue( l.at( i ) ) ;
		p.append( q ) ;
	}
	return p ;
}

QStringList lxqt::Wallet::secretService::readAllKeys( void )
{
	int count ;
	QStringList l ;
	char ** c = lxqt_secret_get_all_keys( m_schema,m_schema_1,&count ) ;
	if( c ){
		for( int i = 0 ; i < count ; i++ ){
			l.append( QString( c[ i ] ) ) ;
			free( c[ i ] ) ;
		}
		free( c ) ;
	}
	return l ;
}

void lxqt::Wallet::secretService::deleteKey( const QString& key )
{
	if( !key.isEmpty() ){
		lxqt_secret_service_clear_sync( key.toAscii().constData(),m_schema,m_schema_1 ) ;
	}
}

int lxqt::Wallet::secretService::walletSize( void )
{
	return lxqt_secret_service_wallet_size( m_schema ) ;
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
	return lxqt_secret_service_wallet_is_open( m_schema ) ;
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

