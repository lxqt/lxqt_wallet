/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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
	internalBackEnd,
	kwalletBackEnd,
	gnomeKeyringBackEnd
}walletBackEnd;

/*
 * forward declare the Wallet class
 */
class Wallet ;

/*
 * check if there is a support for a backend and return true if the back end is supported
 */
Q_DECL_EXPORT bool backEndIsSupported( lxqt::Wallet::walletBackEnd ) ;

/*
 * delete a wallet
 */
Q_DECL_EXPORT void deleteAWallet( const QString& walletName,const QString& applicationName ) ;

/*
 * get a pointer to a requested backend to be used to gain access to the API.It is advised to call
 * backEndIsSupported() to check if a backed is supported before calling this function.
 * 0 is returned if there is no support for requested backend.
 * A caller is responsible for the returned object and must delete it when done with it
 */
Q_DECL_EXPORT lxqt::Wallet::Wallet * getWalletBackend( lxqt::Wallet::walletBackEnd = lxqt::Wallet::internalBackEnd ) ;

/*
 * Below class is the interface that implements various backends.
 */

class Wallet : public QObject
{
public:
	Wallet() ;
	virtual ~Wallet() ;

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
	 * close the a wallet
	 */
	virtual void closeWallet( bool ) = 0 ;

	/*
	 * return the backend in use
	 */
	virtual lxqt::Wallet::walletBackEnd backEnd( void ) = 0 ;

	/*
	 * check if a valid is opened or not
	 */
	virtual bool walletIsOpened( void ) = 0 ;

	/*
	 * return QObject pointer of the backend,not sure why you would want this as communication btw a backend and a user of the
	 * API is done through setInterfaceObject() method.
	 */
	virtual QObject * qObject( void ) = 0 ;

	/*
	 * Behavior of the method according to different back ends.
	 *
	 * gnome keyring - backend not implemented yet
	 *
	 * kwallet:
	 * walletName argument corresponds to the same thing in KWAllet API
	 * applicationName argument corresponds to password folder in KWallet API,default value will set passwordFolder to KDE's default.
	 * password argument is not used
	 *
	 * This back end requires an object to be passed using "setInterfaceObject()" method of this API and the object must have a slot named
	 * "void walletIsOpen(bool)".The slot will be called with "true" if the wallet was opened and with "false" otherwise.
	 *
	 * Calling this open() method will generate a KWallet GUI prompt for a password.
	 *
	 * The return value of this method with KWallet backend is undefined
	 *
	 * internal:
	 * walletName argument is the name of the wallet to open.
	 * applicationName argument is the name of the program that owns the wallet.
	 *
	 * If password argument is given,the method will return true if the wallet is opened and false other wise."walletIsOpen" signal will not
	 * be generated.
	 * If password argument is not given,a GUI window will be generated to ask the user for the password.
	 * If password argument is not given,the return value of the method is undefined.
	 *
	 * This back end requires an object to be passed using "setInterfaceObject()" method of this API and the object must have a slot named
	 * "void walletIsOpen(bool)".The slot will be called with "true" if the wallet was opened and with "false" otherwise.
	 *
	 * Calling this open() method without a password will generate a GUI prompt for a password
	 */
	virtual bool open( const QString& walletName,const QString& applicationName = QString(),const QString& password = QString() ) = 0 ;

	/*
	 * This method is used as a mean of communication between the backend and the user of the library.see open() method documentation above
	 * for a use case of this API
	 */
	virtual void setInterfaceObject( QObject * ) = 0 ;

	/*
	 * this method returns PasswordFolder() in kwallet backend and is undefined in other backends
	 */
	virtual QString storagePath( void ) = 0 ;

	/*
	 * change the wallet key to newWalletKey
	 */
	virtual void changeWalletPassWord( const QString& walletName,const QString& applicationName = QString() ) = 0 ;
};

} // namespace lxqt

} // namespace wallet

#endif // LXQT_WALLET_INTERFACE_H

