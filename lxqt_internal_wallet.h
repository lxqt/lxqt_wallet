#ifndef LXQT_INTERNAL_WALLET_H
#define LXQT_INTERNAL_WALLET_H

#include "lxqt_wallet_interface.h"
#include "lxqtwallet.h"
#include "password_dialog.h"

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include "password_dialog.h"
#include <QPushButton>

namespace lxqt{

namespace Wallet{

class internalWallet : public lxqt::Wallet::Wallet
{
	Q_OBJECT
public:
	internalWallet() ;
	~internalWallet() ;
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
	void passwordIsCorrect( bool ) ;
private slots:
	bool openWallet( QString ) ;
	void cancelled( void ) ;
	void createAWallet( bool ) ;
	void createAWallet( QString ) ;
private:
	bool openWallet( void ) ;
	lxqt_wallet_t m_wallet ;
	QString m_walletName ;
	QString m_applicationName ;
	QString m_password ;
};

}

}
#endif // LXQT_INTERNAL_WALLET_H
