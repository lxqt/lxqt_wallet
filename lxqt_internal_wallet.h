#ifndef LXQT_INTERNAL_WALLET_H
#define LXQT_INTERNAL_WALLET_H

#include "lxqt_wallet_interface.h"

namespace lxqt{

namespace Wallet{
class internalWallet : public lxqt::Wallet::Wallet
{
public:
	bool addKey( const QString& key,const QByteArray& value ) ;
	bool open( const QString& walletName,const QString applicationName ) ;
	QByteArray readValue( const QString& key ) ;
	QVector<lxqt::Wallet::walletKeyValues> readAllKeyValues( void ) ;
	void deleteKey( const QString& key ) ;
	void deleteWallet( const QString& walletName,const QString& applicationName )  ;
	bool walletExists( const QString& walletName,const QString& applicationName ) ;
	int walletSize( void )  ;
};

}

}
#endif // LXQT_INTERNAL_WALLET_H
