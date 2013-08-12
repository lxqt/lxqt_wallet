#ifndef LXQT_WALLET_INTERFACE_H
#define LXQT_WALLET_INTERFACE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QStringList>

namespace lxqt{

namespace Wallet{

class walletKeyValues{
public:
	QString key ;
	QByteArray value ;
};

typedef enum{
	internal,
	kwallet,
	gnomeKeyring
}walletBackEnd;

class Wallet : public QObject
{
	Q_OBJECT
public:
	Wallet() ;
	~Wallet() ;
	/*
	 * get a pointer to a requested backend.
	 * NULL is returned if there is no support for requested backend.
	 * A caller is responsible for the returned object and must delete it when done with it
	 */
	static lxqt::Wallet::Wallet * getWalletBackend( lxqt::Wallet::walletBackEnd = lxqt::Wallet::internal ) ;

	/*
	 * check if there is a support for a backend and return true if the back end is supported
	 */
	static bool backEndIsSupported( lxqt::Wallet::walletBackEnd ) ;

	/*
	 * delete a wallet
	 *
	 */
	static void deleteAWallet( const QString& walletName,const QString& applicationName ) ;

	/*
	 * add an entry to the wallet
	 */
	virtual bool addKey( const QString& key,const QByteArray& value ) = 0 ;

	/*
	 * get a value through a key
	 */
	virtual QByteArray readValue( const QString& key ) = 0 ;

	/*
	 * get all keys and their respective values from the wallet
	 */
	virtual QVector<lxqt::Wallet::walletKeyValues> readAllKeyValues( void ) = 0 ;

	/*
	 * get all keys in the wallet
	 */
	virtual QStringList readAllKeys( void ) = 0 ;
	/*
	 * delete a key in a wallet
	 */
	virtual void deleteKey( const QString& key ) = 0 ;

	/*
	 * delete a wallet
	 */
	virtual void deleteWallet( void ) = 0 ;

	/*
	 * check if a particular wallet exists
	 */
	virtual bool walletExists( const QString& walletName,const QString& applicationName ) = 0 ;

	/*
	 * if the backend is opened,return the number of entries in the wallet
	 */
	virtual int walletSize( void ) = 0 ;

	/*
	 * close the backend
	 */
	virtual void close( void ) = 0 ;

	/*
	 * return the backend in use
	 */
	virtual lxqt::Wallet::walletBackEnd backEnd( void ) = 0 ;

	/*
	 * check if a valid is opened or not
	 */
	virtual bool walletIsOpened( void ) = 0 ;

	/*
	 * return QObject pointer of the backend,this can be used as a hacky way to get to additional functionality of backends
	 * not supported through the public interface published here.
	 */
	virtual QObject * qObject( void ) = 0 ;

	/*
	 * If the password field is given,then the password will be used to open a wallet.
	 * If it is not set,then "setAParent()" must be called with a valid object and the object must
	 * have a slot named "void walletIsOpen( bool )".This slot will be called with "false" if the wallet
	 * could not be opened or true if the wallet is opened.
	 *
	 * The function will get the password from prompting the user with a GUI window
	 */
	virtual bool open( const QString& walletName,const QString& applicationName,const QString& password = QString() ) = 0 ;

	/*
	 * This function must be called with a valid object that has a slot with a signature of "void walletIsOpen( bool )" if "open()"
	 * was called without a password.
	 * The slot will be  called with "true" if the wallet is opened and with "false" if the wallet could not be opened.
	 */
	virtual void setAParent( QWidget * ) = 0 ;
};

} // namespace lxqt

} // namespace wallet

#endif // LXQT_WALLET_INTERFACE_H

