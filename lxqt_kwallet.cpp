#include "lxqt_kwallet.h"

lxqt::Wallet::kwallet::kwallet()
{

}

lxqt::Wallet::kwallet::~kwallet()
{
	m_kwallet->deleteLater() ;
}

bool lxqt::Wallet::kwallet::addKey( const QString& key,const QByteArray& value )
{
	m_kwallet->writePassword( key,value ) ;
	return true ;
}

bool lxqt::Wallet::kwallet::open( const QString& walletName,const QString& applicationName,const QString& password )
{
	m_walletName        = walletName ;
	m_applicationName   = applicationName ;
	m_password          = password ;

	m_kwallet = KWallet::Wallet::openWallet( m_walletName,0,KWallet::Wallet::Asynchronous ) ;

	connect( m_kwallet,SIGNAL( walletOpened( bool ) ),this,SLOT( walletOpened( bool ) ) ) ;

	return false ;
}

void lxqt::Wallet::kwallet::walletOpened( bool opened )
{
	connect( this,SIGNAL( walletIsOpen( bool ) ),this->parent(),SLOT( walletIsOpen( bool ) ) ) ;

	if( opened ){
		if( m_applicationName.isEmpty() ){
			m_kwallet->createFolder( m_kwallet->PasswordFolder() ) ;
			m_kwallet->setFolder( m_kwallet->PasswordFolder() ) ;
		}else{
			m_kwallet->createFolder( m_applicationName ) ;
			m_kwallet->setFolder( m_applicationName ) ;
		}
	}

	emit walletIsOpen( opened ) ;
}

QByteArray lxqt::Wallet::kwallet::readValue( const QString& key )
{
	QString value ;
	m_kwallet->readPassword( key,value ) ;
	return value.toAscii() ;
}

QVector<lxqt::Wallet::walletKeyValues> lxqt::Wallet::kwallet::readAllKeyValues( void )
{
	QVector<lxqt::Wallet::walletKeyValues> p ;
	lxqt::Wallet::walletKeyValues q ;
	QStringList l = m_kwallet->entryList() ;
	QString value ;
	int j = l.size() ;

	for( int i = 0 ; i < j ; i++ ){
		m_kwallet->readPassword( l.at( i ),value ) ;
		q.key = l.at( i ) ;
		q.value = value.toAscii() ;
		p.append( q ) ;
	}
	return p ;
}

QStringList lxqt::Wallet::kwallet::readAllKeys( void )
{
	return m_kwallet->entryList() ;
}

void lxqt::Wallet::kwallet::deleteKey( const QString& key )
{
	m_kwallet->removeEntry( key ) ;
}

void lxqt::Wallet::kwallet::deleteWallet( void )
{
	m_kwallet->deleteWallet( m_walletName ) ;
}

bool lxqt::Wallet::kwallet::walletExists( const QString& walletName,const QString& applicationName )
{
	Q_UNUSED( walletName ) ;
	Q_UNUSED( applicationName ) ;
	return false ;
}

int lxqt::Wallet::kwallet::walletSize( void )
{
	QStringList l = m_kwallet->entryList() ;
	return l.size() ;
}

void lxqt::Wallet::kwallet::closeWallet( bool b )
{
	m_kwallet->closeWallet( m_walletName,b ) ;
}

lxqt::Wallet::walletBackEnd lxqt::Wallet::kwallet::backEnd( void )
{
	return lxqt::Wallet::kwalletBackEnd ;
}

bool lxqt::Wallet::kwallet::walletIsOpened( void )
{
	return m_kwallet->isOpen() ;
}

void lxqt::Wallet::kwallet::setAParent( QObject * parent )
{
	this->setParent( parent ) ;
}

QObject * lxqt::Wallet::kwallet::qObject( void )
{
	return static_cast< QObject * >( this ) ;
}

QString lxqt::Wallet::kwallet::storagePath()
{
	return m_kwallet->PasswordFolder() ;
}
