#include "main_application.h"

main_application::main_application()
{
}

void main_application::start()
{
	QMetaObject::invokeMethod( this,"run",Qt::QueuedConnection ) ;
}

void main_application::run()
{
	lxqt::Wallet::Wallet * w = lxqt::Wallet::Wallet::getWalletBackend() ;

	if( w ){
		qDebug() << "ff";
		qDebug() << w->walletSize() ;
	}else{
		qDebug() << "ww";
	}

	QCoreApplication::exit(0) ;
}
