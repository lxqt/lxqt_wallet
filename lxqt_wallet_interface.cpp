
#include "lxqt_wallet_interface.h"

#include "lxqt_internal_wallet.h"

lxqt::Wallet::Wallet::Wallet()
{

}

lxqt::Wallet::Wallet::~Wallet()
{

}

lxqt::Wallet::Wallet * lxqt::Wallet::Wallet::getWalletBackend( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internal ){
		return new lxqt::Wallet::internalWallet() ;
	}else{
		return NULL ;
	}
}

bool lxqt::Wallet::Wallet::backEndIsSupported( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internal ){
		return true ;
	}else{
		return false ;
	}
}

void lxqt::Wallet::Wallet::deleteAWallet( const QString& walletName,const QString& applicationName )
{
	lxqt_wallet_delete_wallet( walletName.toAscii().constData(),applicationName.toAscii().constData() ) ;
}


