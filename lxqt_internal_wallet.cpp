#include "lxqt_internal_wallet.h"

lxqt::Wallet::internalWallet::internalWallet() : m_wallet( 0 )
{
}

lxqt::Wallet::internalWallet::~internalWallet()
{
	lxqt_wallet_close( &m_wallet ) ;
}

bool lxqt::Wallet::internalWallet::addKey( const QString& key,const QByteArray& value )
{
	lxqt_wallet_add_key( m_wallet,key.toAscii().constData(),value.constData(),value.size() ) ;
	return true ;
}

bool lxqt::Wallet::internalWallet::openWallet()
{
	lxqt_wallet_error r = lxqt_wallet_open( &m_wallet,m_password.toAscii().constData(),m_password.size(),
		      m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
	return r == lxqt_wallet_no_error ;
}

bool lxqt::Wallet::internalWallet::openWallet( QString password )
{
	m_password = password ;
	if( this->walletExists( m_walletName,m_applicationName ) ){
		lxqt_wallet_error r = lxqt_wallet_open( &m_wallet,password.toAscii().constData(),password.size(),
			      m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;

		bool z = ( r == lxqt_wallet_no_error ) ;
		emit walletIsOpen( z ) ;
		emit passwordIsCorrect( z ) ;
		return z ;
	}else{
		if( this->createWallet() ){
			lxqt_wallet_error r = lxqt_wallet_open( &m_wallet,password.toAscii().constData(),password.size(),
				      m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;

			bool z = ( r == lxqt_wallet_no_error ) ;
			emit walletIsOpen( z ) ;
			emit passwordIsCorrect( z ) ;
			return z ;
		}else{
			return false ;
		}
	}
}

void lxqt::Wallet::internalWallet::cancelled()
{
	emit walletIsOpen( false ) ;
}

bool lxqt::Wallet::internalWallet::createWallet( void )
{
	QWidget widget ;
	QMessageBox msg( &widget ) ;

	msg.setText( tr( "wallet \"%1\" does not exist.\nDo you want to create it?" ).arg( m_walletName ) ) ;

	msg.addButton( tr( "yes" ),QMessageBox::YesRole ) ;
	QPushButton * no_button = msg.addButton( tr( "no" ),QMessageBox::NoRole ) ;
	msg.setDefaultButton( no_button ) ;

	msg.exec() ;

	if( msg.clickedButton() != no_button ){
		lxqt_wallet_create( m_password.toAscii().constData(),m_password.size(),
				    m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
		return true ;
	}else{
		return false ;
	}
}

bool lxqt::Wallet::internalWallet::open( const QString& walletName,const QString& applicationName,const QString& password )
{
	m_walletName        = walletName ;
	m_applicationName   = applicationName ;
	m_password          = password ;

	if( m_applicationName.isEmpty() ){
		m_applicationName = m_walletName ;
	}

	if( m_password.isEmpty() ){
		password_dialog * p = new password_dialog() ;
		connect( p,SIGNAL( password( QString ) ),this,SLOT( openWallet( QString ) ) ) ;
		connect( this,SIGNAL( passwordIsCorrect( bool ) ),p,SLOT( passwordIsCorrect( bool ) ) ) ;
		connect( p,SIGNAL( cancelled() ),this,SLOT( cancelled() ) ) ;
		p->ShowUI( m_walletName,m_applicationName ) ;
		return false ;
	}else{
		if( this->walletExists( m_walletName,m_applicationName ) ){
			return this->openWallet() ;
		}else{
			if( this->createWallet() ){
				return this->openWallet() ;
			}else{
				return false ;
			}
		}
	}
}

QByteArray lxqt::Wallet::internalWallet::readValue( const QString& key )
{
	char * cvalue = NULL ;
	size_t value_size ;
	lxqt_wallet_read_key_value( m_wallet,key.toAscii().constData(),&cvalue,&value_size ) ;
	if( cvalue != NULL ){
		return QByteArray( cvalue,value_size ) ;
	}else{
		QByteArray b ;
		return b ;
	}
}

QVector<lxqt::Wallet::walletKeyValues> lxqt::Wallet::internalWallet::readAllKeyValues( void )
{
	const struct lxqt_key_value * r = lxqt_wallet_read_all_key_values( m_wallet ) ;

	QVector<walletKeyValues> w ;

	if( r == 0 ){
		return w ;
	}else{
		size_t j = lxqt_wallet_wallet_size( m_wallet ) ;
		walletKeyValues s ;
		for( size_t i = 0 ; i < j ; i++ ){
			s.key = QString( r[ i ].key ) ;
			s.value = QByteArray( r[ i ].value,r[ i ].value_size - 1 ) ;
			w.append( s ) ;
		}
		return w ;
	}
}

QStringList lxqt::Wallet::internalWallet::readAllKeys()
{
	const struct lxqt_key_value * r = lxqt_wallet_read_all_key_values( m_wallet ) ;

	QStringList l ;
	if( r == 0 ){
		return l ;
	}else{
		size_t j = lxqt_wallet_wallet_size( m_wallet ) ;
		walletKeyValues s ;
		for( size_t i = 0 ; i < j ; i++ ){
			l.append( QString( r[ i ].key ) ) ;
		}
		return l ;
	}
}

void lxqt::Wallet::internalWallet::deleteKey( const QString& key )
{
	lxqt_wallet_delete_key( m_wallet,key.toAscii().constData() ) ;
}

void lxqt::Wallet::internalWallet::deleteWallet( void )
{
	lxqt_wallet_delete_wallet( m_walletName.toAscii().constData(),m_applicationName.toAscii().constData() ) ;
}

bool lxqt::Wallet::internalWallet::walletExists( const QString& walletName,const QString& applicationName )
{
	return lxqt_wallet_exists( walletName.toAscii().constData(),applicationName.toAscii().constData() ) == 0 ;
}

int lxqt::Wallet::internalWallet::walletSize( void )
{
	return lxqt_wallet_wallet_size( m_wallet ) ;
}

void lxqt::Wallet::internalWallet::closeWallet( bool b )
{
	Q_UNUSED( b ) ;
	lxqt_wallet_close( &m_wallet ) ;
}

lxqt::Wallet::walletBackEnd lxqt::Wallet::internalWallet::backEnd()
{
	return lxqt::Wallet::internalBackEnd ;
}

bool lxqt::Wallet::internalWallet::walletIsOpened()
{
	return m_wallet != 0 ;
}

void lxqt::Wallet::internalWallet::setAParent( QObject * parent )
{
	if( parent ){
		this->setParent( parent ) ;
		connect( this,SIGNAL( walletIsOpen( bool ) ),parent,SLOT( walletIsOpen( bool ) ) ) ;
	}
}

QObject * lxqt::Wallet::internalWallet::qObject()
{
	return static_cast< QObject *>( this ) ;
}

QString lxqt::Wallet::internalWallet::storagePath()
{
	return QString() ;
}

