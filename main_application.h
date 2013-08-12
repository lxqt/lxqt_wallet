#ifndef MAIN_APPLICATION_H
#define MAIN_APPLICATION_H

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>

#include "lxqt_wallet_interface.h"
#include "lxqtwallet.h"
#include <QDebug>

class main_application : public QObject
{
	Q_OBJECT
public:
	main_application();
	void start( void ) ;
private slots:
	void run( void ) ;
private:
};

#endif // MAIN_APPLICATION_H
