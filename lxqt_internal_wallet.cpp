#include "lxqt_internal_wallet.h"

bool lxqt::Wallet::internalWallet::addKey( const QString& key,const QByteArray& value )
{
	Q_UNUSED( key ) ;
	Q_UNUSED( value ) ;

	return true ;
}

bool lxqt::Wallet::internalWallet::open( const QString& walletName,const QString applicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	return true ;
}

QByteArray lxqt::Wallet::internalWallet::readValue( const QString& key )
{
	Q_UNUSED( key ) ;
	QByteArray b ;
	return b ;
}

QVector<lxqt::Wallet::walletKeyValues> lxqt::Wallet::internalWallet::readAllKeyValues( void )
{
	QVector<walletKeyValues> w ;
	return w ;
}

void lxqt::Wallet::internalWallet::deleteKey( const QString& key )
{
	Q_UNUSED( key ) ;
}

void lxqt::Wallet::internalWallet::deleteWallet( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
}

bool lxqt::Wallet::internalWallet::walletExists( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	return false ;
}

int lxqt::Wallet::internalWallet::walletSize( void )
{
	return 4 ;
}
