#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QCoreApplication>
#include <QMainWindow>

#include "../frontend/lxqt_wallet_interface.h"
#include "../backend/lxqtwallet.h"
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow( QWidget * parent = 0 ) ;
	~MainWindow();
	void start( void ) ;
private slots:
	void run( void ) ;
	void walletIsOpen( bool ) ;
private:
	lxqt::Wallet::Wallet * m_wallet ;
private:
	Ui::MainWindow * m_ui ;
};

#endif // MAINWINDOW_H
