#ifndef LXQT_WALLET_MANAGER_H
#define LXQT_WALLET_MANAGER_H

#include "lxqt_wallet_interface.h"

namespace lxqt{

class lxqtWallet : public lxqt::Wallet
{
public:
	bool addKey( const QString& key,const QByteArray& value ) ;
	bool open( const QString& walletName,const QString applicationName ) ;
	QByteArray readValue( const QString& key ) ;
	QVector<walletKeyValues> readAllValues( void ) ;
	void deleteKey( const QString& key ) ;
	void deleteWallet( const QString& walletName,const QString& applicationName )  ;
	void walletExists( const QString& walletName,const QString& applicationName ) ;
	int walletSize( void )  ;
};

}

#endif // LXQT_WALLET_MANAGER_H
