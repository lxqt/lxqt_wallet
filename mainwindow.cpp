#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget * parent ) :QMainWindow( parent ),m_ui( new Ui::MainWindow )
{
	m_ui->setupUi( this );
}

MainWindow::~MainWindow()
{
	delete m_ui;
	m_wallet->deleteLater() ;
}


void MainWindow::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void MainWindow::walletIsOpen( bool walletIsOpen )
{
	if( walletIsOpen ){
		QVector<lxqt::Wallet::walletKeyValues> s = m_wallet->readAllKeyValues() ;
		qDebug() << "list size" << s.size() ;
		size_t j = s.size() ;
		for( size_t i = 0 ; i < j ; i++ ){
			qDebug() << "key=" << s.at( i ).key << ":value=" << s.at( i ).value ;
		}
		QStringList l = m_wallet->readAllKeys() ;
		qDebug() << l ;
		m_wallet->close();
	}else{
		qDebug() << "failed to open wallet" ;
	}
}

void MainWindow::run()
{
	this->show() ;
	m_wallet = lxqt::Wallet::Wallet::getWalletBackend() ;
	m_wallet->setAParent( this ) ;

	bool b = m_wallet->open( "wallet_name","application_name" ) ;

	return ;

	if( b ){
		QVector<lxqt::Wallet::walletKeyValues> s = m_wallet->readAllKeyValues() ;
		qDebug() << "list size" << s.size() ;
		size_t j = s.size() ;
		for( size_t i = 0 ; i < j ; i++ ){
			qDebug() << "key=" << s.at( i ).key << ":value=" << s.at( i ).value ;
		}
		m_wallet->close();
	}else{
		qDebug() << "failed to open wallet" ;

	}

	//m_wallet->deleteLater();

	//QCoreApplication::exit(0) ;
}
