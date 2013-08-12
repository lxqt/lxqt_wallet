
#include "lxqt_wallet_interface.h"
#include "lxqt_internal_wallet.h"

#if HAS_KWALLET_SUPPORT
#include "lxqt_kwallet.h"
#endif

lxqt::Wallet::Wallet::Wallet()
{

}

lxqt::Wallet::Wallet::~Wallet()
{

}

lxqt::Wallet::Wallet * lxqt::Wallet::Wallet::getWalletBackend( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internalBackEnd ){
		return new lxqt::Wallet::internalWallet() ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return new lxqt::Wallet::kwallet();
		#else
			return NULL ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return new lxqt::Wallet::gnomeKeyring() ;
		#else
			return NULL ;
		#endif
	}

	return NULL ;
}

bool lxqt::Wallet::Wallet::backEndIsSupported( lxqt::Wallet::walletBackEnd bk )
{
	if( bk == lxqt::Wallet::internalBackEnd ){
		return true ;
	}

	if( bk == lxqt::Wallet::kwalletBackEnd ){
		#if HAS_KWALLET_SUPPORT
			return true ;
		#else
			return false ;
		#endif
	}

	if( bk == lxqt::Wallet::gnomeKeyringBackEnd ){
		#if HAS_GNOME_KEYRING_SUPPORT
			return true ;
		#else
			return false ;
		#endif
	}

	return false ;
}

void lxqt::Wallet::Wallet::deleteAWallet( const QString& walletName,const QString& applicationName )
{
	lxqt_wallet_delete_wallet( walletName.toAscii().constData(),applicationName.toAscii().constData() ) ;
}



