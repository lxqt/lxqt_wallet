#ifndef LXQT_WALLET_INTERFACE_H
#define LXQT_WALLET_INTERFACE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QByteArray>
#include <QVector>

namespace lxqt{

namespace Wallet{

class walletKeyValues{
	QString key ;
	QString value ;
};

typedef enum{
	internal,
	kwallet,
	gnomeKeyring
}walletBackEnds;

class Wallet
{
public:
	Wallet() ;
	~Wallet() ;
	static lxqt::Wallet::Wallet * getWalletBackend( lxqt::Wallet::walletBackEnds = lxqt::Wallet::internal ) ;
	static bool backEndIsSupported( lxqt::Wallet::walletBackEnds ) ;
	virtual bool addKey( const QString& key,const QByteArray& value ) = 0 ;
	virtual bool open( const QString& walletName,const QString applicationName ) = 0 ;
	virtual QByteArray readValue( const QString& key ) = 0 ;
	virtual QVector<lxqt::Wallet::walletKeyValues> readAllKeyValues( void ) = 0 ;
	virtual void deleteKey( const QString& key ) = 0 ;
	virtual void deleteWallet( const QString& walletName,const QString& applicationName ) = 0 ;
	virtual bool walletExists( const QString& walletName,const QString& applicationName ) = 0 ;
	virtual int walletSize( void ) = 0 ;
};

}

}
#endif // LXQT_WALLET_INTERFACE_H

