#ifndef LXQT_INTERNAL_WALLET_H
#define LXQT_INTERNAL_WALLET_H

#include "lxqt_wallet_interface.h"
#include "lxqtwallet.h"
#include "password_dialog.h"

#include <QString>
#include <QByteArray>
#include <QDebug>

#include "password_dialog.h"

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
	void close( void ) ;
	lxqt::Wallet::walletBackEnd backEnd( void ) ;
	bool walletIsOpened( void ) ;
	void setAParent( QWidget * parent ) ;
	QObject * qObject( void ) ;
signals:
	void walletIsOpen( bool ) ;
	void passwordIsCorrect( bool ) ;
private slots:
	bool openWallet( QString ) ;
	void cancelled( void ) ;
private:
	bool openWallet( void ) ;
	lxqt_wallet_t m_wallet ;
	QString m_walletName ;
	QString m_applicationName ;
	QString m_password ;
	QWidget * m_widgetParent ;
};

}

}
#endif // LXQT_INTERNAL_WALLET_H
