#ifndef LXQT_KWALLET_H
#define LXQT_KWALLET_H

#include "lxqt_wallet_interface.h"

#include <QString>
#include <QByteArray>
#include <QDebug>

#include <kwallet.h>

namespace lxqt{

namespace Wallet{

class kwallet : public lxqt::Wallet::Wallet
{
	Q_OBJECT
public:
	kwallet() ;
	~kwallet() ;
	bool addKey( const QString& key,const QByteArray& value ) ;
	bool open( const QString& walletName,const QString& applicationName,const QString& password = QString() ) ;
	QByteArray readValue( const QString& key ) ;
	QVector<lxqt::Wallet::walletKeyValues> readAllKeyValues( void ) ;
	QStringList readAllKeys( void ) ;
	void deleteKey( const QString& key ) ;
	void deleteWallet( void )  ;
	bool walletExists( const QString& walletName,const QString& applicationName ) ;
	int walletSize( void )  ;
	void closeWallet( bool ) ;
	lxqt::Wallet::walletBackEnd backEnd( void ) ;
	bool walletIsOpened( void ) ;
	void setAParent( QObject * parent ) ;
	QObject * qObject( void ) ;
	QString storagePath( void ) ;
signals:
	void walletIsOpen( bool ) ;
private slots:
	void walletOpened( bool ) ;
private:
	KWallet::Wallet * m_kwallet ;
	QString m_walletName ;
	QString m_applicationName ;
	QString m_password ;
};

}

}

#endif // LXQT_KWALLET_H
